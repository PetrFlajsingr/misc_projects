#include "param.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern FILE* source;
const char* helpMsg = ("Prekladac jazyka IFJ16\n\n\
Autori: \n\
    Petr Flajsingr, xflajs00@stud.fit.vutbr.cz\n\
    Igor Frank, xfrank12@stud.fit.vutbr.cz\n\
    Dominik Dvorak, xdvora1t@stud.fit.vutbr.cz\n\
    Pavel Mica, xmicap02@stud.fit.vutbr.cz\n\
    Martin Pospech, xpospe03@stud.fit.vutbr.cz\n\n\
Datum: 04.12.2016\n\n\
Popis: \n\
   Jedna se o prekladac jazyka IFJ16 (podmnozina jazyka Java SE), ktery \n\
   dany jazyk interpretuje. Implementovany je v jazyce C. \n\
   V pripade chyby se program ukonci chybovym hlasenim a vrati odpovidajici chybovy kod.\n\n\
   V projektu jsou implementovane dve rozsireni, UNARY a BASE, pricemz UNARY je implementovano pouze castecne.\n\n\
Pouziti:\n\
	./ifj16 <input_file>\n");
int paramCheck(int argc, char** argv){
	if(argc == 2){
		if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0){
			printf("%s", helpMsg);
			return 1;
		}
		source = fopen(argv[1], "r");
		if(source == NULL){
			return 99;
		}
		return 0;
	}
	return 99;
}