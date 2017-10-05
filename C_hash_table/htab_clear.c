// htab_clear.c
// Řešení IJC-DU2, příklad b), 28.4.2015
// Autor: Petr Flajšingr, FIT
// Přeloženo: gcc 4.8.4
// Funkce pro smazani vsech zaznamu v tabulce

#include <stdlib.h>
#include "htable.h"

/**
 * Uvolneni seznamu
 * @param *item Pocatecni zaznam
 */
void htab_freeRow(struct htab_listitem *item) {
	if(item == NULL) //Konec seznamu
		return;
	htab_freeRow(item->next); //Rekurzivni volani funkce
	//Uvolneni pameti
	free(item->key);
	free(item);
}

/**
 * Uvolneni vsech zaznamu hash tabulky
 * @param *t Hash tabulka
 */
void htab_clear(htab_t *t){
	for (unsigned int x = 0; x < t->size; x++){ //Po konec seznamu
		htab_freeRow((t->data)[x]); //Uvolneni seznamu na dane pozici
	}
}
