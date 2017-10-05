/**
 * @file code_gen.c
 * @project IFJ16
 * @author Igor Frank xfrank12 
 * @date 11.12.2016
 * @brief modul z funkcemi pro generovani kodu a interpretaci pasky
 * **/


#include <limits.h>
#include <float.h>
#include "s_table.h"
#include "code_gen.h"
#include "stack.h"
#include "lex.h"
#include "funcpar.h"
#include "ial.h"
#include "error.h"


/**
 *  GLOBAL VARIABLES 
 **/
extern tClassNodePtr SymbolTable;
tTape *Tape; //aktivni paska

tTmpTableSym *tmpTable;
typedef struct {
	int currIndex;
	int maxSize;
}tTmpInfo;
tTmpInfo tmpInfo; //tabulka docasnych promenych

struct cont{
	char class[1024];
	char function[1024];
}context;
tStack contextStack; //aktivni fuknkce a trida

struct{
	int depth;
	int maxSize;
	int size;
	struct sVar{
		char name[1024];
		int depthVar;
	}*var;
}ActTable; //tabulka aktivnich promenych

struct{
	int depth;
	int maxDepth;
	int maxSize;
	int size;
	tTapeElemPtr *actives;
	struct sTable{
		char class[512];
		char function[512];
		tSymbolVariable var;
		int depthTable;
	}*table;
}VarTable; //pomocna tabulka na rekurzi

/** 
 * end global 
 **/


/************************************************************************
 * funkce prochazi tabulku promenych ve fci a uklada obsah do interni tabulky - na rekurzi
**************************************************************************/
void treewalk(tVariableNodePtr varTree, char* class, char* function){
	if(varTree->LPtr != NULL){
		treewalk(varTree->LPtr, class, function);
	}
	else if (varTree->RPtr != NULL){
		treewalk(varTree->RPtr, class, function);
	}
	if(VarTable.size == VarTable.maxSize){
		VarTable.table = realloc(VarTable.table, sizeof(struct sTable) * (VarTable.maxSize + 10));
	}
	VarTable.table[++(VarTable.size)].var.content = malloc(sizeof(tContentType));
	if(varTree->tVariable.type == TYPE_STRING){
		VarTable.table[VarTable.size].var.content->string_var = malloc(1024);
		strcpy(VarTable.table[VarTable.size].var.content->string_var, varTree->tVariable.content->string_var);
	}
	else if(varTree->tVariable.type == TYPE_INT){
		VarTable.table[VarTable.size].var.content->int_var =  varTree->tVariable.content->int_var;
	}
	else if(varTree->tVariable.type == TYPE_DOUBLE){
		VarTable.table[VarTable.size].var.content->double_var =  varTree->tVariable.content->double_var;
	}	
	VarTable.table[VarTable.size].var.isConst = varTree->tVariable.isConst;
	strcpy(VarTable.table[VarTable.size].var.name, varTree->tVariable.name);
	VarTable.table[VarTable.size].var.isSet = varTree->tVariable.isSet;
	VarTable.table[VarTable.size].var.activated = varTree->tVariable.activated;
	VarTable.table[VarTable.size].var.type = varTree->tVariable.type;

	strcpy(VarTable.table[VarTable.size].class, class);
	strcpy(VarTable.table[VarTable.size].function, function);

	VarTable.table[VarTable.size].depthTable = VarTable.depth;
//	VarTable.size++;

	/*if(strlen(function) != 0){
		varTree->tVariable.isSet = false;
		varTree->tVariable.activated = false;
	}*/
}


/************************************************************************
 * funkce prochazi tabulku promenych ve tridach a uklada obsah do interni tabulky - na rekurzi
**************************************************************************/
void tablewalk(tClassNodePtr classTree){
	if(classTree->variable != NULL){
		treewalk(classTree->variable, classTree->key, "");
	}
	if(classTree->LPtr != NULL)
		tablewalk(classTree->LPtr);
	else if(classTree->RPtr != NULL)
		tablewalk(classTree->RPtr);

	
}
/*********************************************************************
 * zapisuje z interni tabulky do tabulky symbpolu - na rekurzi
 ********************************************************************/
void writeToTable(){
	tVariableNodePtr tmp;
	for(int i = 0; i <= VarTable.size; i++){
		if(VarTable.table[i].depthTable == VarTable.depth){
			if(strlen(VarTable.table[i].function) == 0)
				tmp = searchVariable(SymbolTable, NULL, VarTable.table[i].var.name, VarTable.table[i].class);
			else
				tmp = searchFuncVariable(SymbolTable, NULL, 
						VarTable.table[i].class, VarTable.table[i].function, VarTable.table[i].var.name);
			
			tmp->tVariable.type = VarTable.table[i].var.type;
			tmp->tVariable.isConst = VarTable.table[i].var.isConst;
			strcpy(tmp->tVariable.name, VarTable.table[i].var.name);
			tmp->tVariable.activated = VarTable.table[i].var.activated;
			tmp->tVariable.isSet = VarTable.table[i].var.isSet;

			if(VarTable.table[i].var.type == TYPE_INT)
				tmp->tVariable.content->int_var = VarTable.table[i].var.content->int_var;
			else if(VarTable.table[i].var.type == TYPE_DOUBLE)
				tmp->tVariable.content->double_var = VarTable.table[i].var.content->double_var;
			else if(VarTable.table[i].var.type == TYPE_STRING)
				strcpy(tmp->tVariable.content->string_var, VarTable.table[i].var.content->string_var);
			
		}
	}
}
/*inicializace pasky*/
Errors init_tape(tTape *tape){
	tape = malloc(sizeof(tTape));
	if(tape == NULL)
		return ERROR_INTER;
	tape->first = NULL;
	tape->last = NULL;
	tape->active = NULL;
	return OK_E;
}

/*inicializace interpretu*/
Errors init_interpret(){
	/*Tape.first = NULL;
	Tape.last = NULL;
	Tape.active = NULL;*/
	VarTable.maxSize = 50;
	VarTable.size = -1;
	VarTable.depth = 0;
	VarTable.maxDepth = 20;
	VarTable.table = malloc(sizeof(tSymbolVariable) * 100);
	VarTable.actives = malloc(sizeof(tTapeElemPtr)*VarTable.maxDepth);
	ActTable.maxSize = 50;
	ActTable.size = -1;
	ActTable.depth = 0;
	ActTable.var = malloc(sizeof(struct sVar)*50);
	if(ActTable.var == NULL)
		return ERROR_INTER;
	initStack(&contextStack, sizeof(context));
	tmpInfo.currIndex = 0;
	tmpInfo.maxSize = TMP_TABLE_SIZE;
	if((tmpTable = malloc(sizeof(tTmpTableSym) * TMP_TABLE_SIZE)) == NULL)
		return ERROR_INTER;
	else
		return OK_E;

}

