/**
 * @file s_table.c
 * @author Pavel Mica(xmicap02)
 * @date 9 october 2016
 * @brief Soubor obsahujici definici funkci pro praci s binarnim stromem
**/


#include "s_table.h"
#include "code_gen.h"
#include "funcpar.h"
/* inicializace vsech tabulek */
void initTable (tClassNodePtr *RootClassPtr) {

    *RootClassPtr = NULL;  
}

/* vlozi do tabulky novou tridu */ 
void insertClass (tClassNodePtr *RootPtr, char *className) {

     if(*RootPtr == NULL) {
        tClassNodePtr newNode;
        if((newNode = malloc(sizeof(struct tClassNode))) != NULL) {
            if(className != NULL) {
                //vytvoreni klice pro uzel tridy
                //newNode->key = (char*) malloc(strlen(name));
                strcpy(newNode->key, className);
                //nazev tridy
                //newNode->tClass.name = (char*) malloc(strlen(name));
                strcpy(newNode->tClass.name, className);
            }

            newNode->LPtr = NULL;
            newNode->RPtr = NULL;

            *RootPtr = newNode;
        }
      }
      else {
        int position = 0;

        while(1) {
           if(((*RootPtr)->key[position]) == className[position])
               position++;
           else
               break;
        }

        if(className[position] < (*RootPtr)->key[position])
            insertClass(&(*RootPtr)->LPtr, className);
        else {
             if(className[position] > (*RootPtr)->key[position])
                insertClass(&(*RootPtr)->RPtr, className);
             else {
                //nazev tridy
                //(*RootPtr)->tClass.name = malloc(strlen(name));
                strcpy((*RootPtr)->tClass.name, className);
             }
        }
      } 
}


/* vlozi do tabulky novou funkci */ 
void insertFunction (tClassNodePtr *RootPtr, char *className, tSymbolFunction S) {

  
  tClassNodePtr tmp;
    tFunctionNodePtr tmpFunc;
    tmp = searchClass(*RootPtr, className);

    if(tmp != NULL) {
    tFunctionNodePtr newNode;
    if((newNode = malloc(sizeof(struct tFunctionNode))) != NULL) {
      if(S.name != NULL) {
      //vytvoreni klice pro uzel funkce
      //newNode->key = (char*) malloc(strlen(S.name));
      strcpy(newNode->key, S.name);
      //nazev funkce
      //newNode->tFunction.name = (char*) malloc(strlen(S.name));
      strcpy(newNode->tFunction.name, S.name);
      //predani navratoveho typu funkce
      newNode->tFunction.ret_type = S.ret_type;
      //predani parametru funkce
      //newNode->tFunction.params = (tStack*) malloc(sizeof(S.params));
      newNode->tFunction.params.params = malloc(sizeof(tParam)* S.params.maxSize);
      /*for(int i = 0; i <= S.params.size; i++){
	      newNode->tFunction.params.params[i] = S.params.params[i];
      }
      newNode->tFunction.params.maxSize = S.params.size;*/
      newNode->tFunction.params = S.params;  
            init_tape(&newNode->tFunction.code);
      }

        newNode->LPtr = NULL;
        newNode->RPtr = NULL;

       
       
    }   
    if(tmp->function == NULL){
      tmp->function = newNode;
    }
    else{
      tmpFunc = tmp->function;
      
      while(1){
        int position = 0;
        while(1) {
          if((tmpFunc->key[position]) == S.name[position])
            position++;
          else
            break;
        }      

        if(S.name[position] < tmpFunc->key[position]){
            
          if(tmpFunc->LPtr == NULL){
            tmpFunc->LPtr = newNode;
            return;
         }
          else{
            tmpFunc = tmpFunc->LPtr;
          }
        }     
        else if(S.name[position] > tmpFunc->key[position]){
        
          if(tmpFunc->RPtr == NULL){
            tmpFunc->RPtr = newNode;
            return;
          }
          else{
            tmpFunc = tmpFunc->RPtr;
          }
        }
      }
    }

  }

}
/* vlozi do tabulky novou promennou */
void insertVariable (tClassNodePtr *RootPtr, char *className, tSymbolVariable S) {

  
  tClassNodePtr tmp;
  tVariableNodePtr tmpVar;
  tmp = searchClass(*RootPtr, className);
  if(tmp != NULL){
    tmpVar = tmp->variable;

    tVariableNodePtr newNode;
    if((newNode = malloc(sizeof(struct tVariableNode))) != NULL) {
        if(S.name != NULL) {
           //vytvoreni klice pro uzel promenne
           //newNode->key = (char*) malloc(strlen(S.name));
           strcpy(newNode->key, S.name);
           //nazev promenne
           //newNode->tVariable.name = (char*) malloc(strlen(S.name));
           strcpy(newNode->tVariable.name, S.name);
           //predani datoveho typu 
           newNode->tVariable.type = S.type;
           //predani hodnoty
           newNode->tVariable.content = (tContentType*) malloc(sizeof(S.content));
           newNode->tVariable.content = S.content;
           //isSet
           newNode->tVariable.isSet = S.isSet;
           //isConst
           newNode->tVariable.isConst = S.isConst;
	   newNode->tVariable.activated = S.activated;
/*
           if(S.longName != NULL) {  
        //class.variable
        newNode->tVariable.longName = (char*) malloc(strlen(S.longName));
        strcpy(newNode->tVariable.longName, S.longName);
           }
*/
           newNode->LPtr = NULL;
           newNode->RPtr = NULL;
     
        }
    }
    if(tmpVar == NULL){
      tmp->variable = newNode;
      return;
    }
    else{
      while(1){
        int position = 0;

        while(1) {
           if((tmpVar->key[position]) == S.name[position])
               position++;
           else
               break;
        }

        if(S.name[position] < tmpVar->key[position]){
          if(tmpVar->LPtr == NULL){
            tmpVar->LPtr = newNode;
            return;
          }
          else{
            tmpVar = tmpVar->LPtr;
          }
        }
        else if(S.name[position] > tmpVar->key[position]){
          if(tmpVar->RPtr == NULL){
            tmpVar->RPtr = newNode;
            return;
          }
          else{
            tmpVar = tmpVar->RPtr;
          }
        }
      }
        }
      }
}              


