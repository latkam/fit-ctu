CC = gcc
CFLAGS = -Wall -pedantic -Wextra -g
LDFLAGS = -lcrypto

all: encrypt decrypt
run:
	./encrypt ./images/homer-simpson ecb
	./encrypt ./images/homer-simpson cbc
	./encrypt ./images/mad-scientist ecb
	./encrypt ./images/mad-scientist cbc
	./decrypt ./images/homer-simpson ecb
	./decrypt ./images/homer-simpson cbc
	./decrypt ./images/mad-scientist ecb
	./decrypt ./images/mad-scientist cbc
encrypt: encrypt.o
	$(CC) -o $@ $< $(LDFLAGS)
decrypt: decrypt.o
	$(CC) -o $@ $< $(LDFLAGS)
%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)
clean:
	rm *.o