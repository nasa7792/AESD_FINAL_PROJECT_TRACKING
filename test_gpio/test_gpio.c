#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <syslog.h>

#define GPIO_BASE_ADDRESS 0xFE200000
#define BLOCK_SIZE (4 * 1024)
#define GPIOSET0_INDEX 7
#define GPIOCLR0_INDEX 10
#define GPIO_TEST 17

volatile unsigned int *gpio = NULL;

void setup_memory_mapped_gpio()
{
    int mem_fd;
    void *gpio_map;
    if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0)
    {
        syslog(LOG_ERR, "unable to open /dev/mem try running as sudo");
        exit(-1);
    }
    gpio_map = mmap(
        NULL,
        BLOCK_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        mem_fd,
        GPIO_BASE_ADDRESS);

    close(mem_fd);

    if (gpio_map == MAP_FAILED)
    {
        perror("mmap error");
        exit(EXIT_FAILURE);
    }

    gpio = (volatile unsigned int *)gpio_map;

    // assign pin as output
    int reg_index = GPIO_TEST % 10;
    int bit_index_start = (GPIO_TEST % 10) * 3;
    // clear test pin's bits
    gpio[reg_index] = gpio[reg_index] & ~(7 << bit_index_start);
    // make 001
    gpio[reg_index] = gpio[reg_index] | (1 << bit_index_start);
}

void gpio_write(int status)
{
     syslog(LOG_INFO, "status %d",status);
    if (status == 1)
        gpio[GPIOSET0_INDEX] = (1 << GPIO_TEST);
    else
        gpio[GPIOCLR0_INDEX] = (1 << GPIO_TEST);
}

int main()
{
    setup_memory_mapped_gpio();
    openlog("TEST_GPIO", LOG_PID | LOG_PERROR, LOG_USER);
    while(1){
    gpio_write(1);
    sleep(1);
    gpio_write(0);
    sleep(1);
    }
}