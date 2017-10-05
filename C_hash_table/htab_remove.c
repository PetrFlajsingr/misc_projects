// htab_remove.c
// Řešení IJC-DU2, příklad b), 28.4.2015
// Autor: Petr Flajšingr, FIT
// Přeloženo: gcc 4.8.4
// Funkce pro smazani zaznamu v hash tabulce

#include <string.h>
#include <stdlib.h>
#include "htable.h"

/**
 * Smazani zaznamu v hash tabulce
 * @param *t Hash tabulka
 * @param *key Vyhledavany string
 */
void htab_remove(htab_t *t, const char *key){
	unsigned int hPos = hash_function(key, t->size); //Pozice v tabulce
	struct htab_listitem *item = (t->data)[hPos];

	if(strcmp(item->key, key) == 0){ //Prvni zaznam odpovida
		(t->data)[hPos] = item->next; //Preskoceni zaznamu
		//Uvolneni pameti
		free(item->key);
		free(item);
		return; //Ukonceni funkce
	}
	struct htab_listitem *oneUp = item; //Prechozi prvek
	item = item->next;
	while(item->next != NULL){ //Do konce seznamu
		if(strcmp(item->key, key) == 0){
			oneUp->next = item->next; //Preskoceni zaznamu
			free(item->key);
			free(item);
			break;
		}
		//Posun v seznamu
		oneUp = item;
		item = item->next;
	}
}
