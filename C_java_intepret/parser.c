/**
 * @file parser.c
 * @author Petr Flajsingr (xflajs00@stud.fit.vutbr.cz)
 * @date 9 october 2016
 * @brief Soubor obsahujici definici funkci pro syntaktickou a semantickou analyzi
**/
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "lex.h"
#include "s_table.h"
#include "error.h"
#include "code_gen.h"
#include "stack.h"
#include "datatypes.h"
#include "funcpar.h"
#include "interfaces.h"
#include "etok.h"
#include "expr.h"

//DEBUG
//int lvl = 0;
//#define printfD(x) lvl++; for(int i = 0; i < lvl; i++) printf("    "); printf(x); fflush(stdout);
//#define printfDc(x) for(int i = 0; i < lvl; i++) printf("    "); printf(x); lvl--; fflush(stdout);
//DEBUG

// Globalni promenne
extern FILE* source; // Zdrojovy soubor
extern Tok_T* token; //Lexem
extern tClassNodePtr SymbolTable; //tabulka symbolu
extern tTape* Tape; //paska instrukci
char curClass[100]; //aktualni trida
char curFunc[100]; //aktualni funkce
extern char** arg;
int lev;
bool isVoid;
// \Globalni promenne

/**
 * Hlavni funkce parseru
 * @return OK_E pri projiti bez chyby
 **/
int parser(){
//	printfD("parser\n");
	int ret = 0;
	lev = -1;
	initTable(&SymbolTable);
	//vstupni funkce programu
	tSymbolFunction fun;
	strcpy(fun.name, "init");
	insertClass(&SymbolTable, "ifj16");
	insertFunction(&SymbolTable, "ifj16", fun);
	//\vstupni funkce programu
	do{
		if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
		if(token->type == EOF_T){
			tParamStack param;
			initParams(&param);
			set_tape("ifj16", "init");
			insert_call("Main", "run", &param);
			return OK_E;
		}
		if((ret = classCheck()) != 0){
			return ret;
		}
	}while(1);
}

/**
 * Kontrola spravnosti definice tridy
 * @return OK_E bez chyby
 **/
int classCheck(){
//	printfD("classCheck\n");
	set_tape("ifj16", "init");
	strcpy(curFunc, "");
	if(token->type == CLASS){ //class
		if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
		if(token->type == IDENTIFIER){ //class <id>
			if(searchClass(SymbolTable, token->identifier_name) || strcmp("ifj16", token->identifier_name) == 0){
				return SEMANTIC_ERROR;
			}
			insertClass(&SymbolTable, token->identifier_name); //ulozeni tridy
			strcpy(curClass, token->identifier_name); //ulozeni aktualni tridy pro ukladani promennych a funkci
			if(GetNewToken(source, token, false) == LEX_ERROR) return LEX_ERROR;
			if(token->type == LEFT_DOUBLE_BRACKET){ //class <id> {
				int temp;
				while((temp = classMember()) == 0); //kontrola vsech members
				if(temp != -1 && temp != 0){ //chyba v classMember()
					return temp;
				}
			}else{
				return SYNTAX_ERROR;
			}
		}else{
			return SYNTAX_ERROR;
		}
	}else{
		return SYNTAX_ERROR;
	}
//	printfDc("/classCheck\n");
	return OK_E;
}

/**
 * Kontrola polozek uvnitr tridy
 * @return OK_E bez chyby
 **/
