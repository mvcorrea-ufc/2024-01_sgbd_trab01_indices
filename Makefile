CC=gcc
CFLAGS=-I. 
#-Wall -Wextra -Werror -g

sgbd-t1: src/main.o src/hash_index.o
	$(CC) -o sgbd-t1 src/main.o src/hash_index.o $(CFLAGS)

src/main.o: src/main.c src/hash_index.h
	$(CC) -c src/main.c -o src/main.o $(CFLAGS)

src/hash_index.o: src/hash_index.c src/hash_index.h
	$(CC) -c src/hash_index.c -o src/hash_index.o $(CFLAGS)

run:
	./sgbd-t1

clean:
	rm -f src/*.o sgbd-t1 buckets/*.txt

