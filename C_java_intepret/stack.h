/**
 * @file stack.h
 * @author Igor Frank(xfrank12)
 * @date 4 october 2016
 * @brief Sobor obsahujici zdrojovy kod pro zasobnik
**/

#ifndef H_STACK
#define H_STACK

#define MIN_LENGHT 16

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
	void* elems;
	int size_elems;
	int alloc_size;
	int lenght;
} tStack;

int initStack(tStack* s, int size_elems);
void freeStack(tStack* s);
void pushStack(tStack* s, void* data);
void popStack(tStack* s, void* data);
void topStack(tStack* s, void* data);
bool isEmptyStack(tStack* s);
void popLastStack(tStack* s, void* data);




#endif
