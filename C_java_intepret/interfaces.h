#ifndef INTERFACES_H   /* Include guard */
#define INTERFACES_H

#include "etok.h"
#include "s_table.h"


//======= INTERFACES ============

//---- Interface between expression parser and code generator ----





typedef struct {
	ETok_T left;
	ETok_T right;
	ETok_T operator;
} tOperation;


tOpResultId generate_op(tOperation op);
//sending the required data to code generator
//getting back an ID so we can use it to identify the result later
//in case of errors during code generation, returns -1
//this is a function of the code generator



//---- Error codes ----

typedef enum {
	E_OK = 0,
	E_LEX = 1,
	E_SYN = 2,
	E_SEM_DEF = 3,
	E_SEM_TYPE = 4,
	E_SEM_OTHER = 6,
	E_RUN_INPUT = 7,
	E_RUN_INIT = 8,
	E_RUN_ZERO = 9,
	E_MEM = 99,
} ERROR;







#endif // INTERFACES_H