int classMember(){
//	printfD("classMember\n");
	if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
	if(token->type == STATIC){ //static
		if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
		if(token->type == VOID){ //static void
			isVoid = true;
			if(GetNewToken(source, token, false) == LEX_ERROR) return LEX_ERROR;
			if(token->type == IDENTIFIER){ //static void <id>
				//ulozeni info o funkci
				tSymbolFunction function;
				strcpy(function.name, token->identifier_name);
				function.ret_type = TYPE_VOID;
				strcpy(curFunc, token->identifier_name);
				if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
				if(token->type == LEFT_BRACKET){ //static void <id> (
					initParams(&(function.params));
					do{ //ulozeni parametru
						tParam param;
						memset(&param, 0, sizeof(tParam));
						if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
						if(token->type != RIGHT_BRACKET){ //static void <id> (...
							switch(token->type){
								case INT: param.dType = TYPE_INT; break;
								case DOUBLE: param.dType = TYPE_DOUBLE; break;
								case STRING: param.dType = TYPE_STRING; break;
								default: return SYNTAX_ERROR;
							}
							if(GetNewToken(source, token, false) == LEX_ERROR){
								return LEX_ERROR;
							}
							if(token->type == IDENTIFIER){
								strcpy(param.name, token->identifier_name);
								if(searchParam(&(function.params), param.name)){
									return SEMANTIC_ERROR;
								}
								pushParam(&(function.params), param);
							}else{
								return SYNTAX_ERROR;
							}
							if(GetNewToken(source, token, false) == LEX_ERROR){
								return LEX_ERROR;
							}
						}
					}while(token->type == COMMA); //vice parametru
					if(token->type == RIGHT_BRACKET){ //static void <id>(...)
						if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;	
						if(token->type == LEFT_DOUBLE_BRACKET){ //static void <id>(...){
							if(searchFunction(SymbolTable, NULL, function.name, curClass) || \
							searchVariable(SymbolTable, NULL, function.name, curClass)){ //kontrola konfliktu deklarace
								return SEMANTIC_ERROR;
							}
							//paska pro instrukce
							init_tape(&function.code);
							insertFunction(&SymbolTable, curClass, function); //ulozeni funkce do stromu
							set_tape(curClass, curFunc);
							int ret = blockBody(); //telo funkce
							tOperation op;
							op.operator.tokType = (TokType)CG_RET;
							op.left.dtype = TYPE_VOID;
							op.left.result = -1;
							op.right.result = -1;
							op.left.tokType = START;
							generate_op(op);
							return ret;
						}else{
							return SYNTAX_ERROR;
						}
					}else{
						return SYNTAX_ERROR;
					}
				}else{
					return SYNTAX_ERROR;
				}
			}else{
				return SYNTAX_ERROR;
			}
		}else if(token->type == INT || token->type == DOUBLE || token->type == STRING){ //static <data_type>
			isVoid = false;
		 	tDatatype temp;
		 	switch(token->type){ //ulozeni datoveho typu
		 		case INT: temp = TYPE_INT; break;
		 		case DOUBLE: temp = TYPE_DOUBLE; break;
		 		case STRING: temp = TYPE_STRING; break;
		 		default: return ERROR_INTER;
		 	}
			if(GetNewToken(source, token, false) == LEX_ERROR) return LEX_ERROR;
			if(token->type == IDENTIFIER){ //static <data_type> <id>
				char name[100];
				strcpy(name, token->identifier_name);
				if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
				if(token->type == LEFT_BRACKET){ //static <data_type> <id>(
					//ulozeni info o funkci
					strcpy(curFunc, name);
					tSymbolFunction function;
					strcpy(function.name, name);
					function.ret_type = temp;
					initParams(&(function.params));
					do{//nacteni parametru
						tParam param;
						memset(&param, 0, sizeof(tParam));
						if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
						if(token->type != RIGHT_BRACKET){
							switch(token->type){
								case INT: param.dType = TYPE_INT; break;
								case DOUBLE: param.dType = TYPE_DOUBLE; break;
								case STRING: param.dType = TYPE_STRING; break;
								default: return SYNTAX_ERROR;
							}
							if(GetNewToken(source, token, false) == LEX_ERROR){
								return LEX_ERROR;
							}
							if(token->type == IDENTIFIER){
								strcpy(param.name, token->identifier_name);
								if(searchParam(&(function.params), param.name)){
									return SEMANTIC_ERROR;
								}
								pushParam(&(function.params), param);
							}else{
								return SYNTAX_ERROR;
							}
							if(GetNewToken(source, token, false) == LEX_ERROR){
								return LEX_ERROR;
							}
						}
					}while(token->type == COMMA);
					if(token->type == RIGHT_BRACKET){ //static <data_type> <id>(...)
						if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;	
						if(token->type == LEFT_DOUBLE_BRACKET){
							if(searchFunction(SymbolTable, NULL, function.name, curClass) || \
							searchVariable(SymbolTable, NULL, function.name, curClass)){ //kontrola konfliktu
								return SEMANTIC_ERROR;
							}
							//paska pro instrukce
							init_tape(&function.code);
							insertFunction(&SymbolTable, curClass, function); //ulozeni funkce
							set_tape(curClass, curFunc);
							return blockBody();
						}else{
							return SYNTAX_ERROR;
						}
					}else{
						return SYNTAX_ERROR;
					}
				}else if(token->type == SEMICOLON){ //static <data_type> <id>;
					tSymbolVariable var;
					strcpy(var.name, name);
					var.type = temp;
					var.content = malloc(sizeof(tContentType));
					var.isConst = false;
					var.isSet = false;
					var.longName = NULL;
					if(temp == TYPE_STRING){
						var.content->string_var = malloc(sizeof(char) * 1024);
					}
					if(searchFunction(SymbolTable, NULL, var.name, curClass) || \
						searchVariable(SymbolTable, NULL, var.name, curClass)){ //kontrola konfliktu
						return SEMANTIC_ERROR;
					}
					insertVariable(&SymbolTable, curClass, var);
				}else if(token->type == ASSIGN){ //prirazeni pri inicializaci
					tSymbolVariable var;
					strcpy(var.name, name);
					var.type = temp;
					var.content = malloc(sizeof(tContentType));
					var.isConst = false;
					var.isSet = false;
					var.longName = NULL;
					if(temp == TYPE_STRING){
						var.content->string_var = malloc(sizeof(char) * 1024);
					}
					if(searchFunction(SymbolTable, NULL, var.name, curClass) || \
						searchVariable(SymbolTable, NULL, var.name, curClass)){ //kontrola konfliktu
						return SEMANTIC_ERROR;
					}
					insertVariable(&SymbolTable, curClass, var);
					if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
					Tok_T tempTok = *token;
					if(token->type == READINT || token->type == READDOUBLE || token->type == READSTRING\
						|| token->type == LENGTH || token->type == SUBSTR || token->type == COMPARE\
						|| token->type == FIND || token->type == SORT){ //vestavene funkce
						Errors err;
						tOpResultId returnId = 0;
						if((err = checkPKeyword(token->type, &returnId)) != OK_E){
							return err;
						}
						tOperation op;
						op.operator.tokType = (TokType)CG_ASSIGN;
						op.left.tokType = IDENTIFIER;
						strcpy(op.left.identifier_name, var.name);
						strcpy(op.left.t_string, curClass);
						op.right.dtype = (tDatatype)BOOL_TYPE;
						op.right.result = returnId;
						generate_op(op); //operace prirazeni vysledku
					}else if(token->type == IDENTIFIER || token->type == IDENTIFIER_2){ //<id> = <id2>
						char* name = malloc(sizeof(char) * strlen(token->identifier_name));
						strcpy(name, token->identifier_name);
						char* class = "";
						if(token->type == IDENTIFIER_2){
							char* tmp = malloc(sizeof(char) * strlen(token->identifier_name));
							strcpy(tmp, token->identifier_name);
							unsigned int pos = strstr(tmp, ".") - tmp;
							class = malloc(sizeof(char) * (pos + 1));
							name = malloc(sizeof(char) * (strlen(tmp) - pos));
							strncpy(class, tmp, pos);
							class[pos] = '\0';
							strncpy(name, tmp + pos + 1, (strlen(tmp) - pos));
							free(tmp);
						}
						if(GetNewToken(source, token, true) == 1) return LEX_ERROR;
						if(token->type == LEFT_BRACKET){ //<id> = <id2>(
							if(GetNewToken(source, token, false) == LEX_ERROR) return LEX_ERROR;
							tOpResultId returnRes;
							returnRes = functionCall(class, name); //volani funkce
							if(GetNewToken(source, token, false) == LEX_ERROR) return LEX_ERROR;
							if(token->type == SEMICOLON){ //<id> = <id2>(...);
								tOperation op;
								op.operator.tokType = (TokType)CG_ASSIGN;
								op.left.tokType = IDENTIFIER;
								strcpy(op.left.identifier_name, var.name);
								strcpy(op.left.t_string, curClass);
								op.right.dtype = (tDatatype)BOOL_TYPE;
								op.right.result = returnRes;
								generate_op(op);
							}else{
								free(name);
								return SYNTAX_ERROR;
							}
						}else{
							int a = 1;
							while(token->type != ASSIGN){
								fseek(source, -a, SEEK_CUR);
								GetNewToken(source, token, false);
								a++;
							}
							if(GetNewToken(source, token, false) == LEX_ERROR) return LEX_ERROR;
							exp_status e = parse_expr(tempTok);
							if(e.b != SEMI){
								free(name);
								return SYNTAX_ERROR;
							}
							tOperation op;
							op.left.tokType = IDENTIFIER;
							strcpy(op.left.identifier_name, var.name);
							strcpy(op.left.t_string, curClass);
							op.operator.tokType = (TokType)CG_ASSIGN;
							if(e.token.result != -1){
								op.right.dtype = (tDatatype)BOOL_TYPE;
								op.right.result = e.token.result;
							}else{
								op.left.tokType = IDENTIFIER;
								strcpy(op.right.identifier_name, name);
								strcpy(op.right.t_string, class);
								op.right.result = -1;
							}
							generate_op(op);
							free(name);
						}
					}else if(token->type == T_INT || token->type == T_DOUBLE || \
						token->type == T_STRING || token->type == MINUS || token->type == LEFT_BRACKET){
							exp_status e = parse_expr(*token);
							if(e.b != SEMI){
								return SYNTAX_ERROR;
							}
							tOperation op;
							op.left.tokType = IDENTIFIER;
							strcpy(op.left.identifier_name, var.name);
							strcpy(op.left.t_string, curClass);
							op.operator.tokType = (TokType)CG_ASSIGN;
							op.left.result = -1;
							if(e.token.result != -1){
								op.right.dtype = (tDatatype)BOOL_TYPE;
								op.right.result = e.token.result;
							}else{
								op.right.tokType = 0;
								switch(token->type){
									case T_INT: op.right.dtype = TYPE_INT; op.right.t_int = token->t_int; break;
									case T_DOUBLE: op.right.dtype = TYPE_DOUBLE; op.right.t_double = token->t_double; break;
									case T_STRING: op.right.dtype = TYPE_STRING; strcpy(op.right.t_string, token->t_string); break;
									default: return ERROR_INTER;
								}
								op.right.result = -1;
							}
							generate_op(op);
					}else{
						return SYNTAX_ERROR;
					}
					return OK_E;
				}else{
					return SYNTAX_ERROR;
				}
			}else{
				return SYNTAX_ERROR;
			}
		}else{
			return SYNTAX_ERROR;
		}
	}else if(token->type == RIGHT_DOUBLE_BRACKET){ //class <id> {...}
//		printfDc("/classMember\n");
		return -1;
	}else{
		return SYNTAX_ERROR;
	}
//	printfDc("/classMember\n");	
	return OK_E;
}

