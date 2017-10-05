#pragma once
/**
 * Projekt 2, IPK
 * @name client
 * @author Petr Flajsingr, xflajs00
 **/
#include "ipk2.hh"

/**
 * Trida obsahujici infomace o spojeni
 **/
class conInfo{
public:
	int port; //<cislo portu
	std::string host; //<jmeno serveru
	comType mode; //<typ komunikace -- download/upload
	std::string fileName; //<nazev vyzadovaneho souboru
};

//prototypy funkci

void printHelp();

errorCode param(int, char**, conInfo*);

errorCode myConnect(int*, conInfo*);

errorCode mySend(int sock, comType, std::string);

