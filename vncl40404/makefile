#cross compile
CC ?= gcc

# Default target
all: vncl4040

# Rule to build the 'hello' binary
hello: hello.c
	$(CC) -o vncl4040 vncl4040.c

# Clean up the generated files
.PHONY: clean
clean:
	rm -f *.o vncl4040
