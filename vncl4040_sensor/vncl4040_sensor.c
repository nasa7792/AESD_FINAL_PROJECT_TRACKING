#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdlib.h>
#include <stdint.h>
#include <syslog.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>

#define I2C_DEVICE "/dev/i2c-1"
#define VCNL4040_ADDR 0x60
#define DEVICE_ID_REG 0x0C
#define PROXIMITY_REG 0x08
#define PS_CONF1_REG 0x03
#define FIFO_PATH "/tmp/proxpipe"
#define PINET_IOCTL_SEND_SENSOR_DATA _IOW('p', 1, int)

// Check if the sensor is connected
int check_sensor_connected(int fd)
{
    uint8_t reg = DEVICE_ID_REG;
    uint8_t data[2] = {0};

    struct i2c_msg messages[2] = {
        {.addr = VCNL4040_ADDR, .flags = 0, .len = 1, .buf = &reg},
        {.addr = VCNL4040_ADDR, .flags = I2C_M_RD, .len = 2, .buf = data}};

    struct i2c_rdwr_ioctl_data ioctl_data = {
        .msgs = messages,
        .nmsgs = 2};

    if (ioctl(fd, I2C_RDWR, &ioctl_data) < 0)
    {
        syslog(LOG_ERR, "I2C_RDWR ioctl failed");
        close(fd);
        return EXIT_FAILURE;
    }

    uint16_t device_id = (data[1] << 8) | data[0];
    syslog(LOG_INFO, "VCNL4040 Device ID: 0x%04X", device_id);
    return EXIT_SUCCESS;
}

// Enable proximity sensing
int enable_proximity(int fd)
{
    uint8_t buf[3];
    buf[0] = PS_CONF1_REG;
    buf[1] = 0x00;
    buf[2] = 0x00;

    if (write(fd, buf, 3) != 3)
    {
        syslog(LOG_ERR, "Failed to write to PS_CONF1_REG");
        return EXIT_FAILURE;
    }

    syslog(LOG_INFO, "Proximity mode enabled by writing 0x0000 to PS_CONF1_REG");
    return EXIT_SUCCESS;
}

// Read proximity data
int read_proximity_data(int fd)
{
    unsigned char reg = PROXIMITY_REG;
    unsigned char buf[2];

    struct i2c_msg messages[2] = {
        {VCNL4040_ADDR, 0, 1, &reg},
        {VCNL4040_ADDR, I2C_M_RD, 2, buf}};

    struct i2c_rdwr_ioctl_data ioctl_data = {messages, 2};
    if (ioctl(fd, I2C_RDWR, &ioctl_data) < 0)
    {
        syslog(LOG_ERR, "Failed to get proximity data");
        return -1;
    }

    unsigned int proximity = (buf[1] << 8) | buf[0];
    syslog(LOG_INFO, "Proximity: %d", proximity);
    return proximity;
}

int open_fifo_writer()
{
    int pipe_fd;
    while (1)
    {
        pipe_fd = open(FIFO_PATH, O_WRONLY);
        if (pipe_fd < 0)
        {
            syslog(LOG_ERR, "Waiting for hello reader to open FIFO: %s", strerror(errno));
            sleep(1); // Wait and retry
        }
        else
        {
            syslog(LOG_INFO, "FIFO opened for writing");
            break;
        }
    }
    return pipe_fd;
}

int main()
{
    openlog("I2C_LOG", LOG_PID | LOG_PERROR, LOG_USER);
    int fd = open(I2C_DEVICE, O_RDWR);
    syslog(LOG_ERR, "vncl says hello koppa!");
    if (fd < 0)
    {
        syslog(LOG_ERR, "Unable to open I2C device");
        exit(EXIT_FAILURE);
    }

    if (ioctl(fd, I2C_SLAVE, VCNL4040_ADDR) < 0)
    {
        syslog(LOG_ERR, "Failed to set I2C slave address");
        close(fd);
        return EXIT_FAILURE;
    }

    if (check_sensor_connected(fd) != 0)
    {
        close(fd);
        return EXIT_FAILURE;
    }

    if (enable_proximity(fd) != 0)
    {
        close(fd);
        return EXIT_FAILURE;
    }

    // Create FIFO if it doesn't exist
    if (access(FIFO_PATH, F_OK) == -1)
    {
        syslog(LOG_INFO, "FIFO does not exist, creating FIFO at %s", FIFO_PATH);
        if (mkfifo(FIFO_PATH, 0666) != 0)
        {
            syslog(LOG_ERR, "Failed to create FIFO: %s", strerror(errno));
            close(fd);
            closelog();
            return EXIT_FAILURE;
        }
    }
    else
    {
        syslog(LOG_INFO, "FIFO already exists at %s", FIFO_PATH);
    }

    int pipe_fd = open_fifo_writer();

    int pinet_file;

    pinet_file = open("/dev/pinet", O_RDWR);
    if (pinet_file < 0)
    {
        perror("open failed");
        return EXIT_FAILURE;
    }

    // Main loop: Read sensor and write to FIFO
    while (1)
    {
        int proximity = read_proximity_data(fd);
        if (proximity == -1)
        {
            syslog(LOG_ERR, "Failed to read proximity data");
            break;
        }

        // Write to FIFO, check for errors
        if (dprintf(pipe_fd, "%d\n", proximity) < 0)
        {
            syslog(LOG_ERR, "Write to hello FIFO failed: %s", strerror(errno));
            close(pipe_fd);
            syslog(LOG_INFO, "Attempting to reopen FIFO for writing...");
            pipe_fd = open_fifo_writer(); // Reopen FIFO when reader returns
            continue;
        }
        if (ioctl(pinet_file, PINET_IOCTL_SEND_SENSOR_DATA, &proximity) == -1)
        {
            perror("ioctl faileddddd");
        }
        fsync(pipe_fd); // Ensure data is flushed
        sleep(1);       // Sleep for 1 second before next read
    }

    close(pipe_fd);
    close(fd);
    closelog();
    return EXIT_SUCCESS;
}
