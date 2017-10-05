# IPK projekt 2
# Makefile
# Author: Petr Flajsingr
# login: xflajs00

LOGIN = xflajs00
CC = g++
CFLAGS = -Wall -Wextra -pedantic -std=c++98
FILES = client.cpp client.hh server.cpp server.hh ipk2.cpp ipk2.hh Makefile test.sh testfolder testfolder2 xflajs00.pdf

CLIENT_FILES = client.cpp ipk2.cpp
SERVER_FILES = server.cpp ipk2.cpp

all: client server

#%.o : %.cpp
	#$(CC) $(CFLAGS) -c $< -o $@

client: $(CLIENT_FILES)
	$(CC) $(CFLAGS) $(CLIENT_FILES) -o client

server: $(SERVER_FILES)
	$(CC) $(CFLAGS) $(SERVER_FILES) -o server

clean:
	rm -f *.o
	rm -f server
	rm -f client
	rm -f xflajs00.tar

tar:
	tar -cf $(LOGIN).tar $(FILES) 
	gzip $(LOGIN).tar
