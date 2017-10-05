/** 
 * @file error.h
 * @author Dominik Dvorak(xdvora1t)
 * @date 23 November 2016
 * @brief Hlavickovy soubor error.c
 **/
#ifndef _ERROR_H
#define _ERROR_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/**
 * @brief Enum pro chybovÃ© stavy - viz zadÃ¡nÃ­
 **/
typedef enum T_Errors{
	OK_E = 0,
	LEX_ERROR,
	SYNTAX_ERROR,
	SEMANTIC_ERROR,
	SEMANTIC_TYPE_ERROR,
	PLACEHOLDER,
	SEMANTIC_OTHER_ERROR,
	ERROR_INPUT_NUMBER,
	ERROR_NOT_INITIALIZED,
	ERROR_ZERO_DIV,
	ERROR_OTHER_RUNTIME,
	ERROR_INTER = 99
}Errors;



void printError(int);

#endif