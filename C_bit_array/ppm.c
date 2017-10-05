// ppm.c
// Řešení IJC-DU1, příklad b), 18.3.2015
// Autor: Petr Flajšingr, FIT
// Přeloženo: gcc 4.8.4
// modul obsahujici funkce pro zapsani souboru *.ppm do struktury a pro zapsani struktury do *.ppm
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "error.h"
#include "ppm.h"

//nacteni souboru *.ppm do struktury ppm
struct ppm * ppm_read(const char *filename)
{
	//otevreni souboru
	FILE *file = fopen(filename, "rb");
	//neuspesne otevreni
	if(file == NULL)
	{
		Warning("Chyba otevreni souboru\n");
		return NULL;
	}
	//promenne pro nacteni hlavickovych hodnot
	unsigned a, b, c;
	int ch = fscanf(file,"P6 %u %u %u", &a, &b, &c);
	//neplatne informace v hlavicce
	if(ch != 3 || c != 255)
	{
		Warning("Chyba hlavicky\n");
		fclose(file);
		return NULL;
	}
	//kontrola ukonceni hlavicky
	char check;
	if(fscanf(file, "%c", &check) != 1 && isspace(check) == 0)
	{
		Warning("Chyba hlavicky\n");
		fclose(file);
		return NULL;
	}
	//alokace struktury pro nacteni souboru
	struct ppm *ppmS = malloc(sizeof(struct ppm) + 3 * a * b);
	//chyba alokace
	if(ppmS == NULL)
	{
		Warning("Chyba alokace\n");
		fclose(file);
		return NULL;
	}
	//cteni ze souboru do struktury
	//osetruje chybu cteni
	if(fread(ppmS->data, 1, 3 * a * b, file) != 3 * a * b)
	{
		Warning("Chyba cteni\n");
		free(ppmS);
		fclose(file);
		return NULL;
	}
	//ulozeni velikosti dat do struktury
	ppmS->xsize = a;
	ppmS->ysize = b;
	//uzavreni souboru
	fclose(file);
	//vraceni struktury naplnene *.ppm
	return ppmS;
}

//zapis struktury ppm do souboru *.ppm
int ppm_write(struct ppm *p, const char *filename)
{
	//otevreni souboru
	FILE *file = fopen(filename, "wb");
	//chyba otevreni souboru
	if(file == NULL)
	{
		Warning("Chyba otevreni\n");
		return -1;
	}
	//zapis hlavicky do souboru
	fprintf(file,"P6\n%u %u\n255\n", p->xsize, p->ysize);
	//zapis dat do souboru
	fwrite(p->data, 1, 3 * p->xsize * p -> ysize, file);
	//uzavreni souboru
	fclose(file);
	return 0;
}
