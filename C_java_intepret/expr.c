#include "expr.h"
#include "etok.h"
#include "workaround.h"

// int inputCounter = 0;
// int maxInput = 0;
// exp_status inputArr[100];//shitty workaround, increase size as needed for testing
// FILE *source;

ERROR e_initStack(e_tStack **stack_ptr){
	*stack_ptr = malloc(sizeof(e_tStack));
	if(*stack_ptr != NULL){
        (*stack_ptr)->top = NULL;
		return E_OK;
	}
	else{
		return E_MEM;
	}
}

bool e_isEmpty(e_tStack *stack_ptr){
	if(stack_ptr->top == NULL){
		return true;
	}
	else{
		return false;
	}
}

void e_disposeStack(e_tStack *stack_ptr){
	while(e_isEmpty(stack_ptr) != true){
		//free(stack_ptr->top->t);
		e_tElemPtr temp = stack_ptr->top;
		stack_ptr->top = temp->left;
		free(temp);
	}
	free(stack_ptr);
}

ERROR e_push(e_tStack *stack_ptr, ETok_T t){
	e_tElemPtr temp = malloc(sizeof(struct e_tElem));
	if(temp != NULL){
		temp->left = stack_ptr->top;
		temp->t = t;
		stack_ptr->top = temp;
		return E_OK;
	}
	else{
		return E_MEM;
	}
}

ETok_T e_pop(e_tStack *stack_ptr){
	e_tElemPtr temp = stack_ptr->top;
	stack_ptr->top = temp->left;
	ETok_T t = temp->t;
	free(temp);
	return t;
}

//precedence table
stack_rules rule_table[8][8] = {{S_PROC, S_PROC, S_PROC, S_PROC, S_SHIFT, S_PROC, S_SHIFT, S_PROC},
							{S_SHIFT, S_PROC, S_PROC, S_PROC, S_SHIFT, S_PROC, S_SHIFT, S_PROC},
							{S_SHIFT, S_SHIFT, S_PROC, S_PROC, S_SHIFT, S_PROC, S_SHIFT, S_PROC},
							{S_SHIFT, S_SHIFT, S_SHIFT, S_PROC, S_SHIFT, S_PROC, S_SHIFT, S_PROC},
							{S_SHIFT, S_SHIFT, S_SHIFT, S_SHIFT, S_SHIFT, S_PAR, S_SHIFT, S_ERROR},
							{S_PROC, S_PROC, S_PROC, S_PROC, S_ERROR, S_PROC, S_ERROR, S_PROC},
							{S_PROC, S_PROC, S_PROC, S_PROC, S_ERROR, S_PROC, S_ERROR, S_PROC},
							{S_SHIFT, S_SHIFT, S_SHIFT, S_SHIFT, S_SHIFT, S_ERROR, S_SHIFT, S_EMPTY}};


pr_tokens expr_top_token(e_tStack *expr_stack){
	//this works if the stack uses linked list
	//change if stack implemented in another way
    //if(expr_stack == NULL)
        //printf("expr_stack is NULL\n");
	e_tElemPtr current = expr_stack->top;
	while(current != NULL){
		//printf("%s\t%d\n", "going through token stack:", current->t.tokType);
		switch(current->t.tokType){
	 		case T_INT:
	 		case T_DOUBLE:
	 		case T_STRING:
	 		case IDENTIFIER:
	 			current = current->left;
	 			break;
	 		case LEFT_BRACKET:
	 			return PR_LP;
	 			break;
	 		case RIGHT_BRACKET:
	 			return PR_RP;
	 			break;
	 		case MUL:
	 		case DIV:
	 			if(current->t.result != -1){
	 				current = current->left;
	 			}
	 			else{
	 				return PR_3;	
	 			}
	 			break;
	 		case PLUS:
	 		case MINUS:
	 			if(current->t.result != -1){
	 				current = current->left;
	 			}
	 			else{
	 				return PR_4;	
	 			}
	 			break;
	 		case LESS:
	 		case LESS_EQUAL:
	 		case GREATER:
	 		case GREATER_EQUAL:
	 			if(current->t.result != -1){
	 				current = current->left;
	 			}
	 			else{
	 				return PR_6;	
	 			}
	 			break;
	 		case EQUALS:
	 		case NOT_EQUAL:
	 			if(current->t.result != -1){
	 				current = current->left;
	 			}
	 			else{
	 				return PR_7;	
	 			}
	 			break;
	 		default:
	 			//unexpected symbol on expression stack, E_99, shouldnt occur
	 			return PR_ERR;
	 			break;
		}
	}
	return PR_END; //reached the bottom of the stack		
}