/**
 * Kontrola bloku prikazu (slozeny prikaz)
 * @return OK_E bez chyby
 **/
int blockBody(){
//	printfD("blockBody\n");
	lev++;
	int ret = 0;
	if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
	if(token->type == RIGHT_DOUBLE_BRACKET){ // }, prazdny blok
//		printfDc("/blockBody\n");
		lev--;
		return OK_E;
	}
	do{
		if(token->type == PRINT){ //ifj16.print()
			ret = checkPrint();
			if(ret != 0){
				return ret;
			}
		}else if(token->type == READINT || token->type == READDOUBLE || token->type == READSTRING\
						|| token->type == LENGTH || token->type == SUBSTR || token->type == COMPARE\
						|| token->type == FIND || token->type == SORT){
			int returnId;
			ret = checkPKeyword(token->type, &returnId);
			if(ret != 0){
				return ret;
			}
		}else if((token->type == INT || token->type == DOUBLE || token->type == STRING) && lev == 0){//<datovy_typ>, deklarace promenne
			tDatatype type;
			switch(token->type){
				case INT: type = TYPE_INT; break;
				case DOUBLE: type = TYPE_DOUBLE; break;
				case STRING: type = TYPE_STRING; break;
				default: return ERROR_INTER;
			}
			ret = blockVar(type);
			if(ret != 0){
				return ret;
			}
		}else if(token->type == IDENTIFIER || token->type == IDENTIFIER_2){ //<id>
			char* name = malloc(sizeof(char) * strlen(token->identifier_name));
			strcpy(name, token->identifier_name);
			char* class = "";
			if(token->type == IDENTIFIER_2){
				char* tmp = malloc(sizeof(char) * strlen(token->identifier_name));
				strcpy(tmp, token->identifier_name);
				unsigned int pos = strstr(tmp, ".") - tmp;
				class = malloc(sizeof(char) * (pos + 1));
				name = malloc(sizeof(char) * (strlen(tmp) - pos));
				strncpy(class, tmp, pos);
				class[pos] = '\0';
				strncpy(name, tmp + pos + 1, (strlen(tmp) - pos));
				free(tmp);
			}
			if(GetNewToken(source, token, false) == LEX_ERROR) return LEX_ERROR;
			if(token->type == LEFT_BRACKET){ //<id> (
				ret = functionCall(class, name); //volani funkce
				if(ret != 0){
					return ret;
				}
				if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
				if(token->type != SEMICOLON){ //<id> (...);
					return SYNTAX_ERROR;
				}
			}else{
				if(token->type == ASSIGN){ //<id> =)
					if(GetNewToken(source, token, false) == LEX_ERROR) return LEX_ERROR;
					Tok_T tempTok = *token;
					if(token->type == READINT || token->type == READDOUBLE || token->type == READSTRING\
						|| token->type == LENGTH || token->type == SUBSTR || token->type == COMPARE\
						|| token->type == FIND || token->type == SORT){ //vestavene funkce
						Errors err;
						tOpResultId returnId = 0;;
						if((err = checkPKeyword(token->type, &returnId)) != OK_E){
							return err;
						}
						tOperation op;
						op.operator.tokType = (TokType)CG_ASSIGN;
						op.left.tokType = IDENTIFIER;
						op.left.result = -1;
						strcpy(op.left.identifier_name, name);
						strcpy(op.left.t_string, class);
						op.right.dtype = (tDatatype)BOOL_TYPE;
						op.right.result = returnId;
						generate_op(op); //operace prirazeni vysledku
					}else if(token->type == IDENTIFIER || token->type == IDENTIFIER_2){ //<id> = <id2>
						char* name2 = malloc(sizeof(char) * strlen(token->identifier_name));
						strcpy(name2, token->identifier_name);
						char* class2 = "";
						if(token->type == IDENTIFIER_2){
							char* tmp = malloc(sizeof(char) * strlen(token->identifier_name));
							strcpy(tmp, token->identifier_name);
							unsigned int pos = strstr(tmp, ".") - tmp;
							class2 = malloc(sizeof(char) * (pos+1));
							name2 = malloc(sizeof(char) * (strlen(tmp) - pos));
							strncpy(class2, tmp, pos);
							class2[pos] = '\0';
							strncpy(name2, tmp + pos + 1, (strlen(tmp) - pos));
							free(tmp);
						}
						if(GetNewToken(source, token, true) == 1) return LEX_ERROR;
						if(token->type == LEFT_BRACKET){ //<id> = <id2>(
							if(GetNewToken(source, token, false) == LEX_ERROR) return LEX_ERROR;
							tOpResultId returnRes;
							returnRes = functionCall(class2, name2); //volani funkce
							if(GetNewToken(source, token, false) == LEX_ERROR) return LEX_ERROR;
							if(token->type == SEMICOLON){ //<id> = <id2>(...);
								tOperation op;
								op.operator.tokType = (TokType)CG_ASSIGN;
								op.left.tokType = IDENTIFIER;
								strcpy(op.left.identifier_name, name);
								strcpy(op.left.t_string, class);
								op.right.dtype = (tDatatype)BOOL_TYPE;
								op.right.result = returnRes;
								generate_op(op);
							}else{
								free(name);
								free(name2);
								return SYNTAX_ERROR;
							}
						}else{
							int a = 1;
							while(token->type != ASSIGN){
								fseek(source, -a, SEEK_CUR);
								GetNewToken(source, token, false);
								a++;
							}
							if(GetNewToken(source, token, false) == LEX_ERROR) return LEX_ERROR;
							exp_status e = parse_expr(tempTok);
							if(e.b != SEMI){
								free(name2);
								free(name);
								return SYNTAX_ERROR;
							}
							tOperation op;
							op.left.tokType = IDENTIFIER;
							strcpy(op.left.identifier_name, name);
							strcpy(op.left.t_string, class);
							op.operator.tokType = (TokType)CG_ASSIGN;
							op.left.result = -1;
							if(e.token.result != -1){
								op.right.dtype = (tDatatype)BOOL_TYPE;
								op.right.result = e.token.result;
							}else{
								op.left.tokType = IDENTIFIER;
								strcpy(op.right.identifier_name, name2);
								strcpy(op.right.t_string, class2);
								op.right.result = -1;
							}
							generate_op(op);
						}
					}else if(token->type == T_INT || token->type == T_DOUBLE || \
						token->type == T_STRING || token->type == LEFT_BRACKET){
							exp_status e = parse_expr(*token);
							if(e.b != SEMI){
								free(name);
								return SYNTAX_ERROR;
							}
							tOperation op;
							op.left.tokType = IDENTIFIER;
							strcpy(op.left.identifier_name, name);
							strcpy(op.left.t_string, class);
							op.operator.tokType = (TokType)CG_ASSIGN;
							op.left.result = -1;
							if(e.token.result != -1){
								op.right.dtype = (tDatatype)BOOL_TYPE;
								op.right.result = e.token.result;
							}else{
								op.right.tokType = 0;
								switch(token->type){
									case T_INT: op.right.dtype = TYPE_INT; op.right.t_int = token->t_int; break;
									case T_DOUBLE: op.right.dtype = TYPE_DOUBLE; op.right.t_double = token->t_double; break;
									case T_STRING: op.right.dtype = TYPE_STRING; strcpy(op.right.t_string, token->t_string); break;
									default: return ERROR_INTER;
								}
								op.right.result = -1;
							}
							generate_op(op);
					}else{
						free(name);
						return SYNTAX_ERROR;
					}
				}else{ //<id>++/--
					tOperation op;
					strcpy(op.left.identifier_name, name);
					strcpy(op.left.t_string, class);
					op.left.tokType = IDENTIFIER;
					switch(token->type){
						case PLUS_PLUS: op.operator.tokType = (TokType)CG_INC; break;
						case MINUS_MINUS: op.operator.tokType = (TokType)CG_DEC; break;
						default: return SYNTAX_ERROR;
					}
					if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
					if(token->type != SEMICOLON){ //<id>++/--;
						free(name);
						return SYNTAX_ERROR;
					}
					generate_op(op);
				}
			}
			free(name);
		}else if(token->type == PLUS_PLUS || token->type == MINUS_MINUS){ //++/--
			tOperation op;
			switch(token->type){
				case PLUS_PLUS: op.operator.tokType = (TokType)CG_INC; break;
				case MINUS_MINUS: op.operator.tokType = (TokType)CG_DEC; break;
				default: return ERROR_INTER;
			}
			if(GetNewToken(source, token, false) == LEX_ERROR) return LEX_ERROR;
			if(token->type == IDENTIFIER || token->type == IDENTIFIER_2){ //++/--<id>
				char* name = malloc(sizeof(char) * strlen(token->identifier_name));
				strcpy(name, token->identifier_name);
				char* class = "";
				if(token->type == IDENTIFIER_2){
					char* tmp = malloc(sizeof(char) * strlen(token->identifier_name));
					strcpy(tmp, token->identifier_name);
					unsigned int pos = strstr(tmp, ".") - tmp;
					class = malloc(sizeof(char) * pos);
					name = malloc(sizeof(char) * (strlen(tmp) - pos));
					strncpy(class, tmp, pos);
					strncpy(name, tmp + pos + 1, (strlen(tmp) - pos));
					free(tmp);
				}
				strcpy(op.left.identifier_name, name);
				strcpy(op.left.t_string, class);
				op.left.tokType = IDENTIFIER;
				free(name);
			}else{
				return SYNTAX_ERROR;
			}
			if(GetNewToken(source, token, false) == LEX_ERROR) return LEX_ERROR;
			if(token->type != SEMICOLON){ //++/--<id>;
				return SYNTAX_ERROR;
			}
			op.left.result = -1;
			generate_op(op);
		}else if(token->type == IF){ //if
			ret = checkIf();
			if(ret != 0){
				return ret;
			}
		}else if(token->type == WHILE){ //while
			ret = checkWhile();
			if(ret != 0){
				return ret;
			}
		}else if(token->type == LEFT_DOUBLE_BRACKET){ //zanoreny blok prikazu
			ret = blockBody();
			if(ret != 0){
				return ret;
			}
		}else if(token->type == RETURN){
			if(isVoid){
				if(GetNewToken(source, token, false) == LEX_ERROR) return LEX_ERROR;
				if(token->type == SEMICOLON){
					tOperation op;
					op.operator.tokType = (TokType)CG_RET;
					op.left.dtype = (tDatatype)TYPE_VOID;
					op.left.result = -1;
					op.right.result = -1;
					generate_op(op);
				}else{
					return SEMANTIC_TYPE_ERROR;
				}
			}else{
				if(GetNewToken(source, token, false) == LEX_ERROR) return LEX_ERROR;
				Tok_T tempTok = *token;
				if(token->type == IDENTIFIER || token->type == IDENTIFIER_2){ //return <id>
					char* name2 = malloc(sizeof(char) * strlen(token->identifier_name));
					strcpy(name2, token->identifier_name);
					char* class2 = "";
					if(token->type == IDENTIFIER_2){
						char* tmp = malloc(sizeof(char) * strlen(token->identifier_name));
						strcpy(tmp, token->identifier_name);
						unsigned int pos = strstr(tmp, ".") - tmp;
						class2 = malloc(sizeof(char) * (pos + 1));
						name2 = malloc(sizeof(char) * (strlen(tmp) - pos));
						strncpy(class2, tmp, pos);
						class2[pos] = '\0';
						strncpy(name2, tmp + pos + 1, (strlen(tmp) - pos));
						free(tmp);
					}
					if(GetNewToken(source, token, true) == 1) return LEX_ERROR;
					if(token->type == LEFT_BRACKET){ //<id> = <id2>(
						if(GetNewToken(source, token, false) == LEX_ERROR) return LEX_ERROR;
						tOpResultId returnRes;
						returnRes = functionCall(class2, name2); //volani funkce
						if(GetNewToken(source, token, false) == LEX_ERROR) return LEX_ERROR;
						if(token->type == SEMICOLON){ //<id> = <id2>(...);
							tOperation op;
							op.left.tokType = (tDatatype)BOOL_TYPE;
							op.left.result = returnRes;
							op.operator.tokType = (TokType)CG_RET;
							generate_op(op);
						}else{
							free(name2);
							return SYNTAX_ERROR;
						}
					}else{
						int a = 1;
						while(token->type != RETURN){
							fseek(source, -a, SEEK_CUR);
							GetNewToken(source, token, false);
							a++;
						}
						if(GetNewToken(source, token, false) == LEX_ERROR) return LEX_ERROR;
						exp_status e = parse_expr(tempTok);
						if(e.b != SEMI){
							free(name2);
							return SYNTAX_ERROR;
						}
						tOperation op;
						if(e.token.result != -1){
							op.left.dtype = (tDatatype)BOOL_TYPE;
							op.left.result = e.token.result;
						}else{
							op.left.tokType = IDENTIFIER;
							strcpy(op.left.identifier_name, name2);
							strcpy(op.left.t_string, class2);
							op.left.result = -1;
						}
						op.right.result = -1;
						op.operator.tokType = (TokType)CG_RET;
						generate_op(op);			
					}
				}else{ 
					exp_status e = parse_expr(*token);
					if(e.b != SEMI){
						return SYNTAX_ERROR;
					}
					tOperation op;
					op.operator.tokType = (TokType)CG_RET;
					strcpy(op.left.identifier_name, "");
					if(e.token.result != -1){
						op.left.dtype = (tDatatype)BOOL_TYPE;
						op.left.result = e.token.result;
					}else{
						op.left.tokType = 0;
						switch(token->type){
							case T_INT: op.left.dtype = TYPE_INT; op.left.t_int = token->t_int; break;
							case T_DOUBLE: op.left.dtype = TYPE_DOUBLE; op.left.t_double = token->t_double; break;
							case T_STRING: op.left.dtype = TYPE_STRING; strcpy(op.left.t_string, token->t_string); break;
							default: return ERROR_INTER;
						}
						op.left.result = -1;
					}
					op.right.result = -1;
					generate_op(op);
				}
			}
		}else{
			return SYNTAX_ERROR;
		}
		if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
	}while(token->type != RIGHT_DOUBLE_BRACKET); //konec bloku prikazu
//	printfDc("/blockBody\n");
	lev--;
	return OK_E;
}

