CC = gcc
CFLAGS = -Og -Wall -fPIC
LDFLAGS = -fsanitize=address -shared

LIB = libmlpt.so

.phony: all clean

all: $(LIB)

$(LIB): pagetable.o
	$(CC) $(LDFLAGS) -o $@ $^

pagetable.o: pagetable.c mlpt.h config.h
	$(CC) $(CFLAGS) -c -o $@ pagetable.c

clean:
	rm -f pagetable.o $(LIB)