pr_tokens input_token(){
	exp_status s = getLexeme(true);
	if (s.e != E_OK) {
		//printf("segfault test: getLexeme returning token with error\n");
		return PR_ERR;
	}
	ETok_T nextToken = s.token;
 	switch(nextToken.tokType){
 		case T_INT:
 		case T_DOUBLE:
 		case T_STRING:
 		case IDENTIFIER:
 		case IDENTIFIER_2:
 			return PR_i;
 		case LEFT_BRACKET:
 			return PR_LP;
 		case RIGHT_BRACKET:
 			return PR_RP;
 		case MUL:
 		case DIV:
 			return PR_3;
 		case PLUS:
 		case MINUS:
 			return PR_4;
 		case LESS:
 		case LESS_EQUAL:
 		case GREATER:
 		case GREATER_EQUAL:
 			return PR_6;
 		case EQUALS:
 		case NOT_EQUAL:
 			return PR_7;
 		// case EOF_T:
 		// 	return PR_ERR;
 		default:
 			return PR_END;
 	}
}

stack_rules check_rule(e_tStack *expr_stack){
	////printf("segfault test: check rule before input_token\n");
	pr_tokens i = input_token();
	if (i == PR_ERR) {
		return S_ERROR;
	}
	////printf("segfault test: check_rule after input_token\n");
	pr_tokens e = expr_top_token(expr_stack);
	if (e == PR_ERR) {
		return S_ERROR;
	}
	////printf("segfault test: check_rule after expr_top_token\n");
	if (i == PR_LP) {
		////printf("PR_LP\n");
		if (expr_stack != NULL) {
			if (expr_stack->top != NULL) {
				if ((expr_stack->top->t.tokType == IDENTIFIER) || (expr_stack->top->t.tokType == T_INT) || 
					(expr_stack->top->t.tokType == T_STRING) || (expr_stack->top->t.tokType == T_DOUBLE) || 
					(expr_stack->top->t.result != -1)) {
					return S_ERROR;
				}
			}
		}
	}
	return rule_table[e][i];
}

//wrapper for getting new lexemes and error propagation
exp_status getLexeme(bool next) {
	if (!next) {
        if (inputCounter < maxInput) {
            //printf("%s\t%d\n", "token to get from input is:", inputArr[inputCounter].token.tokType);
            return inputArr[inputCounter++];
        }
    }
    else {
    	if (inputCounter < maxInput) {
    		//printf("%s\t%d\n", "token to show from input is:", inputArr[inputCounter].token.tokType);
    		return inputArr[inputCounter];
    	}
    }
}

//workaround for broken GetNewToken
backtrack scanExpInput(FILE *source, Tok_T previous) {
	inputCounter = 0;
	maxInput = 0;
	memset(inputArr, 0, sizeof(inputArr));
	Tok_T tokken;
	int i = 0;
	int err = 0;
	
	//handling previous token
	exp_status p;
	ETok_T prev;
	strcpy(prev.syntax_character, previous.syntax_character);
 	strcpy(prev.t_string, previous.t_string);
 	strcpy(prev.identifier_name, previous.identifier_name);
 	prev.t_double = previous.t_double;
 	prev.t_int = previous.t_int;
 	prev.tokType = previous.type;
 	prev.result = -1;
 	p.token = prev;
 	p.e = E_OK;
 	inputArr[i++] = p;
 	maxInput++;
 	if (p.token.tokType == EOF_T) {
    	//printf("EOF in previous token\n");
    	return ERR;
	}
 	if (p.token.tokType == LEFT_DOUBLE_BRACKET)
 		return BRACKET;
 	if (p.token.tokType == SEMICOLON)
 		return SEMI;

 	//printf("%s\t%d\n", "inputArr index is:", i);
	while ((err = GetNewToken(source, &tokken, false)) == 0) {
	 	exp_status s;
	 	ETok_T t;
	 	strcpy(t.syntax_character, tokken.syntax_character);
	 	strcpy(t.t_string, tokken.t_string);
	 	strcpy(t.identifier_name, tokken.identifier_name);
	 	t.t_double = tokken.t_double;
	 	t.t_int = tokken.t_int;
	 	t.tokType = tokken.type;
	 	t.result = -1;
	 	s.token = t;
	 	s.e = E_OK;
	 	inputArr[i++] = s;
	 	maxInput++;
	 	if (s.token.tokType == EOF_T) {
	    	//printf("EOF in expression\n");
	    	return ERR;
		}
	 	if (s.token.tokType == LEFT_DOUBLE_BRACKET)
	 		return BRACKET;
	 	if (s.token.tokType == SEMICOLON)
	 		return SEMI;
	}
	if (err == -1) {
 		//printf("error in lexer\n");
 		return ERR;
	}
	//printf("%s\t%d\n", "inputArr index is:", i);
	return OK;
}