/**
 * Kontrola definice/deklarace promenne v bloku prikazu
 * @return OK_E bez chyby
 **/
int blockVar(tDatatype type){
//	printfD("blockVar\n");
	//nastaveni parametru promenne
	tSymbolVariable var;
	var.type = type;
	var.content = malloc(sizeof(tContentType));
	var.longName = NULL;
	if(type == TYPE_STRING){
		var.content->string_var = malloc(sizeof(char) * 1024);
	}
	var.isConst = false;
	if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
	if(token->type == IDENTIFIER_2){
		return SEMANTIC_ERROR;
	}
	if(token->type == IDENTIFIER){ // <datovy_typ> <id>
		strcpy(var.name, token->identifier_name);
		tOperation op;
		strcpy(op.left.identifier_name, var.name);
		op.operator.tokType = (TokType)CG_ACTIVATE;
		op.left.result = -1;
		generate_op(op);
		if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
		var.isSet = false;
		if(searchFuncVariable(SymbolTable, NULL, curClass, curFunc, var.name) || \
			searchFunction(SymbolTable, NULL, var.name, curClass)){
			return SEMANTIC_ERROR;
		}
		insertFuncVariable(&SymbolTable, curClass, curFunc, var);
		if(token->type == SEMICOLON){ // <datovy_typ> <id>;
//			printfDc("/blockVar\n");
			return OK_E;
		}else if(token->type == ASSIGN){ // <datovy_typ> <id> =
			if(GetNewToken(source, token, false) == LEX_ERROR) return LEX_ERROR;
			Tok_T tempTok = *token;
			if(token->type == READINT || token->type == READDOUBLE || token->type == READSTRING\
				|| token->type == LENGTH || token->type == SUBSTR || token->type == COMPARE\
				|| token->type == FIND || token->type == SORT){ //vestavene funkce
				Errors err;
				tOpResultId returnId = 0;
				if((err = checkPKeyword(token->type, &returnId)) != OK_E){
					return err;
				}
				tOperation op;
				op.operator.tokType = (TokType)CG_ASSIGN;
				op.left.tokType = IDENTIFIER;
				op.left.result = -1;
				strcpy(op.left.identifier_name, var.name);
				strcpy(op.left.t_string, "");
				op.right.dtype = (tDatatype)BOOL_TYPE;
				op.right.result = returnId;
				generate_op(op); //operace prirazeni vysledku
			}else if(token->type == IDENTIFIER || token->type == IDENTIFIER_2){ //<id> = <id2>
				char* name = malloc(sizeof(char) * strlen(token->identifier_name));
				strcpy(name, token->identifier_name);
				char* class = "";
				if(token->type == IDENTIFIER_2){
					char* tmp = malloc(sizeof(char) * strlen(token->identifier_name));
					strcpy(tmp, token->identifier_name);
					unsigned int pos = strstr(tmp, ".") - tmp;
					class = malloc(sizeof(char) * (pos+1));
					name = malloc(sizeof(char) * (strlen(tmp) - pos));
					strncpy(class, tmp, pos);
					class[pos] = '\0';
					strncpy(name, tmp + pos + 1, (strlen(tmp) - pos));
					free(tmp);
				}
				if(GetNewToken(source, token, true) == 1) return LEX_ERROR;
				if(token->type == LEFT_BRACKET){ //<id> = <id2>(
					if(GetNewToken(source, token, false) == LEX_ERROR) return LEX_ERROR;
					tOpResultId returnRes;
					returnRes = functionCall(class, name); //volani funkce
					if(GetNewToken(source, token, false) == LEX_ERROR) return LEX_ERROR;
					if(token->type == SEMICOLON){ //<id> = <id2>(...);
						tOperation op;
						op.operator.tokType = (TokType)CG_ASSIGN;
						op.left.tokType = IDENTIFIER;
						op.left.result = -1;
						strcpy(op.left.identifier_name, var.name);
						strcpy(op.left.t_string, "");
						op.right.dtype = (tDatatype)BOOL_TYPE;
						op.right.result = returnRes;
						generate_op(op);
					}else{
						free(name);
						return SYNTAX_ERROR;
					}
				}else{
					int a = 1;
					while(token->type != ASSIGN){
						fseek(source, -a, SEEK_CUR);
						GetNewToken(source, token, false);
						a++;
					}
					if(GetNewToken(source, token, false) == LEX_ERROR) return LEX_ERROR;
					exp_status e = parse_expr(tempTok);
					if(e.b != SEMI){
						free(name);
						return SYNTAX_ERROR;
					}
					tOperation op;
					op.left.tokType = IDENTIFIER;
					strcpy(op.left.identifier_name, var.name);
					strcpy(op.left.t_string, "");
					op.operator.tokType = (TokType)CG_ASSIGN;
					if(e.token.result != -1){
						op.right.dtype = (tDatatype)BOOL_TYPE;
						op.right.result = e.token.result;
					}else{
						op.left.tokType = IDENTIFIER;
						strcpy(op.right.identifier_name, name);
						strcpy(op.right.t_string, class);
						op.right.result = -1;
					}
					generate_op(op);
				}
			}else if(token->type == T_INT || token->type == T_DOUBLE || \
				token->type == T_STRING || token->type == MINUS || token->type == LEFT_BRACKET){
					exp_status e = parse_expr(*token);
					if(e.b != SEMI){
						return SYNTAX_ERROR;
					}
					tOperation op;
					op.left.tokType = IDENTIFIER;
					strcpy(op.left.identifier_name, var.name);
					strcpy(op.left.t_string, "");
					op.operator.tokType = (TokType)CG_ASSIGN;
					op.left.result = -1;
					if(e.token.result != -1){
						op.right.dtype = (tDatatype)BOOL_TYPE;
						op.right.result = e.token.result;
					}else{
						op.right.tokType = 0;
						switch(token->type){
							case T_INT: op.right.dtype = TYPE_INT; op.right.t_int = token->t_int; break;
							case T_DOUBLE: op.right.dtype = TYPE_DOUBLE; op.right.t_double = token->t_double; break;
							case T_STRING: op.right.dtype = TYPE_STRING; strcpy(op.right.t_string, token->t_string); break;
							default: return ERROR_INTER;
						}
						op.right.result = -1;
					}
					generate_op(op);
			}else{
				return SYNTAX_ERROR;
			}
			return OK_E;
		}else{
			return SYNTAX_ERROR;
		}
	}
	return SYNTAX_ERROR;
}

