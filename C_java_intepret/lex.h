/**
 * @file lex.h
 * @author Dominik Dvorak(xdvora1t)
 * @date 27 September 2016
 * @brief Sobor obsahujici hlavickovy soubor lexikalniho analyzatoru
 **/

#ifndef LEX_H
#define LEX_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "error.h"

/**
 * @brief Enum stavu konecneho automatu
 **/
typedef enum E_TokType{
	START = 0,
	PLUS, 					// +
	PLUS_PLUS, 				// ++
	MINUS, 					// -
	MINUS_MINUS, 			// --
	MUL, 					// *
	DIV, 					// /
	SEMICOLON, 				// ;
	RIGHT_BRACKET, 			// )
	LEFT_BRACKET, 			// (
	RIGHT_SHARP_BRACKET, 	// >
	LEFT_SHARP_BRACKET, 	// <
	RIGHT_DOUBLE_BRACKET,	// }
	LEFT_DOUBLE_BRACKET,	// {
	COMMA,					// ,
	COLON,					// :
	ASSIGN,					// =
	NOT_EQUAL,				// !=
	EQUALS,					// ==
	GREATER,				// >
	LESS,					// <
	GREATER_EQUAL,			// >=
	LESS_EQUAL,				// <=
	NEG,					// !
	IDENTIFIER,				// ID
	IDENTIFIER_2_START,
	IDENTIFIER_2,			//IO.ID
	T_INT_BASE,				
	T_INT_OCTA,
	T_INT_BIN,
	T_INT_HEXA,
	T_INT_HEXA_FL,
	T_INT_HEXA_EXP,
	T_INT,					//cely cislo int a = !5! <- T_INT
	T_DOUBLE,				//desetinny cislo
	T_CHAR,					//normalni char 'a'
	T_CHAR_ESC,				//normalni sekvence v charu '\t'
	T_CHAR_ESC_OCTA,		//octa sekvence v charu '\100'
	T_STRING,				//"Mam rad syr"
	T_STRING_ESC,
	T_STRING_ESC_OCTA,
	T_DOUBLE_EXP_SIGN,
	T_DOUBLE_EXP,			//1e5
	T_DOUBLE_AFTER_DOT,
	COMMENT,
	LINE_COMMENT,
	BLOCK_COMMENT,
	END_BLOCK_COMMENT,
	KEYWORD,
	BOOL_TYPE,
	//KLICOVA SLOVA
	BOOLEAN,
	BREAK,
	CLASS,
	CONTINUE,
	DO,
	DOUBLE,
	ELSE,
	FALSE,
	FOR,
	IF,
	INT,
	RETURN,
	STRING,
	STATIC,
	TRUE,
	VOID,
	WHILE,
	READINT,
	READDOUBLE,
	READSTRING,
	PRINT,
	LENGTH,
	SUBSTR,
	COMPARE,
	FIND,
	SORT,
	TYPE_BOOL,
	EOF_T
} TokType;

/**
 * @brief Struktura pro ukladani tokenu
 **/
typedef struct{
	char syntax_character[2];
	char t_string[5000];
	char identifier_name[5000];
	double t_double;
	int t_int;
	TokType type;
}Tok_T;

//Deklarace funkci
int GetNewToken(FILE *, Tok_T *, bool);
int checkKeyword(char *);
void GoToStart(char [], int *, int *, bool *, int *);
void EndOfToken(bool *, char [], Tok_T *, int, int, FILE *, bool, int);
void NullTokken(Tok_T *);
bool checkEndOfNumber(int, FILE *);

#endif
