CC=gcc
CFLAGS=-Wall -g

all: myalloc

myalloc.o: myalloc.c
	$(CC) $(CFLAGS) -c myalloc.c

myalloc: myalloc.o
	$(CC) $(CFLAGS) -o myalloc myalloc.o

clean:
	rm -f *.o myalloc

