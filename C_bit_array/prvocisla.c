// prvocisla.c
// Řešení IJC-DU1, příklad a), 18.3.2015
// Autor: Petr Flajšingr, FIT
// Přeloženo: gcc 4.8.4
// program pro vypis poslednich 10 prvocisel v rozsahu 2 - 201000000
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "bit-array.h"
#include "error.h"
#include "eratosthenes.h"

//makro pro urceni rozsahu vyhledavani prvocisel
#define N 201000000

//vypis poslednich n prvocisel v danem rozsahu
void printLastN(BitArray_t prime, int n)
{
	int i = N - 1; //posledni prvek
	//nalezeni n-teho nejvetsiho prvocisla
	for(; n != 0; i--)
		if(!BA_get_bit(prime, i))
			n--;
	//vypis od n-teho prvocisla
	for(; i < N; i++)
		if(!BA_get_bit(prime, i))
			printf("%d\n", i);
}

int main()
{
	//vytvoreni bitoveho pole
	BA_create(prime, N);
	//oznaceni prvocisel podle pozice
	Eratosthenes(prime);
	//vypsani poslednich 10 prvocisel na stdout
	printLastN(prime, 10);
    return 0;
}
