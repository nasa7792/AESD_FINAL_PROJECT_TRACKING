#cross compile
CC ?= gcc

# Default target
all: vncl4040_sensor

# Rule to build the 'hello' binary
vncl4040_sensor: vncl4040_sensor.c
	$(CC) -o vncl4040_sensor vncl4040_sensor.c

# Clean up the generated files
.PHONY: clean
clean:
	rm -f *.o vncl4040_sensor
