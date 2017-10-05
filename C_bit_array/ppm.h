// ppm.h
// Řešení IJC-DU1, příklad b), 18.3.2015
// Autor: Petr Flajšingr, FIT
// Přeloženo: gcc 4.8.4
// hlavickovy soubor obsahujici rozhrani modulu ppm.c 
// a definujici strukturu pro nacteni ppm souboru
#ifndef PPM_H
#define PPM_H
#include "error.h"
//struktura pro ulozeni ppm souboru
struct ppm 
{
	unsigned xsize;
    unsigned ysize;
    char data[];    // RGB bajty, celkem 3*xsize*ysize
};

struct ppm * ppm_read(const char *);
int ppm_write(struct ppm *, const char *);
#endif
