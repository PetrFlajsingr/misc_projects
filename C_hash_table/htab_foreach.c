// htab_foreach.c
// Řešení IJC-DU2, příklad b), 28.4.2015
// Autor: Petr Flajšingr, FIT
// Přeloženo: gcc 4.8.4
// Funkce pro volani funkce pro kazdy prvek hash tabulky

#include <stdlib.h>
#include "htable.h"

/**
 * Volani funkce pro kazdy zaznam hash tabulky
 * @param *t Hash tabulka
 * @param *function Volana funkce
 */
void htab_foreach(htab_t *t, void(*function)(const char *key, unsigned int value)){
	struct htab_listitem *item;
	for(unsigned int x = 0; x < t->size; x++){ //Cela tabulka
		item = (t->data)[x]; //Posun v tabulce
		while(item != NULL){
			function(item->key, item->data); //Volani funkce
			item = item->next; //Posun v seznamu
		}
	}
}
