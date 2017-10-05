// steg-decode.c
// Řešení IJC-DU1, příklad b), 18.3.2015
// Autor: Petr Flajšingr, FIT
// Přeloženo: gcc 4.8.4
// program pro nacteni zakodovane zpravy ze souboru typu ppm(typ P6)

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <limits.h>
#include "bit-array.h"
#include "eratosthenes.h"
#include "error.h"
#include "ppm.h"

//horni limit velikosti
#define LIMIT 5000*5000*3

int main(int argc, char *argv[])
{
	//struktura pro zapis souboru
	struct ppm *ppmS;
	//pocitadlo pro nacitani bitu
	unsigned int y = 0;
	//promenna signalizujici konec zpravy
	bool end = 0;
	//promenna obsahujici hodnotu bitu z ppm
	bool value = 0;
	//pole pro ulozeni znaku(dvouprvkove kvuli kompatiblite s BA makry)
	unsigned char message[2] = {1};
	//chyba argumentu
	if(argc != 2)
	{
		Warning("Chyba argumentu\n");
		return -1;
	}
	//zapis souboru do struktury
	ppmS = ppm_read(argv[1]);
	//chyba alokace
	if(ppmS == NULL)
		return -1;
	//vytvoreni pole bitu
	BA_create(prime, LIMIT);
	//kontrola prvocisel v bitovem poli
	Eratosthenes(prime);
	for(unsigned long x = 2; x < LIMIT; x++)
	{
		//kontrola prvocisel
		if(BA_get_bit(prime, x) == 0)
		{
			//nastaveni daneho bitu z *.ppm
			value = ppmS->data[x] & 1;
			BA_set_bit(message, y, value);
			//zvyseni urovne zapisu
			if (y < 7) 
				y++;
			else 
				//ukonceni zpravy
				if(message[1] == 0)
				{
					end = true;
					break;
				}		 
				else 
					//pri naplneni bytu
					if(y == 7)
					{
						//kontrola tisknutelnosti znaku
						if(isprint(message[1]) == 0) 
							FatalError("Soubor obsahuje netisknutelny znak\n");
						//tisk platneho znaku
						putchar(message[1]);
						//vynulovani pocitadla pro zapis bitu
						y = 0;
					}
		}
	}
	putchar(10);
	//uvolneni alokovane pameti pro strukturu
	free(ppmS);
	//pri chybnem ukonceni zpravy
	if(end == 0) 
		FatalError("Chybne ukonceni zpravy");
	return 0;
}
