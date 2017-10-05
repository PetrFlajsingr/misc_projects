//
// Created by user on 18.11.2016.
//

#ifndef ETOK_H  /* Include guard */
#define ETOK_H

#include "interfaces.h"
#include "datatypes.h"
#include "lex.h"

typedef int tOpResultId;//TODO choose the type of result_id

//modifying Tok_T so we can use it return data from code generator

typedef struct{
    char syntax_character[2];
    char t_character[4];
    char t_string[1024];
    char identifier_name[1024];
    double t_double;
    int t_int;
    TokType tokType;
    tDatatype dtype; //information about data type of variables
    tOpResultId result; //some kind of id provided by the code generator
} ETok_T;






#endif //    ETOK_H