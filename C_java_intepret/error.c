/** 
 * @file error.c
 * @author Dominik Dvorak(xdvora1t)
 * @date 23 November 2016
 * @brief Soubor obsahujici zdrojovy kod pro vypsani chybovych stavu
 **/

#include "error.h"

 /**
 * @brief Const char pole s chybovÃ½mi zprÃ¡vami
 **/
const char *error_messages[] = {
	"",
	"Lexical error",
	"Syntax error",
	"Semantic error",
	"Semantic error, type compatibility",
	"",
	"Semantic error",
	"Runtime error, wrong input",
	"Runtime initiliazing error",
	"Runtime error, division by zero",
	"Runtime error",
	"Internal error"
};


/**
 * @brief Funkce pro tisk chybove zpravy na stderr
 * @param error_number Typ zpravy
 * @param fmt Char * chybova zprava
 **/
void printError(int error_number){
	if(error_number == 99){
		error_number = 11;
	}
    fprintf(stderr, "%s\n", error_messages[error_number]);
    fflush(stdout);
}