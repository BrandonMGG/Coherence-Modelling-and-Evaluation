CC=gcc
CFLAGS=-Wall
inc=../../include/messages.h
all: consumer controller

consumer: messages.c consumer.c $(inc)
	$(CC) $(CFLAGS) -o consumer messages.c consumer.c -lrt -lpthread

controller: messages.c controller.c $(inc)
	$(CC) $(CFLAGS) -o controller messages.c controller.c -lrt -lpthread

clean:
	rm -f consumer controller