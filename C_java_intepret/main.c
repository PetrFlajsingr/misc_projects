/**
 * @file main.c
 * @author Petr Flajsingr (xflajs00@stud.fit.vutbr.cz)
 * @date 12 december 2016
 * @brief Vstupni bod programu pro ifj16
**/

#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "param.h"
#include "lex.h"
#include "s_table.h"
#include "error.h"

Tok_T* token;
FILE* source;
tClassNodePtr SymbolTable;
char** arg;

int main(int argc, char** argv){
	arg = argv;
	int retVal;
	retVal = paramCheck(argc, argv);
	switch(retVal){
		case 0: break;
		case 1: return 0;
		default: return ERROR_INTER;
	}
	token = malloc(sizeof(Tok_T));
	retVal = parser();
	fclose(source);
	if(retVal){
		printError(retVal);
		free(token);
		return retVal;
	}
	init_interpret();
	retVal = interpret();
	if(retVal){
		printError(retVal);
	}
	free(token);
	fflush(stdout);
	return retVal;
}
