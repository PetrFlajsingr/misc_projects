// htab_free.c
// Řešení IJC-DU2, příklad b), 28.4.2015
// Autor: Petr Flajšingr, FIT
// Přeloženo: gcc 4.8.4
// Funkce pro smazani hash tabulky
#include <stdlib.h>
#include "htable.h"

/**
 * Uvolneni cele hash tabulky
 * @param *t Hash tabulka
 */
void htab_free(htab_t *t){
	htab_clear(t); //Uvolneni vsech zaznamu
	//Uvolneni tabulky
	free(t->data);
	free(t);
}
