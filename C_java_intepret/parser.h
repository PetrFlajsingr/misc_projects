/**
 * @file parser.h
 * @author Petr Flajsingr (xflajs00@stud.fit.vutbr.cz)
 * @date 9 october 2016
 * @brief Hlavickovy soubor pro syntaktickou a semantickou analyzi
**/

#ifndef PARSER_H
#define PARSER_H
#include "datatypes.h"
#include "lex.h"
//#include "code_gen.h"

int checkPKeyword(TokType, int*);

int checkPrint();

int parser();

int classCheck();

int classMember();

int blockBody();

int blockVar(tDatatype);

int functionCall(char*, char*);

int checkIf();

int checkWhile();

#endif
