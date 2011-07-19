#CFLAGS=-O0 -ggdb -std=c99
CFLAGS=-march=native -mtune=native -O2 -g0 -std=c99

.PHONY: clean all

all: shash

clean:
	rm -f *.o shash

shash: shash.o simi.o simiw.o lookup3.o

shash.o: shash.c simi.h simiw.h
simi.o: simi.c simi.h lookup3.h
simiw.o: simiw.c simiw.h lookup3.h
lookup3.o: lookup3.c lookup3.h
