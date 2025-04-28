#include <stdio.h>
#include <pigpio.h>
#include <unistd.h> 
#include <stdlib.h> 
#include <string.h> 
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>

#define MOTOR_GPIO 18 //18 is pwm pin
#define PWM_FREQ 800
#define PROX_PIPE "/tmp/proxpipe" //path of pipe 

int get_proximity()
{
    int fd = open(PROX_PIPE, O_RDONLY);
    if (!fd)
    {
        syslog(LOG_ERR,"Failed to open proximity pipe");
        return -1; // return -1 in case of error
    }

    char buffer[100];
    memset(buffer, 0, sizeof(buffer));

    //read data from pipe
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);
    if (bytesRead <= 0)
    {
        syslog(LOG_ERR, "Failed to read from pipe");
        close(fd);
        return -1;
    }
    buffer[bytesRead] = '\0'; 
    close(fd);
    //convert to int and return s
    return atoi(buffer);
}

int get_pwm(int proximity)
{
    if (proximity >= 1 && proximity <= 100)
    {
        return 1000000; 
    }
    else if (proximity > 100 && proximity <= 300)
    {
        return 500000; // 50%
    }
    else if (proximity > 300 && proximity <= 500)
    {
        return 300000; // 30%
    }
    else
    {
        return 0; //beyond this stop the motor!
    }
}

int main()
{
    openlog("MOTOR_LOGS", LOG_PID | LOG_PERROR, LOG_USER);
    if (gpioInitialise() < 0) {
        syslog(LOG_ERR, "Pigpio initialization failed");
        return 1;
    }

    while (1)
    {
        // get value from pipe
        int proximity = get_proximity();
        if (proximity == -1)
        {
            syslog(LOG_ERR,"Error reading from pipe");
            continue; 
        }
        
        //get pwm duty cycle based on proximity data
        int pwm_value = get_pwm(proximity);

        // Set the PWM value
        gpioHardwarePWM(MOTOR_GPIO, PWM_FREQ, pwm_value);

        sleep(1); // fetch next value
    }
    return 0;
}
