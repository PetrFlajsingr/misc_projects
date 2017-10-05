// htab_lookup.c
// Řešení IJC-DU2, příklad b), 28.4.2015
// Autor: Petr Flajšingr, FIT
// Přeloženo: gcc 4.8.4
// Funkce pro vyhledani zaznamu v hash tabulce

#include <string.h>
#include <stdlib.h>
#include "htable.h"

/**
 * Vyhledani prvku v hash tabulce
 * @param *t Hashovaci tabulka
 * @param *key Vyhledavany string
 */
struct htab_listitem * htab_lookup(htab_t *t, const char *key){
	unsigned int hPos = hash_function(key, t->size); //Hash key v tabulce
	struct htab_listitem *item = (t->data)[hPos]; //Pozice hash v tabulce

	if(item == NULL){ //Prazdny zaznam
		item = malloc (sizeof(struct htab_listitem));
		if(item == NULL){ //Kontrola alokace
			return NULL;
		}
		//Ulozeni hodnot
		item->next = NULL;
		item->data = 0;
		item->key = malloc((strlen(key) + 1) * sizeof(char));
		if(item->key == NULL){
			return NULL;
		}
		strcpy(item->key, key);
		(t->data)[hPos] = item; //Zapsani adresy do hash tabulky
		return item;
	}
	if(strcmp(key, item->key) == 0){ //Key odpovida zaznamu
		return item;
	}
	while(item->next != NULL){ //Prohledani celeho seznamu
		item = item->next; //Posun v seznamu
		if(strcmp(key, item->key) == 0){ //Key odpovida zaznamu
			return item;
		}
	}
	//Pokud neni nalezen odpovidajici zaznam, pridani noveho na konec seznamu
	item->next = malloc(sizeof(struct htab_listitem));
	item = item->next;
	if(item == NULL){ //Kontrola alokace
		return NULL;
	}
	//Ulozeni hodnot
	item->next = NULL;
	item->data = 0;
	item->key = malloc((strlen(key) + 1) * sizeof(char));
	if(item->key == NULL){
		return NULL;
	}
	strcpy(item->key, key);
	return item;
}
