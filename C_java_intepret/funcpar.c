#include "funcpar.h"
#include <stdlib.h>
#include <string.h>

void initParams(tParamStack *params){
	params->params = malloc(sizeof(tParam)*MAX_PARAM_SIZE);
	params->size = 0;
	params->maxSize = MAX_PARAM_SIZE;
}

void pushParam(tParamStack *params, tParam elem){
	if(params->size == params->maxSize){
		params->params = realloc(params->params, sizeof(tParam)*(params->maxSize+MAX_PARAM_SIZE));
	}
	params->params[params->size++] = elem;		
}

//void popParams(tParamStack *params, tParam *elem){


tParam* searchParam(tParamStack *params, char* name){
	for(int i = 0; i <= params->size; i++){
		if(strcmp(params->params[i].name, name) == 0){
			return &params->params[i];
		}
	}
	return NULL;
}

void destroyParams(tParamStack *params){
	free(params->params);
	params->size = 0;
	params->maxSize = 0;
}