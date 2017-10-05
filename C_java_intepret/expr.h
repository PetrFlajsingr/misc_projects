#ifndef EXPR_H   /* Include guard */
#define EXPR_H

#include "interfaces.h"
#include "etok.h"


//struct to hold individual ETok_T tokens and possible errors

typedef enum {
	OK, SEMI, BRACKET, ERR
} backtrack;

typedef struct {
	char class_name[5000];
	char identifier_name[5000];
	bool split;
} qualifiedNames;

typedef struct {
	ETok_T token;
	ERROR e;
	backtrack b;
} exp_status;

//stack typedefs

typedef struct e_tElem {
	struct e_tElem *left;
	ETok_T t;
} *e_tElemPtr;

typedef struct {
	e_tElemPtr top;
} e_tStack;

//precedence table typedefs

typedef enum {
	S_EMPTY, S_SHIFT, S_PROC, S_PAR, S_ERROR
} stack_rules;

typedef enum {
	PR_3, PR_4, PR_6, PR_7, PR_LP, PR_RP, PR_i, PR_END, PR_ERR
} pr_tokens;

//stack declarations

ERROR e_push(e_tStack *stack_ptr, ETok_T t);
ETok_T e_pop(e_tStack *stack_ptr);
void e_disposeStack(e_tStack *stack_ptr);
bool e_isEmpty(e_tStack *stack_ptr);
ERROR e_initStack(e_tStack **stack_ptr);

//expression parser declarations

pr_tokens expr_top_token(e_tStack *expr_stack);
pr_tokens input_token();
stack_rules check_rule(e_tStack *expr_stack);
exp_status shift_token(e_tStack *expr_stack);
exp_status process(e_tStack *expr_stack);
exp_status parse_expr(Tok_T previous);

//function to prepare data to be sent to code generator
tOperation prep_op(ETok_T left, ETok_T right, ETok_T operator);

qualifiedNames handleQualifiedName (char *name);


exp_status getLexeme(bool next);
backtrack scanExpInput(FILE *source, Tok_T previous);


#endif // EXPR_H
