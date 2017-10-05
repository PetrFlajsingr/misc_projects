/**
 * @file s_table.h
 * @author Pavel Mica(xmicap02)
 * @date 9 october 2016
 * @brief Soubor obsahujici definici funkci pro praci s binarnim stromem
**/


#ifndef S_TABLE
#define S_TABLE

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "datatypes.h"
#include "stack.h"
#include "code_gen.h"
#include "funcpar.h"
/*

 datove typy 
typedef enum Datatype { 
    TYPE_INT,   //integer
    TYPE_DOUBLE,    //double
    TYPE_STRING //Å™etÄ›zec
}tDatatype;
*/

/* uchovani dat */
typedef union{                          
        int int_var;                    //integer
        double double_var;              //double
        char *string_var;               //Å™etÄ›zec
}tContentType;


/* informace o tride */
typedef struct{
        char name[1024];                      //nÃ¡zev tridy
}tSymbolClass;


/* informace o funkci */
typedef struct{
    char name[1024];        //nÃ¡zev funkce
    tDatatype ret_type;     //nÃ¡vratovÃ½ typ funkce
    tParamStack params;         //zÃ¡sobnÃ­k parametrÅ¯
    tTape code;
}tSymbolFunction;

/* informace o promenne */
typedef struct{
    char name[1024];        //nÃ¡zev promenne, konstanty 
    tDatatype type;         //datovÃ½ typ promenne, konstanty
    tContentType *content;  //union pro uchovÃ¡nÃ­ dat promÄ›nÃ©, konstanty

    bool activated;
    bool isSet;             //indikÃ¡tor inicializace promÄ›nnÃ©
    bool isConst;           //indikÃ¡tor konstatnÃ­ promÄ›nnÃ©
    char *longName;         //plnÄ› kvalifikovanÃ½ nÃ¡zev "class.variable", popÅ™ mÅ¯Å¾eme vyÅ™eÅ¡it i jinak
}tSymbolVariable;

/* uzel pro tridu */
typedef struct tClassNode{
    char key[1024];
        tSymbolClass tClass;                 
    struct tClassNode *LPtr;
    struct tClassNode *RPtr;

        struct tFunctionNode *function;
        struct tVariableNode *variable;
} *tClassNodePtr;

/* uzel pro funkci */
typedef struct tFunctionNode{
        char key[1024];                    
        tSymbolFunction tFunction; 
        struct tFunctionNode *LPtr;
        struct tFunctionNode *RPtr;

        struct tVariableNode *funcVariable;
} *tFunctionNodePtr;

/* uzel pro promennou */
typedef struct tVariableNode{
        char key[1024];                    
        tSymbolVariable tVariable; 
        struct tVariableNode *LPtr;
        struct tVariableNode *RPtr;
} *tVariableNodePtr;


//___________________________________________________________________________//

/* inicializace tabulek (BT) */
void initTable (tClassNodePtr *RootClassPtr);

/* vyhledavani symbolu v tabulkach */
tClassNodePtr    searchClass        (tClassNodePtr RootPtr, char *name);
tFunctionNodePtr searchFunction     (tClassNodePtr RootPtr, tFunctionNodePtr FuncPtr, char *name, char *className);
tVariableNodePtr searchVariable     (tClassNodePtr RootPtr, tVariableNodePtr VarPtr,  char *name, char *className);
tVariableNodePtr searchFuncVariable (tClassNodePtr RootPtr, tVariableNodePtr VarPtr,
                                     char *className, char *funcName, char *name);

/* vlozi uzel */
void insertClass        (tClassNodePtr *RootPtr, char *name);
void insertFunction     (tClassNodePtr *RootPtr, char *className, tSymbolFunction S);
void insertVariable     (tClassNodePtr *RootPtr, char *className, tSymbolVariable S);
void insertFuncVariable (tClassNodePtr *RootPtr, char *className, char *funcName, tSymbolVariable S);

/* zrusi uzel */
//void deleteClass        (tClassNodePtr    *RootPtr, char *name);
//void deleteFunction     (tFunctionNodePtr *RootPtr, char *name);
//void deleteVariable     (tVariableNodePtr *RootPtr, char *name);
//void deleteFuncVariable (tVariableNodePtr *RootPtr, char *name);

/* zahozeni tabulek (BT) */
//void disposeTable (tClassNodePtr *RootClassPtr);

#endif
