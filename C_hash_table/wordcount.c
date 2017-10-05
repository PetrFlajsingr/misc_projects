// wordcount.c
// Řešení IJC-DU2, příklad b), 28.4.2015
// Autor: Petr Flajšingr, FIT
// Přeloženo: gcc 4.8.4
// Program vypisujici pocet vyskytu slov na stdin

#include <stdio.h>
#include <stdlib.h>
#include "htable.h"
#include "io.c"

/*
	Urceno podle vysledku htab_statistic pri zadavani
	vysledku seq x y a /usr/share/dict/words
	prumer u seq 1000000 2000000 byl 40
	vyssi hodnoty uz by zabiraly velke mnozstvi pameti
*/
#define N 25000 

/**
 * Vypis zadaneho string a int
 * @param *key String
 * @param data Hodnota
 */
void htab_printKeyData(const char *key, unsigned int data)
{
	printf("%s\t%d\n", key, data);
}

/**
 * Hlavni funkce
 */
int main(){
	char a[128]; //Pole pro ulozeni slova
	htab_t *t = htab_init(N); //Inicializace tabulky
	struct htab_listitem *item; //Struktura pro ukladani informaci

	while(fgetw(a, 127, stdin) != EOF){ //Cteni do konce souboru
		item = htab_lookup(t, a); //Vyhledani pozice v hash tabulce
		if(item == NULL){
			fprintf(stderr, "Chyba alokace");
			htab_free(t);
			return -1;
		}
		item->data++; //Inkrementace poctu nalezu
	}
	htab_foreach(t, &htab_printKeyData); //Tisk vsech dat na stdout
	htab_free(t); //Uvolneni pameti
	return 0;
}