//shift token from input to expression stack
exp_status shift_token(e_tStack *expr_stack){
	exp_status s = getLexeme(false);
	////printf("segfault test: shift token after getlexeme\n");
	if(s.e != E_OK){
		//we can free memory here, if needed
		return s;
	}
	if(s.token.tokType == IDENTIFIER_2) {
		//printf("ID 2\n");
		s.token.tokType = IDENTIFIER;//unify id types. names are split later in this function
	}
	if(s.token.tokType == T_STRING)
		s.token.dtype = TYPE_STRING;
	else if(s.token.tokType == T_INT)
		s.token.dtype = TYPE_INT;
	else if(s.token.tokType == T_DOUBLE)
		s.token.dtype = TYPE_DOUBLE;
	else if(s.token.tokType == IDENTIFIER){
		//split variable name if it is a qualified name (class.var)
		////printf("%s\t%s\n", "segfault test: token id before hqn: ", s.token.identifier_name);
		////printf("%s\t%s\n", "segfault test: token t_string before hqn: ", s.token.t_string);
		qualifiedNames names = handleQualifiedName(s.token.identifier_name);
		if (names.split) {
			//printf("var name was split");
			//DONT SWAP THESE TWO CALLS, OTHERWISE THE SECOND ONE WILL HAVE CHANGED STRLEN RESULT
			strncpy(s.token.t_string, names.class_name, strlen(s.token.identifier_name) + 1);
			strncpy(s.token.identifier_name, names.identifier_name, strlen(s.token.identifier_name) + 1);			
		}
	}
	////printf("%s\t%s\n", "segfault test: token id after hqn: ", s.token.identifier_name);
	////printf("%s\t%s\n", "segfault test: token class after hqn: ", s.token.t_string);
	////printf("segfault test: shift token after hqn\n");
	ERROR e = e_push(expr_stack, s.token);//paranoia error checking, stack probably wont break
	if (e != E_OK) {
		s.e = e;
		return s;
	}
	////printf("segfault test: shift token after push\n");
	return s;
}

//pop tokens from stack and process them
//result represents the operation & is pushed to the stack manually outside the function
exp_status process(e_tStack *expr_stack) {
	exp_status s;
	if (expr_stack == NULL) {
		s.e = E_SYN;
		return s;
	}
	if (expr_stack->top == NULL) {
		s.e = E_SYN;
		return s;
	}
	ETok_T right = e_pop(expr_stack);
	if (expr_stack->top == NULL) {
		s.e = E_SYN;
		return s;
	}
	ETok_T middle = e_pop(expr_stack);
	if (expr_stack->top == NULL) {
		s.e = E_SYN;
		return s;
	}
	ETok_T left = e_pop(expr_stack);
	if ((middle.tokType == IDENTIFIER) || (middle.tokType == T_INT) || (middle.tokType == T_STRING) ||
		(middle.tokType == T_DOUBLE) || (middle.result != -1)) {
		//middle token has to be operator, can't be i or E
		//we can free memory here, if needed
		s.e = E_SYN;
		return s;
	}
	if ((left.tokType != IDENTIFIER) && (left.tokType != T_INT) && (left.tokType != T_STRING) && 
		(left.tokType != T_DOUBLE) && (left.result < 0)) {
		//operands can't be operators
		s.e = E_SYN;
		return s;
	}
	if ((right.tokType != IDENTIFIER) && (right.tokType != T_INT) && (right.tokType != T_STRING) && 
		(right.tokType != T_DOUBLE) && (right.result < 0)) {
		//operands can't be operators
		s.e = E_SYN;
		return s;
	}

	// type controls done in interpret instead

    tOpResultId temp = generate_op(prep_op(left, right, middle));
    if (temp == -1) {
		//error during code generation -> probably memory error TODO CHANGE ERROR TYPE MAYBE
		s.e = E_MEM;
		return s;
	}
	middle.result = temp;
	s.token = middle;
	return s;
}


