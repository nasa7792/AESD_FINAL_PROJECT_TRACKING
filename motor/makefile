# Cross compile setup
CC ?= gcc

# Default target
all: motor

# Rule to build the 'motor' binary
motor: motor.c
	$(CC) -o motor motor.c  

# Clean up the generated files
.PHONY: clean
clean:
	rm -f *.o motor
