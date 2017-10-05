// htab.h
// Řešení IJC-DU2, příklad b), 28.4.2015
// Autor: Petr Flajšingr, FIT
// Přeloženo: gcc 4.8.4
// Tabulky obsahujici rozhrani funkci pro praci s hash tabulkou

#ifndef HTAB_H
#define HTAB_H

/**
 * Struktura pro jednotlive polozky seznamu
 */
struct htab_listitem{
	char *key; //String
	int data; //Pocet vyskytu
	struct htab_listitem *next; //Odkaz na dalsi prvek seznamu
};
/**
 * Struktura pro hash tabulku
 */
typedef struct{
	unsigned size; //velikost tabulky
	struct htab_listitem **data; //Odkazy na jednotlivé prvky tabulky
}htab_t;

//Uvolneni seznamu v tabulce
void htab_freeRow(struct htab_listitem *);

//Hashovaci funkce ze zadani
unsigned int hash_function(const char *, unsigned);

//Inicializace hash tabulky
htab_t * htab_init(unsigned int);

//Vyhledani zaznamu v tabulce
struct htab_listitem * htab_lookup(htab_t *, const char *);

//Smazani zaznamu v tabulce
void htab_remove(htab_t *, const char *);

//Smazani vsech zaznamu tabulky
void htab_clear(htab_t *); 

//Smazany cele tabulky
void htab_free(htab_t *);

//Vlani funkce pro kazdy prvek tabulky
void htab_foreach(htab_t *, void(*function)(const char *, unsigned int));

//Statistiky tabulky
void htab_statistics(htab_t *);

#endif
