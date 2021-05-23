# makefile for vmm

CC=gcc
CFLAGS=-Wall

all: vmm.o judge.o
	$(CC) $(CFLAGS) -o vmm vmm.o
	$(CC) $(CFLAGS) -o judge judge.o

vmm.o: vmm.c
	$(CC) $(CFLAGS) -c vmm.c

judge.o: judge.c
	$(CC) $(CFLAGS) -c judge.c

clean:
	rm -rf *.o
	rm -rf vmm
	rm -rf judge