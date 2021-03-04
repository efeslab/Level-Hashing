all: plevel

CFLAGS=-g -O0
CC=gcc

plevel: test.o level_hashing.o hash.o pflush.o log.o
	$(CC) $(CFLAGS) -o plevel test.o level_hashing.o hash.o pflush.o log.o -lm -lpmem -lpmemobj

hash.o: hash.c hash.h
	$(CC) $(CFLAGS) -c hash.c
	
level_hashing.o: level_hashing.c level_hashing.h
	$(CC) $(CFLAGS) -c level_hashing.c

test.o: test.c level_hashing.h
	$(CC) $(CFLAGS) -c test.c

pflush.o: pflush.c pflush.h
	$(CC) $(CFLAGS) -c pflush.c

log.o: log.c log.h
	$(CC) $(CFLAGS) -c log.c

clean:
	rm -rf *.o plevel
