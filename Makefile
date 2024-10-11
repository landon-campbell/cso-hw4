CC = gcc
CFLAGS = -Og -Wall -fsanitize=address

all: pagetable

pagetable: pagetabletest.o pagetable.o
	$(CC) $(CFLAGS) -o pagetable pagetabletest.o pagetable.o

pagetabletest.o: pagetabletest.c mlpt.h config.h
	$(CC) $(CFLAGS) -c pagetabletest.c

pagetable.o: pagetable.c mlpt.h config.h
	$(CC) $(CFLAGS) -c pagetable.c

clean:
	rm -f pagetable pagetabletest.o pagetable.o