exp_status parse_expr(Tok_T previous){
	e_tStack* expr_stack;
	exp_status expr;
    expr.e = e_initStack(&expr_stack);
	if(expr.e != E_OK){
		//memory error
		e_disposeStack(expr_stack);
		return expr;
	}
	stack_rules current_rule;
	backtrack b = scanExpInput(source, previous);
	if (b == ERR) {
		expr.b = b;
		expr.e = E_SYN;
		return expr;
	}
    //printf("%s %d\n", "Rule table: Should be 1: ", rule_table[PR_END][PR_LP]);
	while((current_rule = check_rule(expr_stack)) != S_EMPTY){
		//printf("%s: %d\n", "Rule table check: ", current_rule);
		switch(current_rule){
			case S_PROC:
                expr = process(expr_stack);
				if(expr.e != E_OK){
					e_disposeStack(expr_stack);
					return expr;
				}
				e_push(expr_stack, expr.token);
				break;
			case S_SHIFT:
				expr = shift_token(expr_stack);
				////printf("segfault test: shift token after push4\n");
				if(expr.e != E_OK){
					//printf("segfault test: shift token after push3\n");
					e_disposeStack(expr_stack);
					return expr;
				}
				////printf("segfault test: shift token after push2\n");
				break;
			case S_PAR:
				getLexeme(false);
				expr.token = e_pop(expr_stack);
				if(expr.token.tokType != LEFT_BRACKET){
					// (E) -> E
					ETok_T temp = e_pop(expr_stack);
					if(temp.tokType != LEFT_BRACKET){
						expr.e = E_SYN;
						e_disposeStack(expr_stack);
						//we can free memory here, if needed
						return expr;
					}
					e_push(expr_stack, expr.token);
				}
				else{
					// () -> nothing
					expr.e = E_SYN;
					e_disposeStack(expr_stack);
					return expr;
					//we can free memory here, if needed
				}
				break;
			case S_ERROR:
				//missing operator or unbalanced parentheses
				//syntax error 2
				expr.e = E_SYN;
				e_disposeStack(expr_stack);
				return expr;
				break;
		}
	}
	//printf("segfault test: out of while\n");
	//successfully parsed expression

    // eating the expression ending semicolon
    exp_status sem = getLexeme(true);
    if (sem.token.tokType == SEMICOLON) {
        getLexeme(false);
    }

	if(e_isEmpty(expr_stack) != true){
		expr.token = e_pop(expr_stack);
	}
	else{
		//empty expression
		expr.e = E_SYN;
	}
	e_disposeStack(expr_stack);
	expr.b = b;
	//printf("%s\t%d\n", "expr return status.e:", expr.e);
	//printf("%s\t%d\n", "expr return status.b:", expr.b);
	return expr;
}

tOperation prep_op(ETok_T left, ETok_T right, ETok_T operator) {
	tOperation op;
	op.left = left;
	op.right = right;
	op.operator = operator;
	return op;
}

// all parameters have to be allocated
qualifiedNames handleQualifiedName (char *name) {
	//printf("%s\n", "hqn: start");
	qualifiedNames names;
	char var[5000];
	char class[5000];
	char *target = class;
	int j = 0;
	for (int i = 0; i < strlen(name); i++) {
		if (name[i] == '.') {
			target[i] = '\0';
			target = var;
			j = i + 1;
			continue;
		}
		target[i - j] = name[i];
	}
	//printf("%s\n", "hqn: middle");
	if (j > 1) {
		target[strlen(name)] = '\0';
		strncpy(names.class_name, class, strlen(name) + 1);
		strncpy(names.identifier_name, var, strlen(name) + 1);
		names.split = true;
		//printf("%s", "hqn: split");
	}
	else {
		names.split = false;
		//printf("%s\n", "hqn: not split");
	}
	return names;
}