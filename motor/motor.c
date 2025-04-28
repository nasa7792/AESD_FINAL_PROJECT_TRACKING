//https://embetronicx.com/tutorials/linux/device-drivers/sysfs-in-linux-kernel/
//sources https://forums.raspberrypi.com/viewtopic.php?t=164998
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>

#define PWM_CHIP      "0"   
#define PWM_CHANNEL   "0"   
#define PROX_PIPE     "/tmp/proxpipe" //path to shared pipe

#define PWM_PERIOD 1250 

void pwm_export()
{
    int fd = open("/sys/class/pwm/pwmchip" PWM_CHIP "/export", O_WRONLY);
    if (fd < 0) {
        syslog(LOG_ERR, "Failed to open export");
        return;
    }
    write(fd, PWM_CHANNEL, strlen(PWM_CHANNEL));
    close(fd);
}

void pwm_unexport()
{
    int fd = open("/sys/class/pwm/pwmchip" PWM_CHIP "/unexport", O_WRONLY);
    if (fd >= 0) {
        write(fd, PWM_CHANNEL, strlen(PWM_CHANNEL));
        close(fd);
    }
}

void pwm_enable(int enable)
{
    char path[128];
    snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip%s/pwm%s/enable", PWM_CHIP, PWM_CHANNEL);

    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        syslog(LOG_ERR, "Failed to open enable");
        return;
    }
    dprintf(fd, "%d", enable);
    close(fd);
}

void pwm_set_period(int period_ns)
{
    char path[128];
    snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip%s/pwm%s/period", PWM_CHIP, PWM_CHANNEL);

    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        syslog(LOG_ERR, "Failed to open period");
        return;
    }
    dprintf(fd, "%d", period_ns);
    close(fd);
}

void pwm_set_duty_cycle(int duty_ns)
{
    char path[128];
    snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip%s/pwm%s/duty_cycle", PWM_CHIP, PWM_CHANNEL);

    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        syslog(LOG_ERR, "Failed to open duty_cycle");
        return;
    }
    dprintf(fd, "%d", duty_ns);
    close(fd);
}

int get_proximity()
{
    int fd = open(PROX_PIPE, O_RDONLY);
    if (fd < 0)
    {
        syslog(LOG_ERR, "Failed to open proximity pipe");
        return -1;
    }

    char buffer[100] = {0};
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);
    close(fd);

    if (bytesRead <= 0)
    {
        syslog(LOG_ERR, "Failed to read from pipe");
        return -1;
    }
    //convert to int and return
    return atoi(buffer);
}

int get_duty_cycle(int proximity)
{
    if (proximity >= 1 && proximity <= 100)
        return PWM_PERIOD * 100 / 100; // 100%
    else if (proximity > 100 && proximity <= 300)
        return PWM_PERIOD * 50 / 100;  // 50%
    else if (proximity > 300 && proximity <= 500)
        return PWM_PERIOD * 30 / 100;  // 30%
    else
        return 0; // beyond this stop the motor
}

int main()
{
    openlog("MOTOR_LOGS", LOG_PID | LOG_PERROR, LOG_USER);

    pwm_export();
    sleep(1); // wait for sysfs to create pwmX directory

    pwm_set_period(PWM_PERIOD);
    pwm_set_duty_cycle(0);
    pwm_enable(1);

    while (1)
    {   //read from pipe
        int proximity = get_proximity();
        if (proximity == -1)
        {
            syslog(LOG_ERR, "Error reading from pipe");
            continue;
        }

        int duty_cycle = get_duty_cycle(proximity);
        pwm_set_duty_cycle(duty_cycle);

        sleep(1);
    }

    pwm_enable(0);
    pwm_unexport();
    return 0;
}
