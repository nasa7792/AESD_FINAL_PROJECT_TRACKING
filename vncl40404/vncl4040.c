#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdlib.h>
#include <stdint.h>
#include <syslog.h>

#define I2C_DEVICE "/dev/i2c-1"
#define VCNL4040_ADDR 0x60
#define DEVICE_ID_REG 0x0C
#define PROXIMITY_REG 0x08    // proximity data register
#define PS_CONF1_REG 0x03    // Configuration register for Proximity Sensor

int check_sensor_connected(int fd)
{
	//for reading we need repeadted start operation
	uint8_t reg = DEVICE_ID_REG;
    uint8_t data[2] = {0};
    
    struct i2c_msg messages[2] = {
        {
            .addr = VCNL4040_ADDR,
            .flags = 0,  // Write
            .len = 1,
            .buf = &reg
        },
        {
            .addr = VCNL4040_ADDR,
            .flags = I2C_M_RD,  //flag for Read->https://sites.uclouvain.be/SystInfo/usr/include/linux/i2c.h.html basically just 1
            .len = 2,//need to read 2 bytes
            .buf = data
        }
    };
    
    //send ioctl call for repeated start read
        struct i2c_rdwr_ioctl_data ioctl_data = {
        .msgs = messages,
        .nmsgs = 2
    };

    if (ioctl(fd, I2C_RDWR, &ioctl_data) < 0) {
        syslog(LOG_ERR, "I2C_RDWR ioctl failed");
        close(fd);
        return EXIT_FAILURE;
    }
    
    uint16_t device_id = (data[1] << 8) | data[0];  //combine to get full device id should be 186
    syslog(LOG_INFO,"VCNL4040 Device ID: 0x%04X\n", device_id);	
}


int enable_proximity(int fd)
{
    uint8_t buf[3];

    buf[0] = PS_CONF1_REG;   // config register addres
    buf[1] = 0x00;          
    buf[2] = 0x00;           

    if (write(fd, buf, 3) != 3) {
        syslog(LOG_ERR, "Failed to write to PS_CONF1_REG");
        return EXIT_FAILURE;
    }

    syslog(LOG_INFO, "Proximity mode enabled by writing 0x0000 to PS_CONF1_REG");
    return EXIT_SUCCESS;
}


int read_proximity_data(int file) {
    unsigned char reg = PROXIMITY_REG; // Proximity register address
    unsigned char buf[2];  // Buffer to hold the 2-byte proximity data

    struct i2c_msg messages[2] = {
        { VCNL4040_ADDR, 0, 1, &reg },  // Write register address
        { VCNL4040_ADDR, I2C_M_RD, 2, buf }  // Read 2 bytes of proximity data
    };

    struct i2c_rdwr_ioctl_data ioctl_data = { messages, 2 };
    if (ioctl(file, I2C_RDWR, &ioctl_data) < 0) {
        perror("Failed to get proximity data");
        return -1;
    }

    unsigned int proximity = (buf[1] << 8) | buf[0];
    syslog(LOG_INFO,"Proximity: %d\n", proximity);
    return 0;
}

int main()

{
	openlog("I2C_LOG", LOG_PID | LOG_PERROR, LOG_USER);
	int fd=open(I2C_DEVICE,O_RDWR);
	if(fd<0)
	{
		syslog(LOG_ERR,"unable to open i2c device");
		exit(EXIT_FAILURE);
	}
	
	// Set suboordinate address
    if (ioctl(fd, I2C_SLAVE, VCNL4040_ADDR) < 0) {
        syslog(LOG_ERR, "Failed to set I2C slave address");
        close(fd);
        return EXIT_FAILURE;
    }
	//check for sensor status
	check_sensor_connected(fd);
	//enable the proximity mode
	  if (enable_proximity(fd) != 0) {
        close(fd);
        return EXIT_FAILURE;
    }
    //start reading the proximity values
    while(1)
    {
	if(read_proximity_data(fd)!=0)
	{
	    close(fd);
        return EXIT_FAILURE;
	}	
	sleep(1);
	}
    
    
    close(fd);
    closelog();
    return EXIT_SUCCESS;
}