/*Uvolneni pasky*/
void destroy_tape(tTape* tape){
	tTapeElemPtr tmp;
	while(tape->first != NULL){
		tmp = tape->first;
		tape->first = tape->first->next;
		destroyParams(&tmp->instruction.stack);
		free(tmp);
	}
	tape->last = NULL;
	tape->first = NULL;
	tape->active = NULL;
}
/*Uvolneni interpretu*/
void destroy_interpret(){
	free(tmpTable);
	free(ActTable.var);
	freeStack(&contextStack);
	tmpInfo.maxSize = 0;
	tmpInfo.currIndex = 0;
}
/*Prepnuti pasky*/
Errors set_tape(char* class, char* function){
	tFunctionNodePtr tmp = searchFunction(SymbolTable, NULL, function, class);
	if(tmp == NULL)
		return SEMANTIC_ERROR;
	Tape = &tmp->tFunction.code;
	memset(context.class, 0, 1024);
	strcpy(context.class, class);
	memset(context.function, 0, 1024);
	strcpy(context.function, function);
	return OK_E;
}

/*************************************************************
 *Funkce vklada na aktivni pasku volani funkce v kodu
 *************************************************************/
tOpResultId insert_call(char* class, char* function, tParamStack* params){
	tmpInfo.currIndex = 0;
	tTapeElem *tmp = malloc(sizeof(tTapeElem));
	if(tmp == NULL)
		return -ERROR_INTER;

	//volani interni funkce
	if(strcmp(class, "ifj16") == 0)	{
		tmp->instruction.stack.params = malloc(sizeof(tParam) * params->maxSize);
		if(tmp->instruction.stack.params == NULL)
			return -ERROR_INTER;
		
		tmp->instruction.stack.params = params->params;
		tmp->instruction.stack.size = params->size;
		tmp->instruction.stack.maxSize = params->maxSize;
		if(strcmp(function, "readInt") == 0){
			tmp->instruction.instructionType = CG_SCAN_INT;
		}
		else if(strcmp(function, "readDouble") == 0){
			tmp->instruction.instructionType = CG_SCAN_DOUBLE;
		}
		else if(strcmp(function, "readString") == 0){
			tmp->instruction.instructionType = CG_SCAN_STRING;
		}
		else if(strcmp(function, "print") == 0){
			tmp->instruction.instructionType = CG_PRINT;
		}
		else if(strcmp(function, "length") == 0){
			tmp->instruction.instructionType = CG_LENGTH;
		}
		else if(strcmp(function, "substr") == 0){
			tmp->instruction.instructionType = CG_SUBSTR;
		}
		else if(strcmp(function, "compare") == 0){
			tmp->instruction.instructionType = CG_COMPARE;
		}
		else if(strcmp(function, "find") == 0){
			tmp->instruction.instructionType = CG_FIND;
		}
		else if(strcmp(function, "sort") == 0){
			tmp->instruction.instructionType = CG_SORT;
		}
		else{
			return -ERROR_INTER;
		}
		
	}
	else{
		if(strlen(class) !=0)
			strcpy(tmp->instruction.src1.identifier_name, class);
		else
			strcpy(tmp->instruction.src1.identifier_name, context.class);

		strcpy(tmp->instruction.src2.identifier_name, function);
		

		tmp->instruction.instructionType = CG_CALL;
		
		tmp->instruction.stack = *params;
	}
	tmp->instruction.result.dtype = 3; 
	tmp->instruction.result.result = tmpInfo.currIndex;

	tmp->instruction.src1.result = -1;
	tmp->instruction.src2.result = -1;

	tmp->next = NULL;
	tmp->previous = NULL;
	if(Tape->first == NULL){
		Tape->last = tmp;
		Tape->first = tmp;
		Tape->active = tmp;
	}
	else{
		tmp->previous = Tape->last;
		Tape->last->next = tmp;
		Tape->last = tmp;
	}
	return tmpInfo.currIndex;
}

/******************************************************
 * funkce generujici instrukce na aktivni pasku
 ******************************************************/
tOpResultId generate_op(tOperation op){

	tTapeElem *tmp = (tTapeElem*)malloc(sizeof(tTapeElem));
	if(tmp == NULL)
		return -ERROR_INTER;

	tmp->instruction.src1 = op.left;
	tmp->instruction.src2 = op.right;

	tmp->instruction.instructionType = (tOperands)op.operator.tokType;
	tmp->instruction.result.dtype = 3;  
	tmp->instruction.result.t_int = ++(tmpInfo.currIndex);

	if(tmpInfo.currIndex == tmpInfo.maxSize){
		tmpTable = realloc(tmpTable,sizeof(tTmpTableSym)*(tmpInfo.maxSize + TMP_TABLE_SIZE));
		if(tmpTable == NULL)
			return -ERROR_INTER;
	}

	if(op.operator.tokType == (TokType)CG_ASSIGN || op.operator.tokType == (TokType)CG_RET)
		tmpInfo.currIndex = 0;
	if(op.operator.tokType == (TokType)CG_RET)
		tmp->instruction.result.t_int = tmpInfo.currIndex;

		tmp->next = NULL;
		if(Tape->first == NULL){
			tmp->previous = NULL;
			Tape->first = tmp;
			Tape->active = tmp;
		}
		else{
			tmp->previous = Tape->last;
			Tape->last->next = tmp;
		}
			
		Tape->last = tmp;

	return tmpInfo.currIndex;	

}

/**********************************************************
 * funkce pripravi operand nacte hodnoty z tabuly
 * *********************************************************/
