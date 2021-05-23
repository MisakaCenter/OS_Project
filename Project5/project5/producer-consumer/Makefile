# makefile for producer-consumer
#

CC=gcc
CFLAGS=-Wall
PTHREADS=-lpthread

all: producer-consumer.o buffer.o
	$(CC) $(CFLAGS) -o producer-consumer buffer.o producer-consumer.o $(PTHREADS)

buffer.o: buffer.c
	$(CC) $(CFLAGS) -c buffer.c $(PTHREADS)

producer-consumer.o: producer-consumer.c
	$(CC) $(CFLAGS) -c producer-consumer.c $(PTHREADS)

clean:
	rm -rf *.o
	rm -rf producer-consumer