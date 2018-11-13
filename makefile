# make file for bakery algorithm

OBJS	= main.o
CC  	= gcc -pthread

all: bakery

main.o: main.c
	$(CC) -c main.c

bakery: main.o
	$(CC) -o bakery main.o

clean:
	rm  	$(OBJS)
