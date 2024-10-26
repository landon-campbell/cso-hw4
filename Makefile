CC = gcc
CFLAGS = -Og -Wall
LDFLAGS = -fsanitize=address
AR = ar
ARFLAGS = rcs

LIB = libmlpt.a

.PHONY: all clean

all: $(LIB)

$(LIB): pagetable.o
	$(AR) $(ARFLAGS) $@ $^

pagetable.o: pagetable.c mlpt.h config.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ pagetable.c

clean:
	rm -f pagetable.o $(LIB)
