#!/bin/bash
#vytvoreni potrebnych spustitelnych souboru
make
g++ -Wall -Wextra -pedantic server.cpp ipk2.cpp -o testfolder/server
g++ -Wall -Wextra -pedantic client.cpp ipk2.cpp -o testfolder2/client

RETURN=0
COUNTER=0
echo "Zapnuti serveru na portu 1505."
printf "\n"
cd testfolder
./server -p 1505 &
RETURN=$?
PID=$!
cd ..
if [ "$RETURN" -ne 0 ]; then
	echo "Server skončil s chybou $RETURN"
	exit 0
fi
echo "Server je zapnut"
printf "\n"
echo "Test 01: help clienta:"

OUTPUT="$(./client --help)"
RETURN=$?
printf "\n"
if [ "$RETURN" -ne 0 ]; then
	echo "Chyba"
else
	echo "$OUTPUT"
	COUNTER=$[COUNTER + 1]
fi
printf "\n"
echo "Test 02: download souboru:"
./client -h 127.0.0.1 -p 1505 -d a.txt 
RETURN=$?
if [ "$RETURN" -ne 0 ]; then
	echo "Chyba klienta: $RETURN"
else
	DIFF=$(diff ./testfolder/a.txt a.txt)
	if [ "$DIFF" != "" ]; then
		echo "Chyba"
	else
		echo "OK"
		COUNTER=$[COUNTER + 1]
	fi
fi
rm -f ./testfolder2/a.txt
printf "\n"
echo "Test 03: upload souboru:"
cd testfolder2
./client -h 127.0.0.1 -p 1505 -u b.txt 
cd ..
RETURN=$?
if [ "$RETURN" -ne 0 ]; then
	echo "Chyba klienta: $RETURN"
else
	DIFF=$(diff ./testfolder/b.txt ./testfolder2/b.txt)
	if [ "$DIFF" != "" ]; then
		echo "Chyba"
	else
		echo "OK"
		COUNTER=$[COUNTER + 1]
	fi
fi

printf "\n"
echo "Test 04: download neexistujícího souboru:"

OUTPUT=$(./client -h 127.0.0.1 -p 1505 -u nope.txt)
RETURN=$?
if [ "$RETURN" -ne 0 ]; then
	echo "Chyba klienta: $RETURN"
else
	if [ "$OUTPUT" == "Soubor nenalezen..." ]; then
		echo "OK"
		COUNTER=$[COUNTER + 1]
	else
		echo "Chyba"
	fi
fi

echo "$COUNTER správně z možných 4"


rm -f ./testfolder/b.txt
disown
kill $PID
rm -f ./testfolder/server
rm -f ./testfolder2/client
printf "\n"
