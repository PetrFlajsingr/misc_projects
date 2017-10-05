/**
 * @file ial.c
 * @project IFJ16
 * @author Petr Flajsingr (xflajs00@stud.fit.vutbr.cz)
 * @date 4 November 2016
 * @brief Modul obahujici definice funkci pro vyhledavaci algoritmus KMP
 * a radici algoritmus heap sort
 **/

#include "ial.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

//***************SORT******************

/**
 * @brief Vyhledavaci algoritmus heap sort
 * @param string vstupni retezec
 * @return serazeny retezec nebo NULL v pripade neuspechu
 **/
char* sort(char* string){
	int len = strlen(string);
	char* newString = malloc(sizeof(char) * (len + 1)); //serazeny string
	if(newString == NULL || len == 0){ //kontrola alokace
		return NULL;
	}
	T_heap* heap = buildHeap(string, len); //vytvoreni heap pro heapsort
	if(heap == NULL){ //chyba alokace
		return NULL;
	}
	minHeap(heap, len); //serazeni heap 
	for(int i = len - 1; i > 0; --i){ //ukladani serazenych znaku a zmena heap
		newString[len - 1 - i] = heap->value;
		heap = newHeapRoot(heap, i + 1);
		minHeap(heap, i);
	}
	newString[len - 1] = heap->value; //ulozeni posledni hodnoty
	newString[len] = '\0';
	freeHeap(heap);
	return newString;
}

/**
 * @brief Vytvoreni heapu pro sort
 * @param string vstupni retezec
 * @param len delka vstupniho retezce
 * @return vytvoreny heap
 **/
T_heap* buildHeap(char* string, int len){
	T_heap* heap = malloc(sizeof(T_heap)); //alokace pameti
	if(heap == NULL){ //chyba alokace
		return NULL;
	}

	heap->value = string[0]; //ulozeni prvniho prvku

	//tvorba a vynulovani zasobniku
	T_stack stack[len];
	for(int i = 0; i < len; ++i){
		stack[i] = NULL;
	}

	stack[0] = heap;
	int j = 0; //pocitadlo pro cteni ze zasobniku
	int j2 = 1; //pocitadlo pro ukladani do zasobniku
	T_heap* temp;
	for(int i = 1; i < len; ++i){ //ulozeni vsech prvku
		if(i % 2 == 1){ //left child
			temp = stack[j];
			stack[j2] = temp->left_child = malloc(sizeof(T_heap));
			temp->left_child->value = string[i];
			temp->left_child->parent = temp;
			j++;
		}else{ //right child
			stack[j2] = temp->right_child = malloc(sizeof(T_heap));
			temp->right_child->value = string[i];
			temp->right_child->parent = temp;
		}
		j2++;
	}
	return heap;
}

/**
 * @brief Presun nejmensiho prvku do root node
 * @param heap heap...
 * @param size pocet prvku
 **/
void minHeap(T_heap* heap, int size){
	T_heap* temp;
	bool again = false; //prepinac pro opakovani razeni
	for(int i = size / 2; i > 0; --i){ //od posledniho prvku s child node
		temp = getNodePtr(heap, i); //nalezeni prvku
		//prehozeni prvku mezi uzly
		if(temp->right_child != NULL && temp->right_child->value < temp->value){
			swap(&(temp->right_child->value), &(temp->value));
			again = true;
		}
		if(temp->left_child != NULL && temp->left_child->value < temp->value){
			swap(&(temp->left_child->value), &(temp->value));
			again = true;
		}
	}
	if(again){
		minHeap(heap, size);
	}
}

/**
 * @brief Ziskani pointeru na pozadovany prvek
 * @param heap heap...
 * @param index pozaovany index
 * @return pointer na pozadovany prvek
 **/
T_heap* getNodePtr(T_heap* heap, int index){
	int pathLen = index;
	T_heap* temp = heap;
	int stack[index];

	for(int i = 0; i < index; i++){ //nastaveni stavu pro ignorovani hodnot
		stack[i] = 2;
	}

	//urceni cesty k uzlu
	for(int i = 0; index > 1; ++i){
		if(index % 2 == 0){
			stack[i] = 0;
		}else{
			stack[i] = 1;
		}
		index /= 2;
	}

	//pruchod k pozadovanemu uzlu
	for(int i = pathLen - 1; i >= 0; --i){
		if(stack[i] == 0){
			temp = temp->left_child;
		}else if(stack[i] == 1){
			temp = temp->right_child;
		}
	}
	return temp;
}

/**
 * @brief Prohozeni hodnot dvou char
 **/
void swap(char* a, char* b){
	char temp = *a;
	*a = *b;
	*b = temp;
}

/**
 * @brief Smazani root node a vlozeni nove
 * @param heap heap...
 * @param size pocet prvku
 * @return upraveny heap
 **/
T_heap* newHeapRoot(T_heap* heap, int size){
	T_heap* temp = getNodePtr(heap, size);
	//uprava hodnot na nove
	temp->left_child = heap->left_child;
	temp->right_child = heap->right_child;
	//smazani zaznamu o posunutem prvku
	if(temp == temp->parent->left_child){
		temp->parent->left_child = NULL;
	}else if(temp == temp->parent->right_child){
		temp->parent->right_child = NULL;
	}
	free(heap);
	return temp;
}

/**
 * @brief Uvolneni neuvolnenych prvku
 **/
void freeHeap(T_heap* heap){
	if(heap->left_child != NULL){
		free(heap->left_child);
	}
	//free(heap);
}

//**************/SORT******************

//***************FIND******************
/**
 * @brief Funkce pro vyhledavani podretezce v jinem retezci
 * @param string retezec ve kterem je hledano
 * @param substring vyhledavany retezec
 * @return pocatecni index nalezeneho retezce
 **/
int find(char* string, char* substring){
	int m = 0, i = 0, table[strlen(substring)]; //inicializace promennych
	matchTable(substring, table); //tvorba tabulky
	//samotny KMP algoritmus
	while((m + i) < strlen(string)){
		if(substring[i] == string[m + i]){
			if(i == strlen(substring) - 1){
				return m;
			}
			i++;
		}else{
			if(table[i] > -1){
				m = m + i - table[i];
				i = table[i];
			}else{
				m++;
				i = 0;
			}
		}
	}
	return -1;
}

/**
 * @brief Vytvoreni tabulky pro KMP
 * @param subtring vyhledavany retezec
 * @param table tabulka castecnych match
 **/
void matchTable(char* substring, int* table){
	int pos = 2;
	int cnd = 0;

	table[0] = -1;
	table[1] = 0;

	while(pos < strlen(substring)){
		if(substring[pos - 1] == substring[cnd]){
			table[pos] = cnd + 1;
			cnd++;
			pos++;
		}else if(cnd > 0){
			cnd = table[cnd];
		}else {
			table[pos] = 0;
			pos++;
		}
	}
}



//**************/FIND******************