/* vlozi do tabulky novou promennou funkce */ 
void insertFuncVariable (tClassNodePtr *RootPtr, char *className, char *funcName, tSymbolVariable S) {

  
  tFunctionNodePtr tmp;
  tVariableNodePtr tmpVar;
  tmp = searchFunction(*RootPtr, NULL, funcName, className);
  tmpVar = tmp->funcVariable;

  if(tmp != NULL){

           tVariableNodePtr newNode;
	 //  if(newNode != NULL)
	//	free(newNode);
           if((newNode = malloc(sizeof(struct tVariableNode))) != NULL) {
               if(S.name != NULL) {
                  //vytvoreni klice pro uzel promenne
                  //newNode->key = (char*) malloc(strlen(S.name));
                  strcpy(newNode->key, S.name);
                  //nazev promenne
                  //newNode->tVariable.name = (char*) malloc(strlen(S.name));
                  strcpy(newNode->tVariable.name, S.name);
                  //predani datoveho typu
                  newNode->tVariable.type = S.type;
                  //predani hodnoty
                  newNode->tVariable.content = (tContentType*) malloc(sizeof(S.content));
		  //newNode->tVariable.content->string_var = malloc(1024);
                  newNode->tVariable.content = S.content;
                  //isSet
                  newNode->tVariable.isSet = S.isSet;
                  //isConst
                  newNode->tVariable.isConst = S.isConst;
		  newNode->tVariable.activated = S.activated;
                 /* if(S.longName != NULL) {
                     //class.variable
                     newNode->tVariable.longName = (char*) malloc(strlen(S.longName));
                     strcpy(newNode->tVariable.longName, S.longName);
                  }
*/
                  newNode->LPtr = NULL;
                  newNode->RPtr = NULL;
 
            } 
         }
      
         if(tmp->funcVariable == NULL){
           tmp->funcVariable = newNode;
           return;
         }
         else {
    while(1){       
       int position = 0;

       while(1) {
          if((tmpVar->key[position]) == S.name[position])
        position++;
          else
        break;
       }

       if(S.name[position] < tmpVar->key[position]){
          if(tmpVar->LPtr == NULL){
             tmpVar->LPtr = newNode;
             return;
          }
          else{
          tmpVar = tmpVar->LPtr;
          }
       }
       else if(S.name[position] > tmpVar->key[position]){
         if(tmpVar->RPtr == NULL){
        tmpVar->RPtr = newNode;
        return;
         }
         else{
        tmpVar = tmpVar->RPtr;
         }
       }
    }

       }
   }

}


