CC = gcc
CFLAGS = -Wall -pedantic -Wextra -g
LDFLAGS = -lcrypto -lm

all: hash stream
run:
	./hash
	./stream < input.in
hash: hash.o
	$(CC) -o $@ $< $(LDFLAGS)
stream: stream.o
	$(CC) -o $@ $< $(LDFLAGS)
stream-sifrovani: stream-sifrovani.o
	$(CC) -o $@ $< $(LDFLAGS)
%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)
clean:
	rm *.o 2>/dev/null 1>&2	
