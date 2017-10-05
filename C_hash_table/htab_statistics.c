// htab_statistics.c
// Řešení IJC-DU2, příklad b), 28.4.2015
// Autor: Petr Flajšingr, FIT
// Přeloženo: gcc 4.8.4
// Funkce pro zobrazeni informaci o hash tabulce

#include <stdlib.h>
#include <stdio.h>
#include "htable.h"

/**
 * Statistiky hash tabulky
 * @param *t Hash tabulka
 */
void htab_statistics(htab_t *t){
	unsigned int min = -1; //Minimalni pocet zaznamu
	unsigned int max = 0; //Maximalni pocet zaznamu
	unsigned int avg = 0; //Prumerny pocet zaznamu
	unsigned int count = 0; //Pocitadlo
	struct htab_listitem *item;

	for(unsigned int x = 0; x < t->size; x++){ //Cela tabulka
		item = t->data[x]; //Posun v tabulce
		while(item != NULL){ //Cely seznam
			count++; //Inkrementace pocitadla
			item = item->next; //Posun v seznamu
		}
		if(count > max){ //Nove maximum
			max = count;
		}
		if(count < min){ //Nove minimum
			min = count;
		}
		avg += count;
		count = 0;
	}
	printf("Min: %d\nMax: %d\nAvg: %d\n", min, max, avg/t->size);
}
