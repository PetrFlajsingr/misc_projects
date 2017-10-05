/**
 * @file code_gen.h
 * @project IFJ16
 * @author Igor Frank xfrank12
 * @date 11.12.2016
 * @brief Hlavickovy soubor obahujici prototypy funkci a struktur pro generator instrukci a interpret
 * */

#pragma once
#include "interfaces.h"
#include "funcpar.h"
#include "error.h"

#define TMP_TABLE_SIZE 20


typedef enum Operands{
	CG_PLUS = PLUS,
	CG_INC = PLUS_PLUS,
	CG_MINUS = MINUS,
	CG_DEC = MINUS_MINUS,
	CG_DIV = DIV,
	CG_MUL = MUL,
	CG_ASSIGN = ASSIGN,
	CG_GREATER = GREATER,
	CG_GREATER_EQUAL = GREATER_EQUAL,
	CG_LESS = LESS,
	CG_EQUALS = EQUALS,
	CG_NOT_EQUAL = NOT_EQUAL,
	CG_LESS_EQUAL = LESS_EQUAL,
	CG_PRINT,
	CG_SCAN_INT,
	CG_SCAN_DOUBLE,
	CG_SCAN_STRING,
	CG_CALL,
	CG_RET,
	CG_IF,
	CG_ELSE,
	CG_ENDIF,
	CG_LOOP_START,
	CG_LOOP,
	CG_ENDLOOP,
	CG_LENGTH,
	CG_SUBSTR,
	CG_COMPARE,
	CG_FIND,
	CG_SORT,
	CG_ACTIVATE,
}tOperands;


typedef struct{
    char syntax_character[2];
    char t_character[4];
    char t_string[1024];
    char identifier_name[1024];
    char identifier_class[1024];
    double t_double;
    int t_int;
    TokType tokType;
    tDatatype dtype; //information about data type of variables
    tOpResultId result; //some kind of id provided by the code generator
} CGTok_T;

//struktura reprezentujici jednu instrukci
typedef struct instruction{
	ETok_T src1;
	ETok_T src2;
	ETok_T result;
	tParamStack stack;			//params func
	tOperands instructionType;
} tInstruction;

//element instrukce obsahujici odkaz na dalsi instrukci ale i predchozi
typedef struct tapeElem{
	tInstruction instruction;
	struct tapeElem *next;
	struct tapeElem *previous;
}tTapeElem;

typedef tTapeElem *tTapeElemPtr;

//koren pasky
typedef struct tapeRoot{
	tTapeElemPtr first;
	tTapeElemPtr active;
	tTapeElemPtr last;
}tTape;

//struktura pro interni pamet docasnych vysledku
typedef struct tmpTableSym{
	tDatatype type;
	union data{
		int i_data;
		double d_data;
		char s_data[1024];
	}data;
}tTmpTableSym;

Errors init_tape(tTape* tape);								//inicializace pasky
Errors init_interpret();									//inicializace interpretu
void destroy_interpret();								//free interpret
//tOpResultId generate_to_function(tOperation op, char* class, char* function);
tOpResultId insert_call(char* class, char* function, tParamStack* params);	//vlozi volani fce na pasku
Errors set_tape(char* class, char* function);						//nastavi do ktere pasky se maji vkladat instrukce
Errors interpret();									//zacne s interpretaci programu
