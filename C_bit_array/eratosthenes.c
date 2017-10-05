// eratosthenes.c
// Řešení IJC-DU1, příklad a), 18.3.2015
// Autor: Petr Flajšingr, FIT
// Přeloženo: gcc 4.8.4
// modul obsahujici funkci pro nalezeni prvocisel pomoci Eratosthenova sita
#include <stdio.h>
#include <math.h>
#include "eratosthenes.h"
#include "bit-array.h"
#include <stdbool.h>

//funkce pro vyber prvocisel v bitovem poli
void Eratosthenes(BitArray_t pole)
{
	//maximalni kontrolovana hodnota
	const unsigned long sqN = sqrt(BA_size(pole));
    for(unsigned long x = 2; x < sqN; x++)
    {
		//pokud je hodnota prvocislo
		if(!(BA_get_bit(pole, x)))
		{
			//nastaveni vsech nasobku prvocisla na 1
			for(unsigned long y = 2; y * x < BA_size(pole); y++)
			{
				BA_set_bit(pole, y * x, 1);
			}
		}
	}
}
