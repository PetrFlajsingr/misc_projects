// tail2.cc
// Řešení IJC-DU2, příklad a), 9.4.2015
// Autor: Petr Flajšingr, FIT
// Přeloženo: gcc 4.8.4
// Program vypisujici pozadovany pocet radku od konce souboru
#include <string>
#include <iostream>
#include <queue>
#include <sstream>
#include <cctype>
#include <cstring>
#include <fstream>

using namespace std;

/**
 * Tisk napovedy
 */
void printHelp()
{
	cout 	<< "Pouziti: tail [MOZNOSTI] [SOUBOR]" << endl
			<< "Bez pararametru vypise poslednich 10 radku souboru." << endl
			<< "Pokud je zadan soubor, cte ze souboru, jinak ze stdin." << endl << endl
			<< "Moznosti pro spusteni:" << endl
			<< "-n [N] - vypise N radku od konce souboru/stdin." << endl
			<< "-n +[N] - vypise vse od N-teho radku do konce souboru/stdin." << endl
			<< "--help - vypise napovedu" << endl;
}

/**
 * Hlavni funkce
 * @param argc Pocet argumentu
 * @param *argv[] Argumenty programu
 */
int main(int argc, char *argv[]){
	ios::sync_with_stdio(false);
	int a = 0; //Pocatecni pozice pro vypis
	int n = 10; //Pocet radku pro vypsani od konce
	fstream file; //Soubor
	istream *input = &cin; //Pro cteni

	char *numCheck; // Kontrola stavu prevodu na cislo
	if(argc > 4){ //platny pocet argumentu
		cerr << "Chyba argumentu" << endl;
	}
	else{
		if(argc == 2 && strcmp(argv[1], "--help") == 0){ //vypis napovedy
			printHelp();
			return 0;
		}
		else{
			if(argc == 2 || argc == 4){ //zadany soubor
				file.open(argv[argc - 1], ios::in);
				if (!file.is_open()){
					cerr << "Chyba souboru" << endl;
					return 1;
				}
				else{
					input = &file;				
				}
			}
		}
	}
	if(argc == 3 || argc == 4){ //zadany argument "-n"
		if(strcmp(argv[1], "-n") != 0){
			cerr << "Chyba argumentu" << endl;
			return 2;
		}
		if(argv[2][0] == '+'){
			a = strtol(argv[2], &numCheck, 10);
			if(numCheck[0] != 0){
				cerr << "Chyba argumentu" << endl;
			}
			n = 0;
		}
		else{
			n = strtol(argv[2], &numCheck, 10);
			if(numCheck[0] != 0){
				cerr << "Chyba argumentu" << endl;
			}
		}
	}
	int x = 0; //Pocitadlo
	string oneLine; //Buffer pro jeden radek
	if(a != 0){ //Vypis od urciteho radku
		while(getline(*input, oneLine) != NULL){
			x++;
			if(x >= a){
				cout << oneLine << "\n";
			}
		}
	}
	else{
		queue<string> buffer; //Buffer pro vypis pozadovanych radku	

		while(getline(*input, oneLine) != NULL) {
			buffer.push(oneLine); //Ulozeni radku do bufferu
			x++;
			if(x > n){ //Vymazani nejstarsiho udaje z bufferu pri presazeni velikosti
				buffer.pop();
			}
		}	

		for(int y = 0; y < n && y < x; y++){ //Vypis radku z bufferu
			cout << buffer.front() << endl;
			buffer.pop();
		}
	}
	return 0;
}