/**
 * Kontrola volani funkce
 * @param trida funkce
 * @param jmeno funkce
 * @return OK_E bez chyby
 **/
int functionCall(char* class, char* name){
//	printfD("functionCall\n");
	int varPPcount = 0;
	char vars[20][20];
	char varClass[20][20];
	memset(varClass, 0, sizeof(char) * 20 * 20);
	TokType varOp[20];
	//parametry volani
	tParamStack params;
	initParams(&params);
	do{
		if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
		tParam param;
		memset(&param, 0, sizeof(tParam));
		if(token->type == RIGHT_BRACKET){ //<id>()
//			printfDc("/functionCall\n");
			insert_call((class ? class : curClass), name, &params);
			return OK_E;
		}else if(token->type == PLUS_PLUS || token->type == MINUS_MINUS){ //++ -- v parametru
			tOperation op;
			if(token->type == PLUS_PLUS){
				op.operator.tokType = (TokType)CG_INC;
			}else if(token->type == MINUS_MINUS){
				op.operator.tokType = (TokType)CG_DEC;
			}
			if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
			if(token->type == IDENTIFIER || token->type == IDENTIFIER_2){ //promenna
				op.left.tokType = IDENTIFIER;
				strcpy(param.name, token->identifier_name);
				strcpy(op.left.identifier_name, token->identifier_name);
				if(token->type == IDENTIFIER_2){
					char* tmp = malloc(sizeof(char) * strlen(token->identifier_name));
					strcpy(tmp, token->identifier_name);
					unsigned int pos = strstr(tmp, ".") - tmp;
					strncpy(param.data.tString, tmp, pos);
					strncpy(param.name, tmp + pos + 1, (strlen(tmp) - pos));
					strncpy(op.left.t_string, tmp, pos);
					strncpy(op.left.identifier_name, tmp + pos + 1, (strlen(tmp) - pos));
					free(tmp);
				}
			}
			op.left.result = -1;
			generate_op(op);
		}else if(token->type == T_INT || token->type == T_DOUBLE || token->type == T_STRING){ //konstanta
			switch(token->type){
				case T_INT: param.dType = TYPE_INT; param.data.tInt = token->t_int; break;
				case T_DOUBLE: param.dType = TYPE_DOUBLE; param.data.tDouble = token->t_double; break;
				case T_STRING: param.dType = TYPE_STRING; strcpy(param.data.tString, token->t_string); break;
				default: return ERROR_INTER;
			}
		}else if(token->type == IDENTIFIER || token->type == IDENTIFIER_2){ //promenna
			strcpy(param.name, token->identifier_name);
			strcpy(param.data.tString, "");
			if(token->type == IDENTIFIER_2){
				char* tmp = malloc(sizeof(char) * strlen(token->identifier_name));
				strcpy(tmp, token->identifier_name);
				unsigned int pos = strstr(tmp, ".") - tmp;
				strncpy(param.data.tString, tmp, pos);
				strncpy(param.name, tmp + pos + 1, (strlen(tmp) - pos));
				free(tmp);
			}
			if(GetNewToken(source, token, true) == LEX_ERROR) return LEX_ERROR;
			if(token->type == PLUS_PLUS || token->type == MINUS_MINUS){
				strcpy(vars[varPPcount], param.name);
				strcpy(varClass[varPPcount], param.data.tString);
				if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
				if(token->type == PLUS_PLUS){
					varOp[varPPcount] = (TokType)CG_INC;
				}else if(token->type == MINUS_MINUS){
					varOp[varPPcount] = (TokType)CG_DEC;
				}
			}
		}else{
			return SYNTAX_ERROR;
		}
		varPPcount++;
		pushParam(&params, param);
		if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
	}while(token->type == COMMA); //vice parametru
	if(token->type == RIGHT_BRACKET){ //<id>(...)
		insert_call((class ? class : curClass), name, &params);
		if(varPPcount){
			for(int i = 0; i < varPPcount; i++){
				if(varOp[i] == 2 || varOp[i] == 4){
					tOperation op;
					op.left.result = -1;
					op.operator.tokType = varOp[i];
					op.left.tokType = IDENTIFIER;
					strcpy(op.left.identifier_name, vars[i]);
					strcpy(op.left.t_string, varClass[i]);
					generate_op(op);
				}
			}
		}
//		printfDc("/functionCall\n");
		return OK_E;
	}
	return SYNTAX_ERROR;
}

