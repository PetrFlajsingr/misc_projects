#pragma once
/**
 * Projekt 2, IPK
 * @name server
 * @author Petr Flajsingr, xflajs00
 **/
#include "ipk2.hh"

//prototypy funkci

void printHelp();

errorCode param(int, char**, std::string*);

errorCode serverMain(std::string);