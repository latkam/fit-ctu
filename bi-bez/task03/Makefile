CC = gcc
CFLAGS = -Wall -pedantic -Wextra -g
LDFLAGS = -lcrypto

all: encrypt decrypt
run:
	./encrypt ./data/data1 pubkey.pem
	./encrypt ./data/data2 pubkey.pem
	./encrypt ./data/data3 pubkey.pem
	./encrypt ./data/data4 pubkey.pem
	./encrypt ./data/data5 pubkey.pem
	./decrypt ./data/data1 privkey.pem
	./decrypt ./data/data2 privkey.pem
	./decrypt ./data/data3 privkey.pem
	./decrypt ./data/data4 privkey.pem
	./decrypt ./data/data5 privkey.pem
diff:
	diff ./data/data1.bin ./data/data1_dec.bin
	diff ./data/data2.bin ./data/data2_dec.bin
	diff ./data/data3.bin ./data/data3_dec.bin
	diff ./data/data4.bin ./data/data4_dec.bin
	diff ./data/data5.bin ./data/data5_dec.bin		
encrypt: encrypt.o
	$(CC) -o $@ $< $(LDFLAGS)
decrypt: decrypt.o
	$(CC) -o $@ $< $(LDFLAGS)
%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)
clean:
	rm -rf encrypt decrypt
