# Always use ARM cross-compiler
CROSS_COMPILE = arm-linux-gnueabihf-

CC=$(CROSS_COMPILE)gcc

# Default target is 'all'
all: test_gpio

# Rule to build the 'hello' binary
test_gpio: test_gpio.c
	$(CC) -o test_gpio test_gpio.c

# Clean up the generated files
.PHONY: clean
clean:
	rm -f *.o test_gpio