/* vyhledavani tridy pomoci nazvu */
tClassNodePtr searchClass (tClassNodePtr RootPtr, char *name) {

    int position = 0;

    if(RootPtr != NULL) {
       if(strcmp(name, RootPtr->key) == 0)
           return RootPtr;
       else {
           while(1) {
             if((RootPtr->key[position]) == name[position])
                position++;
             else
                break;
           }

           if((RootPtr->key[position]) > name[position])
             return searchClass(RootPtr->LPtr, name);
           else
             return searchClass(RootPtr->RPtr, name);
       }
     }
     else
        return NULL;
} 


/* vyhledavani funkce pomoci nazvu */
tFunctionNodePtr searchFunction (tClassNodePtr RootPtr, tFunctionNodePtr FuncPtr, char *name, char *className) {

    if(FuncPtr == NULL && RootPtr != NULL) {
        tClassNodePtr tmp;
        tmp = searchClass(RootPtr, className);
	if(tmp == NULL)
		return NULL;
        FuncPtr = tmp->function;  
     }


    int position = 0;

    if(FuncPtr != NULL) {
       if(strcmp(name, FuncPtr->key) == 0) {
           return FuncPtr;
        }
       else {
           while(1) {
             if((FuncPtr->key[position]) == name[position]) {
                position++;
             }
             else
                break;
           }

           if((FuncPtr-> key[position]) > name[position])
             return searchFunction(NULL, FuncPtr->LPtr, name, NULL);
           else
             return searchFunction(NULL, FuncPtr->RPtr, name, NULL);
       }
     }
     else
        return NULL;
}


/* vyhledavani promenne pomoci nazvu */
tVariableNodePtr searchVariable (tClassNodePtr RootPtr, tVariableNodePtr VarPtr, char *name, char *className) {

    if(VarPtr == NULL && RootPtr != NULL) {
        tClassNodePtr tmp;
        tmp = searchClass(RootPtr, className);
        VarPtr = tmp->variable; 
    }

    int position = 0;

    if(VarPtr != NULL) {
       if(strcmp(name, VarPtr->key) == 0) {
           return VarPtr;
        }
       else
       {
           while(1) {
             if((VarPtr->key[position]) == name[position]) {
                position++;
             }
             else
                break;
           }

           if((VarPtr->key[position]) > name[position])
             return searchVariable(NULL, VarPtr->LPtr, name, NULL);
           else
             return searchVariable(NULL, VarPtr->RPtr, name, NULL);
       }
     }
     else
        return NULL;
}


/* vyhledavani promenne funkce pomoci nazvu */
tVariableNodePtr searchFuncVariable (tClassNodePtr RootPtr, tVariableNodePtr VarPtr,
                                      char *className, char *funcName, char *name) {

    int position = 0;

    if(RootPtr != NULL) {
         tFunctionNodePtr FuncPtr;
         FuncPtr = searchClass(RootPtr, className) -> function; 
         if(FuncPtr != NULL) {
            tFunctionNodePtr tmp;
            tmp = searchFunction(RootPtr, NULL, funcName, className);
	    if(tmp == NULL)
		   return NULL; 
            VarPtr = tmp -> funcVariable;        
         }
     }

    if(VarPtr != NULL) {

       if(strcmp(name, VarPtr->key) == 0) {
           return VarPtr;
        }
       else
       {
           while(1) {
             if((VarPtr->key[position]) == name[position]) {
                position++;
             }
             else
                break;
           }

           if((VarPtr->key[position]) > name[position])
             return searchFuncVariable(NULL, VarPtr->LPtr, NULL, NULL, name);
           else
             return searchFuncVariable(NULL, VarPtr->RPtr, NULL, NULL, name);
       }
     }
      else
        return NULL;

}


