CC=gcc
CFLAGS=-I. -g -Wall 
#-Wall -Wextra -Werror -g
#-O2 -Wall -g
# -g -> with debug info

APP = sgbd-t1
SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)

$(APP): $(OBJS)
	$(CC) -o $(APP) $(OBJS) $(CFLAGS)
	@echo "Compiled ["$@"] successfully!"

src/main.o: src/main.c src/hash_index.h
	$(CC) -c src/main.c -o src/main.o $(CFLAGS)

src/hash_index.o: src/hash_index.c src/hash_index.h
	$(CC) -c src/hash_index.c -o src/hash_index.o $(CFLAGS)

run: $(APP)
	./$(APP)

clean:
	rm -f $(APP) $(OBJS) buckets/*.txt