/**
 * Kontrola if else
 * @return OK_E bez chyby
 **/
int checkIf(){
//	printfD("checkIf\n");
	int ret = 0;
	if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
	if(token->type == LEFT_BRACKET){ //if(
		exp_status e = parse_expr(*token);
		if(e.b != BRACKET){
			return SYNTAX_ERROR;
		}
		tOperation op;
		op.operator.tokType = (TokType)CG_IF;
		if(e.token.result != -1){
			op.left.dtype = (tDatatype)BOOL_TYPE;
			op.left.result = e.token.result;
		}else{
			if(e.token.tokType == IDENTIFIER){
				op.left.tokType = IDENTIFIER;
				strcpy(op.left.identifier_name, e.token.identifier_name);
				strcpy(op.left.t_string, e.token.t_string);
			}else{
				switch(e.token.tokType){
					case T_INT:{ 
						op.left.dtype = TYPE_INT; 
						op.left.t_int = e.token.t_int; 
						if(op.left.t_int != 0 && op.left.t_int != 1){
							return SEMANTIC_ERROR;
						}
						break;
					}
					case T_DOUBLE:
					case T_STRING: return SEMANTIC_ERROR;
					default: return ERROR_INTER;
				}
				op.left.result = -1;
			}
		}
		generate_op(op);
		ret = blockBody();
		if(ret != 0){
			return ret;
		}
		tOperation op1;
		op1.operator.tokType = (TokType)CG_ELSE;
		op1.left.result = -1;
		generate_op(op1);
		if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
		if(token->type == ELSE){ //if(...){...}else
			if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
			if(token->type == LEFT_DOUBLE_BRACKET){ //if(...){...}else{
				
				ret = blockBody();//if(...){...}else{...}
				if(ret != 0){
					return ret;
				}
				tOperation op2;
				op2.operator.tokType = (TokType)CG_ENDIF;
				op2.left.result = -1;
				generate_op(op2);
//				printfDc("/checkIf\n");
				return OK_E;
			}
		}
	}
	return SYNTAX_ERROR;
}