Errors prepare_operand(ETok_T *src, char* class, bool *init){

	if(/*src->dtype == (tDatatype)BOOL_TYPE &&*/ src->result > -1){
		*init = true;
		switch(tmpTable[src->result].type){
			case TYPE_INT:
				src->t_int = tmpTable[src->result].data.i_data;
				break;
			case TYPE_DOUBLE:
				src->t_double = tmpTable[src->result].data.d_data;
				break;
			case TYPE_STRING:
				strcpy(src->t_string, tmpTable[src->result].data.s_data);
				break;
			case TYPE_VOID:
				return ERROR_NOT_INITIALIZED;
				break;
			default:
				return ERROR_INTER;
				break;
		}
		src->dtype = tmpTable[src->result].type;
		strcpy(class, context.class);
	}
	else if(src->tokType == IDENTIFIER){
		tVariableNodePtr tmp;
		if(strlen(src->t_string) == 0){
			tmp = searchFuncVariable(SymbolTable, NULL, context.class, context.function, src->identifier_name);
			if(tmp == NULL){
				tmp = searchVariable(SymbolTable, NULL, src->identifier_name, context.class);
				if(tmp == NULL)
					return SEMANTIC_ERROR;
			}
			else if(!tmp->tVariable.activated)
				return SEMANTIC_ERROR;

			memset(class, 0, 1024);
			strcpy(class, context.class);
		}
		else{
			tmp = searchVariable(SymbolTable, NULL, src->identifier_name, src->t_string);
			if(tmp == NULL)
				return SEMANTIC_ERROR;
			memset(class, 0, 1024);
			strcpy(class, src->t_string);
		}
		
		switch(tmp->tVariable.type){
			case TYPE_INT:
				src->t_int = tmp->tVariable.content->int_var;
				break;
			case TYPE_DOUBLE:
				src->t_double = tmp->tVariable.content->double_var;
				break;
			case TYPE_STRING:
				if(strlen(tmp->tVariable.content->string_var) != 0)
					strcpy(src->t_string, tmp->tVariable.content->string_var);
				break;
			default:
				return ERROR_INTER;
				break;
		}
		*init = tmp->tVariable.isSet;
		src->dtype = tmp->tVariable.type;
	}
	else{
		*init = true;
	}
	return OK_E;
}

/**********************************************************
 * funkce pripravi zasobnik nacte hodnoty z tabuly
 * *********************************************************/
Errors prepare_stack(tParamStack *stack){
	for(int i = 0; i < stack->size; ++i){
		if(strlen(stack->params[i].name) != 0){
			tVariableNodePtr tmp;
			if(strlen(stack->params[i].data.tString) == 0){
				tmp = searchFuncVariable(SymbolTable, NULL, context.class, context.function, stack->params[i].name);
				if(tmp == NULL){
					tmp = searchVariable(SymbolTable, NULL, stack->params[i].name, context.class);
					if(tmp == NULL)
						return SEMANTIC_ERROR;
				}
				else if(!tmp->tVariable.activated)
					return SEMANTIC_ERROR;
				strcpy(stack->params[i].class, context.class);
			}
			else if(strlen(stack->params[i].class) != 0){ // jiz pripraven
				tmp = searchVariable(SymbolTable, NULL, stack->params[i].name, stack->params[i].class);
				if(tmp == NULL){
					tmp = searchFuncVariable(SymbolTable, NULL, stack->params[i].class, context.function, stack->params[i].name);
				}
			}
			else{
				tmp = searchVariable(SymbolTable, NULL, stack->params[i].name, stack->params[i].data.tString);
				if(tmp == NULL)
					return SEMANTIC_ERROR;
				strcpy(stack->params[i].class, stack->params[i].data.tString);
			}
			switch(tmp->tVariable.type){
				case TYPE_INT:
					stack->params[i].data.tInt = tmp->tVariable.content->int_var;
					break;
				case TYPE_DOUBLE:
					stack->params[i].data.tDouble = tmp->tVariable.content->double_var;
					break;
				case TYPE_STRING:
					strcpy(stack->params[i].data.tString, tmp->tVariable.content->string_var);
					break;
				default:
					return ERROR_INTER;
					break;
			}
			stack->params[i].init = tmp->tVariable.isSet;
			stack->params[i].dType = tmp->tVariable.type;
		}
		else
			stack->params[i].init = true;
	}
	return OK_E;
}
/****************************************************************
 * jadro modulu - interpretuje aktivni pasku
 * **************************************************************/
