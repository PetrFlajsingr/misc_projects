/**
 * @file stack.c
 * @author Igor Frank(xfrank12)
 * @date 4 october 2016
 * @brief Soubor obsahujici definici funkci pro praci se zasobnikem
**/


#include "stack.h"

int initStack(tStack* s, int size_elems){
	s->elems = malloc(size_elems * MIN_LENGHT);
	if(s->elems == NULL)
	       return -1;
	s->size_elems = size_elems;
	s->alloc_size = MIN_LENGHT;
	s->lenght = 0;	
	return 0;
}

void freeStack(tStack* s){
	free(s->elems);
}

void* currentSize(tStack* s){
	return ((size_t*)s->elems + s->lenght * s->size_elems);
}

void pushStack(tStack* s, void* data){
	if(s->alloc_size == s->lenght){
		void* tmp = NULL;
		tmp = realloc(s->elems, 2 * s->size_elems * s->alloc_size);
	
		if(tmp == NULL)
			return;
	}
		memcpy(currentSize(s), data, s->size_elems);
		s->lenght++;
}

void popStack(tStack* s, void* data){
	s->lenght--;
	memcpy((void*)data, currentSize(s), s->size_elems);
}

void topStack(tStack *s, void* data){
	memcpy((void*)data, currentSize(s), s->size_elems);
}

bool isEmptyStack(tStack* s){
	if(s->lenght == 0)
		return true;
	return false;
}

void* lastAddress(tStack* s){
	return ((size_t*)s->elems + s->alloc_size * s->size_elems - s->size_elems);
}	

void popLastStack(tStack* s, void* data){
	memcpy((void*)data, s->elems, s->size_elems);
	memmove(s->elems, (size_t*)s->elems + s->size_elems, (s->size_elems * (s->lenght - 1)));
	s->lenght--;
}