/**
 * Kontrola while
 * @return OK_E bez chyby
 **/
int checkWhile(){ 
//	printfD("checkWhile\n");
	if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
	if(token->type == LEFT_BRACKET){ //while(
		tOperation opStart;
		opStart.operator.tokType = (TokType)CG_LOOP_START;
		opStart.left.result = -1;
		generate_op(opStart); 
		exp_status e = parse_expr(*token);
		if(e.b != BRACKET){
			return SYNTAX_ERROR;
		}
		tOperation op;
		op.operator.tokType = (TokType)CG_LOOP;
		op.right.result = -1;
		if(e.token.result != -1){
			op.left.dtype = (tDatatype)BOOL_TYPE;
			op.left.result = e.token.result;
		}else{
			if(e.token.tokType == IDENTIFIER){
				op.left.tokType = IDENTIFIER;
				strcpy(op.left.identifier_name, e.token.identifier_name);
				strcpy(op.left.t_string, e.token.t_string);
			}else{
				switch(e.token.tokType){
					case T_INT:{ 
						op.left.dtype = TYPE_INT; 
						op.left.t_int = e.token.t_int; 
						if(op.left.t_int != 0 && op.left.t_int != 1){
							return SEMANTIC_ERROR;
						}
						break;
					}
					case T_DOUBLE:
					case T_STRING: return SEMANTIC_ERROR;
					default: return ERROR_INTER;
				}
				op.left.result = -1;
			}
		}
		generate_op(op);
		int ret = blockBody();//while(...){...}
		if(ret != OK_E){
			return ret;
		}
		tOperation op1;
		op1.left.result = -1;
		op1.operator.tokType = (TokType)CG_ENDLOOP;
		generate_op(op1);
	}
//	printfDc("/checkWhile\n");
	return OK_E;
}

/**
 * Kontrola funkcĂ­ z ifj16
 * @param typ keyword
 * @return OK_E bez chyby
 **/
int checkPKeyword(TokType type, int* returnId){
//	printfD("checkKeyword\n");
	if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
	if(token->type != LEFT_BRACKET){
		return SYNTAX_ERROR;
	}
	char* name = malloc(sizeof(char) * 12);
	tParamStack params;
	initParams(&params);
	switch(type){
		case READINT: {
			strcpy(name, "readInt"); 
			break;
		}
		case READDOUBLE: {
			strcpy(name, "readDouble"); 
			break;
		}
		case READSTRING: {
			strcpy(name, "readString"); 
			break;
		}
		case LENGTH: {
			strcpy(name, "length");
			if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
			tParam param;
			if(token->type == T_STRING){
				strcpy(param.name, "");
				param.dType = TYPE_STRING; 
				strcpy(param.data.tString, token->t_string);
			}else if(token->type == IDENTIFIER || token->type == IDENTIFIER_2){
				strcpy(param.name, token->identifier_name);
				if(token->type == IDENTIFIER_2){
					char* tmp = malloc(sizeof(char) * strlen(token->identifier_name));
					strcpy(tmp, token->identifier_name);
					unsigned int pos = strstr(tmp, ".") - tmp;
					strncpy(param.data.tString, tmp, pos);
					strncpy(param.name, tmp + pos + 1, (strlen(tmp) - pos));
					free(tmp);
				}
			}else{
				return SEMANTIC_TYPE_ERROR;
			}
			pushParam(&params, param);
			break;
		}
		case SUBSTR:{
			strcpy(name, "substr");
			for(int i = 0; i < 3; ++i){
				if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
				tParam param;
				memset(&param, 0, sizeof(tParam));
				if(token->type == T_STRING && i == 0){
					strcpy(param.name, "");
					param.dType = TYPE_STRING; 
					strcpy(param.data.tString, token->t_string);
				}else if(token->type == T_INT && i != 0){
					strcpy(param.name, "");
					param.dType = TYPE_INT; 
					param.data.tInt = token->t_int;
				}else if(token->type == IDENTIFIER || token->type == IDENTIFIER_2){
					strcpy(param.name, token->identifier_name);
					if(token->type == IDENTIFIER_2){
						char* tmp = malloc(sizeof(char) * strlen(token->identifier_name));
						strcpy(tmp, token->identifier_name);
						unsigned int pos = strstr(tmp, ".") - tmp;
						strncpy(param.data.tString, tmp, pos);
						strncpy(param.name, tmp + pos + 1, (strlen(tmp) - pos));
						free(tmp);
					}
				}else{
					return SEMANTIC_TYPE_ERROR;
				}
				if(i < 2){
					if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
					if(token->type != COMMA){
						return SYNTAX_ERROR;
					}
				}
				pushParam(&params, param);
			}
			break;
		}
		case COMPARE:{
			strcpy(name, "compare");
			for(int i = 0; i < 2; ++i){
				if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
				tParam param;
				memset(&param, 0, sizeof(tParam));
				if(token->type == T_STRING){
					strcpy(param.name, "");
					param.dType = TYPE_STRING; 
					strcpy(param.data.tString, token->t_string);
				}else if(token->type == IDENTIFIER || token->type == IDENTIFIER_2){
					strcpy(param.name, token->identifier_name);
					if(token->type == IDENTIFIER_2){
						char* tmp = malloc(sizeof(char) * strlen(token->identifier_name));
						strcpy(tmp, token->identifier_name);
						unsigned int pos = strstr(tmp, ".") - tmp;
						strncpy(param.data.tString, tmp, pos);
						strncpy(param.name, tmp + pos + 1, (strlen(tmp) - pos));
						free(tmp);
					}
				}else{
					return SEMANTIC_TYPE_ERROR;
				}
				if(i < 1){
					if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
					if(token->type != COMMA){
						return SYNTAX_ERROR;
					}
				}
				pushParam(&params, param);
			}
			break;
		}
		case FIND:{
			strcpy(name, "find");
			for(int i = 0; i < 2; ++i){
				if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
				tParam param;
				memset(&param, 0, sizeof(tParam));
				if(token->type == T_STRING){
					strcpy(param.name, "");
					param.dType = TYPE_STRING; 
					strcpy(param.data.tString, token->t_string);
				}else if(token->type == IDENTIFIER || token->type == IDENTIFIER_2){
					strcpy(param.name, token->identifier_name);
					if(token->type == IDENTIFIER_2){
						char* tmp = malloc(sizeof(char) * strlen(token->identifier_name));
						strcpy(tmp, token->identifier_name);
						unsigned int pos = strstr(tmp, ".") - tmp;
						strncpy(param.data.tString, tmp, pos);
						strncpy(param.name, tmp + pos + 1, (strlen(tmp) - pos));
						free(tmp);
					}
				}else{
					return SEMANTIC_TYPE_ERROR;
				}
				if(i < 1){
					if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
					if(token->type != COMMA){
						return SYNTAX_ERROR;
					}
				}
				pushParam(&params, param);
			}
			break;
		}
		case SORT:{
			strcpy(name, "sort");
			if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
				tParam param;
				memset(&param, 0, sizeof(tParam));
				if(token->type == T_STRING){
					strcpy(param.name, "");
					param.dType = TYPE_STRING; 
					strcpy(param.data.tString, token->t_string);
				}else if(token->type == IDENTIFIER || token->type == IDENTIFIER_2){
					strcpy(param.name, token->identifier_name);
					if(token->type == IDENTIFIER_2){
						char* tmp = malloc(sizeof(char) * strlen(token->identifier_name));
						strcpy(tmp, token->identifier_name);
						unsigned int pos = strstr(tmp, ".") - tmp;
						strncpy(param.data.tString, tmp, pos);
						strncpy(param.name, tmp + pos + 1, (strlen(tmp) - pos));
						free(tmp);
					}
				}else{
					return SEMANTIC_TYPE_ERROR;
				}
				pushParam(&params, param);
			break;
		}
		default: {
			free(name); 
			return ERROR_INTER;
		}
	}
	if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
	if(token->type != RIGHT_BRACKET){
		free(name);
		return SYNTAX_ERROR;
	}
	if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
	*returnId = insert_call("ifj16", name, &params);
	if(token->type != SEMICOLON){
		free(name);
		return SYNTAX_ERROR;
	}
	free(name);
//	printfDc("/checkKeyword\n");
	return OK_E;
}