Errors interpret(){
	Errors State;
	State = set_tape("ifj16", "init");
	if(State != OK_E)
		return State;
	tTapeElemPtr currInstruction = Tape->active;
	bool init1, init2;
	char *class1, *class2;
	ETok_T src1, src2, res;
	tVariableNodePtr tmp;
	tParamStack tmpStack ;
	int depth;
	
	class1 = malloc(1024);
	if(class1 == NULL){
		return ERROR_INTER;
	}

	class2 = malloc(1024);
	if(class2 == NULL){
		free(class1);
		return ERROR_INTER;
	}
	tmpStack.params = NULL;	
//	if(currInstruction != NULL){
//	}
	while(currInstruction != NULL){
		src1 = currInstruction->instruction.src1;
		src2 = currInstruction->instruction.src2;
		res = currInstruction->instruction.result;
		//tmpStack.params = malloc(currInstruction->instruction.stack.maxSize * sizeof(tParam));
	/*	if(tmpStack.params == NULL){
			free(class1);
			free(class2);
			return ERROR_INTER;
		}
	*/	//memcpy(&tmpStack.params, &currInstruction->instruction.stack.params, sizeof(tParam) * currInstruction->instruction.stack.maxSize);
		tmpStack.maxSize = currInstruction->instruction.stack.maxSize;
		tmpStack.size = currInstruction->instruction.stack.size;
		tmpStack.params = currInstruction->instruction.stack.params;
		//TODO table a result do lokalniho src
		State = prepare_operand(&src1, class1, &init1);
		if(State != OK_E){
			free(tmpStack.params);
			free(class1);
			free(class2);
			return State;
		}
		State = prepare_operand(&src2, class2, &init2);
		if(State != OK_E){
			free(tmpStack.params);
			free(class1);
			free(class2);
			return State;
		}
		if(currInstruction->instruction.stack.params != NULL){
			State = prepare_stack(&tmpStack);
			if(State != OK_E){
				free(tmpStack.params);
				free(class1);
				free(class2);
				return State;
			}
		}


		switch((tOperands)currInstruction->instruction.instructionType){
			case CG_INC:
			case CG_DEC:
				if(!init1){
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_NOT_INITIALIZED;
				}
				tmp = searchFuncVariable(SymbolTable, NULL, class1, context.function, src1.identifier_name);
				if(tmp == NULL){
					tmp = searchVariable(SymbolTable, NULL, src1.identifier_name, class1);
					if(tmp == NULL){
						free(tmpStack.params); free(class1); free(class2);
						return SEMANTIC_ERROR;
					}
					if(tmp->tVariable.type == TYPE_INT){
						if(currInstruction->instruction.instructionType == CG_INC)
							tmp->tVariable.content->int_var++;
						else
							tmp->tVariable.content->int_var--;
					}
					else if(tmp->tVariable.type == TYPE_DOUBLE){
						if(currInstruction->instruction.instructionType == CG_INC)
							tmp->tVariable.content->double_var++;
						else
							tmp->tVariable.content->double_var--;
					}
					else if(tmp->tVariable.type == TYPE_STRING){
						free(tmpStack.params); free(class1); free(class2);
						return SEMANTIC_TYPE_ERROR;
					}
					else{
						free(tmpStack.params); free(class1); free(class2);
						return ERROR_INTER;
					}
					//insertVariable(&SymbolTable, class1, tmp->tVariable);
				}
				else if(!tmp->tVariable.activated)
					return SEMANTIC_ERROR;
				else{
					if(tmp->tVariable.type == TYPE_INT)
						tmp->tVariable.content->int_var++;
					else if(tmp->tVariable.type == TYPE_DOUBLE)
						tmp->tVariable.content->double_var++;
					else if(tmp->tVariable.type == TYPE_STRING){
						free(tmpStack.params); free(class1); free(class2);
						return SEMANTIC_TYPE_ERROR;
					}
					else{
						free(tmpStack.params); free(class1); free(class2);
						return ERROR_INTER;
					}
					//insertFuncVariable(&SymbolTable, class1, context.function, tmp->tVariable);
				}
				break;
			case CG_PLUS:
			case CG_MINUS:
			case CG_DIV:
			case CG_MUL:
				if(!init1 || !init2){
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_NOT_INITIALIZED;
				}
				if(src1.dtype == TYPE_INT && src2.dtype == TYPE_INT){
					if(currInstruction->instruction.instructionType == CG_PLUS)
						tmpTable[res.t_int].data.i_data = src1.t_int + src2.t_int;
					else if (currInstruction->instruction.instructionType == CG_MINUS)
						tmpTable[res.t_int].data.i_data = src1.t_int - src2.t_int;
					else if(currInstruction->instruction.instructionType == CG_MUL)
						tmpTable[res.t_int].data.i_data = src1.t_int * src2.t_int;
					else{
						if(src2.t_int == 0){
							free(tmpStack.params); free(class1); free(class2);
							return ERROR_ZERO_DIV;
						}
						tmpTable[res.t_int].data.i_data = src1.t_int / src2.t_int;
					}
					tmpTable[res.t_int].type = TYPE_INT;
				}
				else if(src1.dtype == TYPE_INT && src2.dtype == TYPE_DOUBLE){
					if(currInstruction->instruction.instructionType == CG_PLUS)
						tmpTable[res.t_int].data.d_data = src1.t_int + src2.t_double;
					else if (currInstruction->instruction.instructionType == CG_MINUS)
						tmpTable[res.t_int].data.d_data = src1.t_int - src2.t_double;
					else if(currInstruction->instruction.instructionType == CG_MUL)
						tmpTable[res.t_int].data.d_data = src1.t_int * src2.t_double;
					else{
						if(src2.t_double == 0.0){
							free(tmpStack.params); free(class1); free(class2);
							return ERROR_ZERO_DIV;
						}
						tmpTable[res.t_int].data.d_data = src1.t_int / src2.t_double;
					}
					tmpTable[res.t_int].type = TYPE_DOUBLE;
				}
				else if(src1.dtype == TYPE_DOUBLE && src2.dtype == TYPE_INT){
					if(currInstruction->instruction.instructionType == CG_PLUS)
						tmpTable[res.t_int].data.d_data = src1.t_double + src2.t_int;
					else if (currInstruction->instruction.instructionType == CG_MINUS)
						tmpTable[res.t_int].data.d_data = src1.t_double - src2.t_int;
					else if(currInstruction->instruction.instructionType == CG_MUL)
						tmpTable[res.t_int].data.d_data = src1.t_double * src2.t_int;
					else{
						if(src2.t_int == 0){
							free(tmpStack.params); free(class1); free(class2);
							return ERROR_ZERO_DIV;
						}
						tmpTable[res.t_int].data.d_data = src1.t_double / src2.t_int;
					}
					tmpTable[res.t_int].type = TYPE_DOUBLE;
				}
				else if(src1.dtype == TYPE_DOUBLE && src2.dtype == TYPE_DOUBLE){
					if(currInstruction->instruction.instructionType == CG_PLUS)
						tmpTable[res.t_int].data.d_data = src1.t_double + src2.t_double;
					else if (currInstruction->instruction.instructionType == CG_MINUS)
						tmpTable[res.t_int].data.d_data = src1.t_double - src2.t_double;
					else if(currInstruction->instruction.instructionType == CG_MUL)
						tmpTable[res.t_int].data.d_data = src1.t_double * src2.t_double;
					else{
						if(src2.t_double == 0.0){
							free(tmpStack.params); free(class1); free(class2);
							return ERROR_ZERO_DIV;
						}
						tmpTable[res.t_int].data.d_data = src1.t_double / src2.t_double;
					}
					tmpTable[res.t_int].type = TYPE_DOUBLE;
				}
				else if(src1.dtype == TYPE_STRING && src2.dtype == TYPE_STRING){
					if(currInstruction->instruction.instructionType == CG_PLUS){
						strcpy(tmpTable[res.t_int].data.s_data, strcat(src1.t_string, src2.t_string));
						tmpTable[res.t_int].type = TYPE_STRING;
					}
					else{
						free(tmpStack.params); free(class1); free(class2);
						return SEMANTIC_TYPE_ERROR;
					}
				}
				else if((src1.dtype == TYPE_STRING && src2.dtype == TYPE_DOUBLE) || 
						(src1.dtype == TYPE_STRING && src2.dtype == TYPE_INT) ||
						(src1.dtype == TYPE_INT && src2.dtype == TYPE_STRING) || 
						(src1.dtype == TYPE_DOUBLE && src2.dtype == TYPE_STRING)){
					free(tmpStack.params); free(class1); free(class2);
					return SEMANTIC_ERROR;
				}
				else{
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_INTER;
				}
				break;
			case CG_EQUALS:
			case CG_NOT_EQUAL:
			case CG_GREATER:
			case CG_GREATER_EQUAL:
			case CG_LESS:
			case CG_LESS_EQUAL:
				if(!init1 || !init2){
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_NOT_INITIALIZED;
				}
				if(src1.dtype == TYPE_INT && src2.dtype == TYPE_INT){
					if(currInstruction->instruction.instructionType == CG_EQUALS){
						if(src1.t_int == src2.t_int)
							tmpTable[res.t_int].data.i_data = true;
						else
							tmpTable[res.t_int].data.i_data = false;
					}
					else if(currInstruction->instruction.instructionType == CG_NOT_EQUAL){
						if(src1.t_int != src2.t_int)
							tmpTable[res.t_int].data.i_data = true;
						else
							tmpTable[res.t_int].data.i_data = false;
					}
					else if(currInstruction->instruction.instructionType == CG_GREATER){
						if(src1.t_int > src2.t_int)
							tmpTable[res.t_int].data.i_data = true;
						else
							tmpTable[res.t_int].data.i_data = false;
					}
					else if(currInstruction->instruction.instructionType == CG_GREATER_EQUAL){
						if(src1.t_int >= src2.t_int)
							tmpTable[res.t_int].data.i_data = true;
						else
							tmpTable[res.t_int].data.i_data = false;
					}
					else if(currInstruction->instruction.instructionType == CG_LESS){
						if(src1.t_int < src2.t_int)
							tmpTable[res.t_int].data.i_data = true;
						else
							tmpTable[res.t_int].data.i_data = false;
					}
					else{
						if(src1.t_int >= src2.t_int)
							tmpTable[res.t_int].data.i_data = true;
						else
							tmpTable[res.t_int].data.i_data = false;
					}
					tmpTable[res.t_int].type = TYPE_INT;

				}
				else if(src1.dtype == TYPE_DOUBLE && src2.dtype == TYPE_INT){
					if(currInstruction->instruction.instructionType == CG_EQUALS){
						if(src1.t_double == src2.t_int)
							tmpTable[res.t_int].data.i_data = true;
						else
							tmpTable[res.t_int].data.i_data = false;
					}
					else if(currInstruction->instruction.instructionType == CG_NOT_EQUAL){
						if(src1.t_double != src2.t_int)
							tmpTable[res.t_int].data.i_data = true;
						else
							tmpTable[res.t_int].data.i_data = false;
					}
					else if(currInstruction->instruction.instructionType == CG_GREATER){
						if(src1.t_double > src2.t_int)
							tmpTable[res.t_int].data.i_data = true;
						else
							tmpTable[res.t_int].data.i_data = false;
					}
					else if(currInstruction->instruction.instructionType == CG_GREATER_EQUAL){
						if(src1.t_double >= src2.t_int)
							tmpTable[res.t_int].data.i_data = true;
						else
							tmpTable[res.t_int].data.i_data = false;
					}
					else if(currInstruction->instruction.instructionType == CG_LESS){
						if(src1.t_double < src2.t_int)
							tmpTable[res.t_int].data.i_data = true;
						else
							tmpTable[res.t_int].data.i_data = false;
					}
					else{
						if(src1.t_double >= src2.t_int)
							tmpTable[res.t_int].data.i_data = true;
						else
							tmpTable[res.t_int].data.i_data = false;
					}
				}
				else if(src1.dtype == TYPE_INT && src2.dtype == TYPE_DOUBLE){
					if(currInstruction->instruction.instructionType == CG_EQUALS){
						if(src1.t_int == src2.t_double)
							tmpTable[res.t_int].data.i_data = true;
						else
							tmpTable[res.t_int].data.i_data = false;
					}
					else if(currInstruction->instruction.instructionType == CG_NOT_EQUAL){
						if(src1.t_int != src2.t_double)
							tmpTable[res.t_int].data.i_data = true;
						else
							tmpTable[res.t_int].data.i_data = false;
					}
					else if(currInstruction->instruction.instructionType == CG_GREATER){
						if(src1.t_int > src2.t_double)
							tmpTable[res.t_int].data.i_data = true;
						else
							tmpTable[res.t_int].data.i_data = false;
					}
					else if(currInstruction->instruction.instructionType == CG_GREATER_EQUAL){
						if(src1.t_int >= src2.t_double)
							tmpTable[res.t_int].data.i_data = true;
						else
							tmpTable[res.t_int].data.i_data = false;
					}
					else if(currInstruction->instruction.instructionType == CG_LESS){
						if(src1.t_int < src2.t_double)
							tmpTable[res.t_int].data.i_data = true;
						else
							tmpTable[res.t_int].data.i_data = false;
					}
					else{
						if(src1.t_int >= src2.t_double)
							tmpTable[res.t_int].data.i_data = true;
						else
							tmpTable[res.t_int].data.i_data = false;
					}
				}
				else if(src1.dtype == TYPE_DOUBLE && src2.dtype == TYPE_DOUBLE){
					if(currInstruction->instruction.instructionType == CG_EQUALS){
						if(src1.t_double == src2.t_double)
							tmpTable[res.t_int].data.i_data = true;
						else
							tmpTable[res.t_int].data.i_data = false;
					}
					else if(currInstruction->instruction.instructionType == CG_NOT_EQUAL){
						if(src1.t_double != src2.t_double)
							tmpTable[res.t_int].data.i_data = true;
						else
							tmpTable[res.t_int].data.i_data = false;
					}
					else if(currInstruction->instruction.instructionType == CG_GREATER){
						if(src1.t_double > src2.t_double)
							tmpTable[res.t_int].data.i_data = true;
						else
							tmpTable[res.t_int].data.i_data = false;
					}
					else if(currInstruction->instruction.instructionType == CG_GREATER_EQUAL){
						if(src1.t_double >= src2.t_double)
							tmpTable[res.t_int].data.i_data = true;
						else
							tmpTable[res.t_int].data.i_data = false;
					}
					else if(currInstruction->instruction.instructionType == CG_LESS){
						if(src1.t_double < src2.t_double)
							tmpTable[res.t_int].data.i_data = true;
						else
							tmpTable[res.t_int].data.i_data = false;
					}
					else{
						if(src1.t_double >= src2.t_double)
							tmpTable[res.t_int].data.i_data = true;
						else
							tmpTable[res.t_int].data.i_data = false;
					}
				}
				else {
					free(tmpStack.params); free(class1); free(class2);
					return SEMANTIC_TYPE_ERROR;
				}
				tmpTable[res.t_int].type = TYPE_INT;
				break;
			case CG_ASSIGN: //src1 kam src2 co
				if(!init2){
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_NOT_INITIALIZED;
				}
				if(src1.dtype == TYPE_INT){
					if(src2.dtype != TYPE_INT){
						free(tmpStack.params); free(class1); free(class2);
						return SEMANTIC_TYPE_ERROR;
					}
					tmp = searchFuncVariable(SymbolTable, NULL, class1, context.function, src1.identifier_name);
					if(tmp == NULL){
						tmp = searchVariable(SymbolTable, NULL, src1.identifier_name, class1);
						if(tmp == NULL){
							free(tmpStack.params); free(class1); free(class2);
							return ERROR_INTER;
						}
						tmp->tVariable.content->int_var = src2.t_int;
						tmp->tVariable.isSet = true;
						//insertVariable(&SymbolTable, class1, tmp->tVariable);
					}
					else if(!tmp->tVariable.activated)
						return SEMANTIC_ERROR;
					else{
					       	tmp->tVariable.content->int_var = src2.t_int;
						tmp->tVariable.isSet = true;
						//insertFuncVariable(&SymbolTable, class1, context.function, tmp->tVariable);
					}
				}
				else if (src1.dtype == TYPE_DOUBLE){
					if(src2.dtype == TYPE_STRING || src2.dtype == TYPE_VOID){
						free(tmpStack.params); free(class1); free(class2);
						return SEMANTIC_TYPE_ERROR;
					}
					tmp = searchFuncVariable(SymbolTable, NULL, class1, context.function, src1.identifier_name);
					if(tmp == NULL){
						tmp = searchVariable(SymbolTable, NULL, src1.identifier_name, class1);
						if(tmp == NULL){
							free(tmpStack.params); free(class1); free(class2);
							return ERROR_INTER;
						}
						if(src2.dtype == TYPE_DOUBLE)
							tmp->tVariable.content->double_var = src2.t_double;
						else if(src2.dtype == TYPE_INT)
							tmp->tVariable.content->double_var = src2.t_int;
						else{//TODO inyternal err
						}

						tmp->tVariable.isSet = true;
						//insertVariable(&SymbolTable, class1, tmp->tVariable);
					}
					else if(!tmp->tVariable.activated)
						return SEMANTIC_ERROR;
					else{
						if(src2.dtype == TYPE_DOUBLE)
							tmp->tVariable.content->double_var = src2.t_double;
						else if(src2.dtype == TYPE_INT)
							tmp->tVariable.content->double_var = src2.t_int;
						else{//TODO inyternal err
						}
						tmp->tVariable.isSet = true;
						//insertFuncVariable(&SymbolTable, class1,context.function, tmp->tVariable);
					}
				}
				else if(src1.dtype == TYPE_STRING){
					if(src2.dtype == TYPE_INT || src2.dtype == TYPE_DOUBLE || src2.dtype == TYPE_VOID){
							free(tmpStack.params); free(class1); free(class2);
							return SEMANTIC_TYPE_ERROR;
					}
					tmp = searchFuncVariable(SymbolTable, NULL, class1, context.function, src1.identifier_name);
					if(tmp == NULL){
						tmp = searchVariable(SymbolTable, NULL, src1.identifier_name, class1);
						if(tmp == NULL){
							free(tmpStack.params); free(class1); free(class2);
							return ERROR_INTER;
						}
						strcpy(tmp->tVariable.content->string_var, src2.t_string);
						tmp->tVariable.isSet = true;
						//insertVariable(&SymbolTable, class1, tmp->tVariable);
					}
					else if(!tmp->tVariable.activated)
						return SEMANTIC_ERROR;
					else{
						strcpy(tmp->tVariable.content->string_var, src2.t_string);
						tmp->tVariable.isSet = true;
						//insertFuncVariable(&SymbolTable, class1, context.function ,tmp->tVariable);
					}
				}
				break;
			case CG_IF:
				if(!init1){
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_NOT_INITIALIZED;
				}
				if(src1.dtype == TYPE_STRING){
					free(tmpStack.params); free(class1); free(class2);
					return SEMANTIC_TYPE_ERROR;
				}
				if(src1.t_int == 0){
					depth = 1;
					while(depth != 0){
						Tape->active = Tape->active->next;
						currInstruction = Tape->active;
						if(currInstruction->instruction.instructionType == CG_IF)
							depth++;
						else if(currInstruction->instruction.instructionType == CG_ELSE)
							depth--;
					}
				}
				else if(src1.t_int > 1){
					free(tmpStack.params); free(class1); free(class2);
					return SEMANTIC_ERROR;	
				}				
				break;
			case CG_ELSE:
				;
				depth = 1;
				while(depth != 0){
					Tape->active = Tape->active->next;
					currInstruction = Tape->active;
					if(currInstruction->instruction.instructionType == CG_IF)
						depth++;
					else if(currInstruction->instruction.instructionType == CG_ENDIF)
						depth--;
				}
				break;
			case CG_ENDIF:
				break;
			case CG_LOOP_START:
				break;
			case CG_LOOP:
				if(!init1){
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_NOT_INITIALIZED;
				}
				if(src1.dtype == TYPE_STRING){
					free(tmpStack.params); free(class1); free(class2);
					return SEMANTIC_TYPE_ERROR;
				}
				if(src1.t_int == 0){
					depth = 1;
					while(depth != 0){
						Tape->active = Tape->active->next;
						currInstruction = Tape->active;
						if(currInstruction->instruction.instructionType == CG_LOOP)
							depth++;
						else if(currInstruction->instruction.instructionType == CG_ENDLOOP)
							depth--;
					}
				}
				else if(src1.t_int > 1){
					free(tmpStack.params); free(class1); free(class2);
					return SEMANTIC_ERROR;
				}
				break;
			case CG_ENDLOOP:
				;
				depth = 1;
				while(depth != 0){
					Tape->active = Tape->active->previous;
					currInstruction = Tape->active;
					if(currInstruction->instruction.instructionType == CG_ENDLOOP)
						depth++;
					else if(currInstruction->instruction.instructionType == CG_LOOP_START)
						depth--;
				}
				break;					
			case CG_PRINT:				
				if(!tmpStack.params[0].init){
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_NOT_INITIALIZED;
				}
				if(tmpStack.params[0].dType == TYPE_INT){
					printf("%d", tmpStack.params[0].data.tInt);
					fflush(stdout);
				}
				else if(tmpStack.params[0].dType == TYPE_DOUBLE){
					printf("%g", tmpStack.params[0].data.tDouble);
					fflush(stdout);
				}
				else if(tmpStack.params[0].dType == TYPE_STRING){
					printf("%s", tmpStack.params[0].data.tString);
					fflush(stdout);
				}
				else{		
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_INTER;
				}
				break;
			case CG_SCAN_INT:;
				long long i;
				char c[1024], garbage, *str;
				if(fgets(c, 1023, stdin) == NULL){
					//TODO no line err
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_INTER;
				}
				if(strcmp(c, "\n") == 0){
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_OTHER_RUNTIME;
				}
				str = strtok(c, "\t\r\n");
				if(sscanf(str, "%Ld%c", &i, &garbage) != 1){
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_INPUT_NUMBER;
				}
				else if(i > INT_MAX || i < INT_MIN){
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_INPUT_NUMBER;
				}
				else{
					tmpTable[res.result].data.i_data = i;
					tmpTable[res.result].type = TYPE_INT;
				}
				break;
			case CG_SCAN_DOUBLE:;
				long double d;
				char c2[1024], garbage2, *str2;
				if(fgets(c2, 1023, stdin) == NULL){
					//TODO no line err
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_INTER;
				}
				if(strcmp(c, "\n") == 0){
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_OTHER_RUNTIME;
				}
				str2 = strtok(c2, "\t\r\n");
				if(sscanf(c2, "%Lf%c", &d, &garbage2) != 1){
					//TODO IN Err
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_INPUT_NUMBER;
				}
				else if(d > DBL_MAX || d < DBL_MIN){
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_INPUT_NUMBER;
				}
				else{
					tmpTable[res.result].data.d_data = d;
					tmpTable[res.result].type = TYPE_DOUBLE;
				}
				break;
			case CG_SCAN_STRING:;
				char c3[1024] , *val;
				if(fgets(c3, 1023, stdin) == NULL){
					//TODO no line err
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_INTER;
				}
				if(strcmp(c, "\n") == 0){
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_OTHER_RUNTIME;
				}
				else{				
					val = strtok(c3, "\t\r\n");
					strcpy(tmpTable[res.result].data.s_data, val);
					tmpTable[res.result].type = TYPE_STRING;
				}
				break;
			case CG_LENGTH:
				if(!tmpStack.params[0].init){
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_NOT_INITIALIZED;
				}
				if(tmpStack.params[0].dType != TYPE_STRING){
					free(tmpStack.params); free(class1); free(class2);
					return SEMANTIC_TYPE_ERROR;
				}
				tmpTable[res.t_int].data.i_data = strlen(tmpStack.params[0].data.tString); //TODO res.result nebo res.t_int?
				tmpTable[res.t_int].type = TYPE_INT;
				break;
			case CG_SUBSTR:
				if(!tmpStack.params[0].init || !tmpStack.params[1].init || !tmpStack.params[2].init){
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_NOT_INITIALIZED;
				}
				if(tmpStack.params[0].dType != TYPE_STRING ||
					       	tmpStack.params[1].dType != TYPE_INT || 
						tmpStack.params[2].dType != TYPE_INT){
					free(tmpStack.params); free(class1); free(class2);
					return SEMANTIC_TYPE_ERROR;
				}
				int offset = tmpStack.params[1].data.tInt;
				strncpy(tmpTable[res.t_int].data.s_data, (tmpStack.params[0].data.tString + offset), 
						tmpStack.params[2].data.tInt);
				tmpTable[res.result].type = TYPE_STRING;
				break;
			case CG_COMPARE:
				if(!tmpStack.params[0].init || !tmpStack.params[1].init){
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_NOT_INITIALIZED;
				}
				if(tmpStack.params[0].dType != TYPE_STRING || tmpStack.params[1].dType != TYPE_STRING){
					free(tmpStack.params); free(class1); free(class2);
					return SEMANTIC_ERROR;
				}
				int cmp = strcmp(tmpStack.params[0].data.tString, tmpStack.params[1].data.tString);
				if(cmp < 0)
					tmpTable[res.t_int].data.i_data = -1;
				else if(cmp > 0)
					tmpTable[res.t_int].data.i_data = 1;
				else
					tmpTable[res.t_int].data.i_data = 0;
				tmpTable[res.t_int].type = TYPE_INT;
				break;
			case CG_FIND:
				if(!tmpStack.params[0].init || !tmpStack.params[1].init){
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_NOT_INITIALIZED;
				}
				if(tmpStack.params[0].dType != TYPE_STRING || tmpStack.params[1].dType != TYPE_STRING){
					free(tmpStack.params); free(class1); free(class2);
					return SEMANTIC_ERROR;
				}
				tmpTable[res.t_int].data.i_data = 
					find(tmpStack.params[0].data.tString, tmpStack.params[1].data.tString);
				tmpTable[res.t_int].type = TYPE_INT;
				break;
			case CG_SORT:
				if(!tmpStack.params[0].init){
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_NOT_INITIALIZED;
				}
				if(tmpStack.params[0].dType != TYPE_STRING){
					free(tmpStack.params); free(class1); free(class2);
					return SEMANTIC_ERROR;
				}
				strcpy(tmpTable[res.t_int].data.s_data, sort(tmpStack.params[0].data.tString));
				tmpTable[res.t_int].type = TYPE_STRING;
				break;
			case CG_ACTIVATE:
				tmp = searchFuncVariable(SymbolTable, NULL, context.class, context.function, src1.identifier_name);
				if(tmp == NULL){
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_INTER;
				}
				tmp->tVariable.activated = true;
				//insertFuncVariable(&SymbolTable, context.class, context.function, tmp->tVariable);

				if(ActTable.size == ActTable.maxSize){
					ActTable.var = realloc(ActTable.var, (sizeof(struct sVar)*(10+ActTable.maxSize)));
					if(tmp == NULL){
						free(tmpStack.params); free(class1); free(class2);
						return ERROR_INTER;
					}
					ActTable.maxSize += 10;
				}
				strcpy(ActTable.var[++ActTable.size].name, tmp->key);
				ActTable.var[ActTable.size].depthVar = ActTable.depth;
				break;
			case CG_CALL:;
				//TODO vyresit rekurzi
				tFunctionNodePtr func = searchFunction(SymbolTable, NULL, src2.identifier_name, src1.identifier_name);
				if(func == NULL){
					free(tmpStack.params); free(class1); free(class2);
					return SEMANTIC_ERROR;
				}

				tFunctionNodePtr funRec = searchFunction(SymbolTable, NULL, context.function, context.class);
				VarTable.depth++;
				if(funRec->funcVariable != NULL)
					treewalk(funRec->funcVariable, context.class, context.function);
				tablewalk(SymbolTable);
				if(VarTable.depth == VarTable.maxDepth){
					VarTable.actives = realloc(VarTable.actives, sizeof(tTapeElemPtr)*(VarTable.maxDepth + 10));
					VarTable.maxDepth += 10;
				}
				VarTable.actives[VarTable.depth] = Tape->active;
				Tape->active = Tape->first;


				tParamStack callStack, tableStack;
				callStack = currInstruction->instruction.stack;
				tableStack = func->tFunction.params;
				ActTable.depth++;
				if(callStack.size != tableStack.size){
					//rozdilny pocet params
					free(tmpStack.params); free(class1); free(class2);
					return SEMANTIC_TYPE_ERROR;
				}
				for(int i = 0; i < callStack.size; i++){ // v cyklu kontrola parametru
					if(strlen(callStack.params[i].name) != 0){// je parametrem promena v tabulce?
						if(!callStack.params[i].init){
							free(tmpStack.params); free(class1); free(class2);
							return ERROR_NOT_INITIALIZED;
						}
					
						//je ve funkci?
						tmp = searchFuncVariable(SymbolTable, NULL, callStack.params[i].class, context.function, callStack.params[i].name);
						//tmp = NULL;

						if(tmp == NULL){
							tmp = searchVariable(SymbolTable, NULL, 
									callStack.params[i].name, callStack.params[i].class);

							if(tmp == NULL){//neexistuje promena					
								free(tmpStack.params); free(class1); free(class2);
								return SEMANTIC_ERROR;
							}
						}
						else if(!tmp->tVariable.activated)
							return SEMANTIC_ERROR;
						callStack.params[i].dType = tmp->tVariable.type;
						if(tmp->tVariable.type == TYPE_INT)
							callStack.params[i].data.tInt = tmp->tVariable.content->int_var;
						else if(tmp->tVariable.type == TYPE_DOUBLE)
							callStack.params[i].data.tDouble = 
								tmp->tVariable.content->double_var;
						else if(tmp->tVariable.type == TYPE_STRING)
							strcpy(callStack.params[i].data.tString, 
									tmp->tVariable.content->string_var);
						else{
							free(tmpStack.params); free(class1); free(class2);
							return ERROR_INTER;
						}

					}
					if(callStack.params[i].dType != tableStack.params[i].dType){
						if(callStack.params[i].dType == TYPE_INT && tableStack.params[i].dType == TYPE_DOUBLE){
						}
						else{
						//rozdilne data type v definici a volani
						free(tmpStack.params); free(class1); free(class2);
						return SEMANTIC_TYPE_ERROR;
						}
					}
				}
				for(int i = 0; i < callStack.size; i++){
					tVariableNodePtr funvar = searchFuncVariable(SymbolTable, NULL, src1.identifier_name, src2.identifier_name, tableStack.params[i].name);
					if(funvar != NULL){
						if(funvar->tVariable.type == TYPE_INT)
							funvar->tVariable.content->int_var = callStack.params[i].data.tInt;
						else if(funvar->tVariable.type == TYPE_DOUBLE)
							funvar->tVariable.content->double_var = callStack.params[i].data.tDouble;
						else if(funvar->tVariable.type == TYPE_INT)
							strcpy(funvar->tVariable.content->string_var, callStack.params[i].data.tString);

						continue;						
					}
					tSymbolVariable var;
					var.type = callStack.params[i].dType;
					memset(var.name, 0, 1024);
					strncpy(var.name, tableStack.params[i].name, (strlen(tableStack.params[i].name)));
					var.isConst = false;
					var.longName = NULL;
					var.isSet = true;
					var.content = malloc(sizeof(tContentType));
					var.activated = true;
					if(var.content == NULL){
						free(tmpStack.params); free(class1); free(class2);
						return ERROR_INTER;
					}
					
					if(var.type == TYPE_INT)
						var.content->int_var = callStack.params[i].data.tInt;
					else if(var.type == TYPE_DOUBLE)
						var.content->double_var = callStack.params[i].data.tDouble;
					else if(var.type == TYPE_STRING){
						var.content->string_var = malloc(1024);
						if(var.content->string_var == NULL){
							free(tmpStack.params); free(class1); free(class2), free(var.content);
							return ERROR_INTER;
						}
						strcpy(var.content->string_var, callStack.params[i].data.tString);
					}
					else{
						free(tmpStack.params); free(class1); free(class2), free(var.content);
						return ERROR_INTER;
					}

					insertFuncVariable(&SymbolTable, src1.identifier_name, src2.identifier_name, var);
				}

				pushStack(&contextStack,(void*)&context);
				State = set_tape(src1.identifier_name, src2.identifier_name);
				if(State !=OK_E){
					free(tmpStack.params); free(class1); free(class2);
					return State;
				}
				currInstruction = Tape->active;
				continue;
			case CG_RET:
				if(!init1){
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_NOT_INITIALIZED;
				}
				while(ActTable.var[ActTable.size].depthVar == ActTable.depth && ActTable.size != 0){
					tmp = searchFuncVariable(SymbolTable, NULL, 
							context.class, context.function, ActTable.var[ActTable.size].name);
					if(tmp == NULL){
						free(tmpStack.params); free(class1); free(class2);
						return ERROR_INTER;
					}
					else if(!tmp->tVariable.activated)
						return SEMANTIC_ERROR;
					tmp->tVariable.activated = false;
					//insertFuncVariable(&SymbolTable, context.class, context.function, tmp->tVariable);
					ActTable.size--;
				}
				ActTable.depth--;
				tFunctionNodePtr tmpfunc = searchFunction(SymbolTable, NULL, context.function, context.class);
				if(tmpfunc == NULL){
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_INTER;
				}
				if(src1.dtype != tmpfunc->tFunction.ret_type){
					if(!(tmpfunc->tFunction.ret_type == TYPE_DOUBLE && src1.dtype == TYPE_INT)){
					free(tmpStack.params); free(class1); free(class2);
					return SEMANTIC_TYPE_ERROR;
					}
				}
				struct cont tmpCont;
				popStack(&contextStack, (void*)&tmpCont);
				State = set_tape(tmpCont.class, tmpCont.function);
				if(State != OK_E){
					free(tmpStack.params); free(class1); free(class2);
					return State;
				}
				if(src1.dtype == TYPE_INT){
					tmpTable[res.result].data.i_data = src1.t_int;
					tmpTable[res.result].type = TYPE_INT;
				}
				else if(src1.dtype == TYPE_DOUBLE){
					tmpTable[res.result].data.d_data = src1.t_double;
					tmpTable[res.result].type = TYPE_DOUBLE;
				}
				else if(src1.dtype == TYPE_STRING){
					strcpy(tmpTable[res.result].data.s_data, src1.t_string);
					tmpTable[res.result].type = TYPE_STRING;
				}
				else if (src1.dtype == TYPE_VOID){
					tmpTable[res.result].type = TYPE_VOID;
				}
				else{
					free(tmpStack.params); free(class1); free(class2);
					return ERROR_INTER;
				}
				
				writeToTable();
				Tape->active = VarTable.actives[VarTable.depth];
				VarTable.depth--;

				break;
				//TODO return data

			default:
				free(tmpStack.params); free(class1); free(class2);
				return ERROR_INTER;				
				break;

		}
		Tape->active = Tape->active->next;
		currInstruction = Tape->active;			
		//free(tmpStack.params);
	}
	free(class1); free(class2);
	if((strcmp(context.class, "ifj16") != 0) || (strcmp(context.function, "init") != 0))
		return 	SEMANTIC_TYPE_ERROR;
	else
		return OK_E;	
}































