// io.c
// Řešení IJC-DU2, příklad b), 28.4.2015
// Autor: Petr Flajšingr, FIT
// Přeloženo: gcc 4.8.4
// Funkce pro cteni slova ze souboru
#include <ctype.h>
#include <stdio.h>

/**
 * Cteni slova ze souboru
 * @param *s Pointer pro ulozeni precteneho stringu
 * @param max Maximalni delka slova
 * @param *f Cteny soubor
 */
int fgetw(char *s, int max, FILE *f){
	char a;
	int x = 0; //Pocitadlo prectenych znaku
	while((a=fgetc(f)) != EOF && x < max){ //Cteni do konce souboru nebo max poctu znaku
		if(isspace(a)){ //Cteny znak je bily
			if(x != 0){ //Pokud byl precten aspon jeden znak
				break; //Ukonceni cteni
			}
		}
		else{ //Ulozeni znaku
			s[x] = a;
			x++;
		}
	}
	s[x] = 0; //Ukonceni stringu
	if(x > max){ //Zahozeni zbytku slova
		while(!isspace((a = fgetc(f))));
	}
	if(x == 0){ //Nula znaku precteno
		return EOF;
	}
	return x;
}
