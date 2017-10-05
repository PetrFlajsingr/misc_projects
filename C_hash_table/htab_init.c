// htab_init.c
// Řešení IJC-DU2, příklad b), 28.4.2015
// Autor: Petr Flajšingr, FIT
// Přeloženo: gcc 4.8.4
// Funkce pro inicializaci hash tabulky

#include <stdlib.h>
#include "htable.h"

/**
 * Inicializuje hashovaci tabulku
 * @param size Velikost tabulky
 */
htab_t * htab_init(unsigned int size){
	htab_t *htable; //Hashovaci tabulka
	htable = malloc(sizeof(htab_t)); 
	if(htable == NULL){ //Kontrola alokace
		return NULL;
	}
	htable->size = size; //Ulozeni velikosti
	htable->data = calloc(size, sizeof(struct htab_listitem *)); //Alokace a vynulovani pameti
	if(htable->data == NULL){ //Kontrola alokace
		free(htable);
		return NULL;
	}
	return htable;
}
