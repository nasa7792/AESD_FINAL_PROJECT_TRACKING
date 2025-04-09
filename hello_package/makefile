#check for cross compile flag if set use cross comiple
ifdef CROSS_COMPILE
      CC=$(CROSS_COMPILE)gcc
else
#else use gcc
    CC=gcc
endif

all: hello
hello: hello.c
	$(CC) -o hello hello.c
#clean .o and binary files
.PHONY: clean
clean:
	rm -f *.o writer