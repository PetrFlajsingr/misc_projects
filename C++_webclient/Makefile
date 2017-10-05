CC = g++
CFLAGS = -Wall -Wextra -pedantic -std=c++11

webclient: webclient.cpp
	$(CC) $(CFLAGS) webclient.cpp -o webclient

clean:
	rm -f webclient
	rm -f xflajs00.tar
tar:
	tar -cf xflajs00.tar Makefile webclient.cpp
	gzip xflajs00.tar

