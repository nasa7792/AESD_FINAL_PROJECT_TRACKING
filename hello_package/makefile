# Always use ARM cross-compiler
CROSS_COMPILE = arm-linux-gnueabihf-

CC=$(CROSS_COMPILE)gcc

# Default target is 'all'
all: hello

# Rule to build the 'hello' binary
hello: hello.c
	$(CC) -o hello hello.c

# Clean up the generated files
.PHONY: clean
clean:
	rm -f *.o hello
