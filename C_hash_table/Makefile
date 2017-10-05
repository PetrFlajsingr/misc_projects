CC=gcc
C+=g++
CFLAGS=-std=c99 -pedantic -Wall -Wextra
CCFLAGS=-std=c++11 -pedantic -Wall -Wextra

all: tail tail2 htable wordcount

tail: tail.c
	$(CC) $(CFLAGS) tail.c -o tail
tail2: tail2.cc
	$(C+) $(CCFLAGS) tail2.cc -o tail2
htable: htable.h htab_clear.c htab_foreach.c htab_free.c hash_function.c htab_init.c htab_lookup.c htab_remove.c htab_statistics.c
	$(CC) $(CFLAGS) -fPIC -c htab_init.c -o htab_init-shared.o
	$(CC) $(CFLAGS) -fPIC -c htab_clear.c -o htab_clear-shared.o
	$(CC) $(CFLAGS) -fPIC -c htab_foreach.c -o htab_foreach-shared.o
	$(CC) $(CFLAGS) -fPIC -c htab_free.c -o htab_free-shared.o
	$(CC) $(CFLAGS) -fPIC -c htab_lookup.c -o htab_lookup-shared.o
	$(CC) $(CFLAGS) -fPIC -c htab_remove.c -o htab_remove-shared.o
	$(CC) $(CFLAGS) -fPIC -c htab_statistics.c -o htab_statistics-shared.o
	$(CC) $(CFLAGS) -fPIC -c hash_function.c -o hash_function-shared.o
	$(CC) $(CFLAGS) -shared -o libhtable.so  htab_init-shared.o htab_clear-shared.o htab_foreach-shared.o htab_free-shared.o htab_lookup-shared.o htab_remove-shared.o htab_statistics-shared.o hash_function-shared.o

	$(CC) $(CFLAGS) -c htab_init.c -o htab_init-static.o
	$(CC) $(CFLAGS) -c htab_clear.c -o htab_clear-static.o
	$(CC) $(CFLAGS) -c htab_foreach.c -o htab_foreach-static.o
	$(CC) $(CFLAGS) -c htab_free.c -o htab_free-static.o
	$(CC) $(CFLAGS) -c htab_lookup.c -o htab_lookup-static.o
	$(CC) $(CFLAGS) -c htab_remove.c -o htab_remove-static.o
	$(CC) $(CFLAGS) -c htab_statistics.c -o htab_statistics-static.o
	$(CC) $(CFLAGS) -c hash_function.c -o hash_function-static.o
	ar crs libhtable.a  htab_init-static.o htab_clear-static.o htab_foreach-static.o htab_free-static.o htab_lookup-static.o htab_remove-static.o htab_statistics-static.o hash_function-static.o
	ranlib libhtable.a
wordcount: htable wordcount.c io.c
	$(CC) $(CFLAGS) wordcount.c libhtable.a -o wordcount
	$(CC) $(CFLAGS) wordcount.c -o wordcount-dynamic -L. -lhtable
clean:
	rm -f tail
	rm -f tail2
	rm -f wordcount
	rm -f wordcount-dynamic
	rm -f *.o
	rm -f *.a
	rm -f *.so
zip:
	zip xflajs00.zip *.c *.cc *.h Makefile
