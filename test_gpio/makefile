#cross compile
CC ?= gcc

# Default target
all: test_gpio

# Rule to build the 'hello' binary
hello: hello.c
	$(CC) -o test_gpio test_gpio.c

# Clean up the generated files
.PHONY: clean
clean:
	rm -f *.o test_gpio
