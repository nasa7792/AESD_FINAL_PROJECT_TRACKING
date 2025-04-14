#cross compile
CC ?= gcc

# Default target
all: hello

# Rule to build the 'hello' binary
hello: hello.c
	$(CC) -o hello hello.c

# Clean up the generated files
.PHONY: clean
clean:
	rm -f *.o hello
