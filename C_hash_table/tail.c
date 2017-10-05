// tail.c
// Řešení IJC-DU2, příklad a), 3.4.2015
// Autor: Petr Flajšingr, FIT
// Přeloženo: gcc 4.8.4
// Program vypisujici pozadovany pocet radku od konce souboru
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * Tisk napovedy
 */
void printHelp(){
	printf("Pouziti: tail [MOZNOSTI] [SOUBOR]\n\
Bez pararametru vypise poslednich 10 radku souboru.\n\
Pokud je zadan soubor, cte ze souboru, jinak ze stdin.\n\n\
Moznosti pro spusteni:\n\
-n [N] - vypise N radku od konce souboru/stdin.\n\
-n +[N] - vypise vse od N-teho radku do konce souboru/stdin.\n\
--help - vypise napovedu\n");
}

/**
 * Kontrola argumentu a nastaveni hodnoty promennych
 *
 * @param argc Pocet argumentu
 * @param *argv[] Argumenty programu
 * @param *a Pocatecni hodnota vypisu
 * @param *n Pocet radku vypsanych od konce
 */
int paramCheck(int argc, char *argv[], int *a, int *n){
	char *numCheck; // Kontrola stavu prevodu na cislo
	if(argc > 4){ //platny pocet argumentu
		return 2;
	}
	else
		if(argc == 2 && strcmp(argv[1], "--help") == 0){ //vypis napovedy
			printHelp();
			return 1;
		}
		else
			if(argc == 2 || argc == 4){ //zadany soubor
				fclose(stdin); 
				stdin = fopen(argv[argc - 1], "r");
				if(stdin == NULL)
					return 3;
			}
	if(argc == 3 || argc == 4){ //zadany argument "-n"
		if(strcmp(argv[1], "-n") != 0)
			return 2;
		if(argv[2][0] == '+'){
			*a = strtol(argv[2], &numCheck, 10);
			if(numCheck[0] != 0)
				return 2;
			*n = 0;
		}
		else{
			*n = strtol(argv[2], &numCheck, 10);
			if(numCheck[0] != 0)
				return 2;
		}
	}
	return 0;
}

/**
 * Hlavni funkce
 * @param argc Pocet argumentu
 * @param *argv[] Argumenty programu
 */
int main(int argc, char *argv[]){
	int n = 10; // Pocet radku pro vypis od konce souboru
	int a = 0;  // Pocatecni radek pro vypis
	switch(paramCheck(argc, argv, &a, &n)){ //vypis chybovych hlaseni pro nepatne argumenty a soubory
		case 0: break;
		case 1: return 0;
		case 2: fprintf(stderr, "Chyba argumentu\n"); return -1;
		case 3: fprintf(stderr, "Chyba souboru\n"); return -1;
	}
	char *buffer; // Buffer pro jednotlive radky
	if(a != 0){ //vypis od urciteho radku
		int lineCount = 0; // Aktualni pozice v souboru
		buffer = malloc(510 * sizeof(char)); //alokace pameti pro nacteni jednoho radku

		if(buffer == NULL){ //kontrola alokace
			fprintf(stderr, "Chyba alokace\n"); 
			return -1;
		}

		while(fgets(buffer, 510, stdin) != NULL){ //cyklus prochazejici vstup dokud nenarazi na EOF
			lineCount++; //inkrementace poctu radku
			if(lineCount >= a){ //vypis pozadovanych radku
				printf("%s", buffer);
			}
		}
	}
	else{ //pokud je zadana promenna n
		buffer = malloc((n + 1) * 510 * sizeof(char)); //alokace pameti pro nacteni pozadovaneho poctu radku
		if(buffer == NULL){ //kontrola alokace
			fprintf(stderr, "Chyba alokace\n"); 
			return -1;
		}
		int check = 0; // Kontrola delky radku
		bool err = 0; // Status chyboveho vypisu
		while(fgets(&buffer[n * 510], 510, stdin) != NULL){ //cyklus prochazejici vstup dokud nenarazi na EOF
			if(check == 1){
				check = 2;
			}
			if(strlen(&buffer[n * 510]) == 509 && check != 2){ //velikost radku mimo rozsah
				check = 1;
				if(err == 0){ //vypis chyby pri prekroceni velikosti radku
					fprintf(stderr, "CHYBA: Radky jsou delsi nez 510 znaku\n");
					err = 1;
				}
			}
			if(check != 2){ //posuv radku v bufferu
				for(int x = 0; x < n; x++){
					for(int y = 0; y < 510; y++){
						buffer[x * 510 + y] = buffer[(x + 1) * 510 + y];
					}
				}
			}
			if(strlen(&buffer[n * 510]) != 509 && check == 2){
				check = 0;
			}
		}
		for(int x = 0; x < n; x++){ //cyklus pro vypis bufferu
			for(int y = 0; y < 510; y++){
				printf("%c", buffer[x * 510 + y]);
				if(buffer[x * 510 + y] == '\n'){ //break pri ukonceni radku
					break;
				}
			}
			if(buffer[x * 510 + strlen(&buffer[x * 510]) - 1] != '\n' && buffer[x * 510 + strlen(&buffer[x * 510]) - 1] != 0){
				putchar(10);
			}
		}
	}
	free(buffer); //uvolneni alokovane pameti
	fclose(stdin);
	return 0;
}