/**
 * Kontrola ifj16.print
 * @return OK_E bez chyby
 **/
int checkPrint(){
//	printfD("checkPrint\n");
	int varPPcount = 0;
	char vars[20][20];
	char varClass[20][20];
	memset(varClass, 0, sizeof(char) * 20 * 20);
	TokType varOp[20];
	if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
	if(token->type != LEFT_BRACKET){
		return SYNTAX_ERROR;
	}
	do{
		if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
		tParamStack params;
		initParams(&params);
		tParam param;
		if(token->type == T_STRING || token->type == T_INT || token->type == T_INT){
			switch(token->type){
				case T_INT: param.dType = TYPE_INT; param.data.tInt = token->t_int; break;
				case T_DOUBLE: param.dType = TYPE_DOUBLE; param.data.tDouble = token->t_double; break;
				case T_STRING: param.dType = TYPE_STRING;  strcpy(param.data.tString, token->t_string); break;
				default: return ERROR_INTER;
			}
			strcpy(param.name, "");
		}else if(token->type == PLUS_PLUS || token->type == MINUS_MINUS){
			tOperation op;
			if(token->type == PLUS_PLUS){
				op.operator.tokType = (TokType)CG_INC;
			}else if(token->type == MINUS_MINUS){
				op.operator.tokType = (TokType)CG_DEC;
			}
			op.left.tokType = IDENTIFIER;
			if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
			if(token->type == IDENTIFIER || token->type == IDENTIFIER_2){ //promenna
				op.left.tokType = IDENTIFIER;
				strcpy(param.name, token->identifier_name);
				strcpy(op.left.identifier_name, token->identifier_name);
				if(token->type == IDENTIFIER_2){
					char* tmp = malloc(sizeof(char) * strlen(token->identifier_name));
					strcpy(tmp, token->identifier_name);
					unsigned int pos = strstr(tmp, ".") - tmp;
					char* class = malloc(sizeof(char) * strlen(token->identifier_name));
					strncpy(class, tmp, pos);
					class[pos] = '\0';
					strcpy(param.data.tString, class);
					strncpy(param.name, tmp + pos + 1, (strlen(tmp) - pos));
					strcpy(op.left.t_string, class);
					strncpy(op.left.identifier_name, tmp + pos + 1, (strlen(tmp) - pos));
					free(tmp);
				}

			}
			op.left.result = -1;
			op.right.result = -1;
			generate_op(op);
		}else if(token->type == IDENTIFIER || token->type == IDENTIFIER_2){
			strcpy(param.name, token->identifier_name);
			strcpy(param.data.tString, "");
			if(token->type == IDENTIFIER_2){
				char* tmp = malloc(sizeof(char) * strlen(token->identifier_name));
				strcpy(tmp, token->identifier_name);
				unsigned int pos = strstr(tmp, ".") - tmp;
				char* class = malloc(sizeof(char) * strlen(token->identifier_name));
				strncpy(class, tmp, pos);
				class[pos] = '\0';
				strcpy(param.data.tString, class);
				strncpy(param.data.tString, tmp, pos);
				strncpy(param.name, tmp + pos + 1, (strlen(tmp) - pos));
				free(tmp);
			}
			if(GetNewToken(source, token, true) == LEX_ERROR) return LEX_ERROR;
			if(token->type == PLUS_PLUS || token->type == MINUS_MINUS){
				strcpy(vars[varPPcount], param.name);
				strcpy(varClass[varPPcount], param.data.tString);
				if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
				if(token->type == PLUS_PLUS){
					varOp[varPPcount] = (TokType)CG_INC;
				}else if(token->type == MINUS_MINUS){
					varOp[varPPcount] = (TokType)CG_DEC;
				}
			}
		}else{
			return SEMANTIC_TYPE_ERROR;
		}
		pushParam(&params, param);
		insert_call("ifj16", "print", &params);
		if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
		varPPcount++;
	}while(token->type == PLUS);
	if(token->type != RIGHT_BRACKET){
		return SYNTAX_ERROR;
	}
	if(GetNewToken(source, token, false) == LEX_ERROR)return LEX_ERROR;
	if(token->type != SEMICOLON){
		return SYNTAX_ERROR;
	}
	if(varPPcount){
		for(int i = 0; i < varPPcount; i++){
			if(varOp[i] == 4 || varOp[i] == 2){
				tOperation op;
				op.operator.tokType = varOp[i];
				op.left.tokType = IDENTIFIER;
				op.left.result = -1;
				strcpy(op.left.identifier_name, vars[i]);
				strcpy(op.left.t_string, varClass[i]);
				generate_op(op);
			}
		}
	}
//	printfDc("/checkPrint\n");
	return OK_E;
}
