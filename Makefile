CFLAGS = -std=c89 -pedantic

mappa: mappa.c taxicab.h Makefile
	gcc $(CFLAGS) mappa.c -o mappa 

random-set:  random-set.c taxicab.h Makefile
	gcc $(CFLAGS) random-set.c -o random-set

taxicab: master.c test-master.c Makefile
	gcc $(CFLAGS) master.c test-master.c -o taxicab

all: taxicab random-set mappa

clean:
	rm -f *.o taxicab random-set mappa *~

run: all
	./taxicab
