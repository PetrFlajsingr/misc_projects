#pragma once

#include "datatypes.h"
#include <stdbool.h>

#define MAX_PARAM_SIZE 3
//int size = 0;
//int maxSize = 0;
//struktura reprezentujici parametr fce
typedef struct{
	char name[1024];
	char class[1024];
	bool init;
	tDatatype dType;
	union{
		int tInt;
		double tDouble;
		char tString[1024];
	}data;
}tParam;

typedef struct{
	tParam *params;
	int size;
	int maxSize;
}tParamStack;

void initParams(tParamStack *params);
void pushParam(tParamStack *params, tParam elem);
//void popParam(tParamStack *params, tParam *elem);
tParam* searchParam(tParamStack *params, char* name);
void destroyParams(tParamStack *params);
