//
// Created by user on 8.12.2016.
//

#ifndef EXPR_TEST_GLOBALS_H
#define EXPR_TEST_GLOBALS_H

#include <stdio.h>
#include "lex.h"
#include "s_table.h"

// Globalni promenne
extern FILE* source; // Zdrojovy soubor
extern Tok_T* token; //Lexem
extern tClassNodePtr SymbolTable; //tabulka symbolu
extern tTape* Tape; //paska instrukci
char curClass[100]; //aktualni trida
char curFunc[100]; //aktualni funkce
extern char** arg;
bool innerBlock;
// \Globalni promenne

#endif //EXPR_TEST_GLOBALS_H