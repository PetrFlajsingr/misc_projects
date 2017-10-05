#include <fitkitlib.h>
#include <keyboard/keyboard.h>
#include <lcd/display.h>

/**
 * Projekt: IMP, MSP430: Enigma
 * Autor: Petr Flajsingr, xflajs00@stud.fit.vutbr.cz
 * Popis: Jednoducha implementace sifrovaciho stroje enigma v jazyce C.
 *		  K vypracovani byla pouzita univerzitou poskytnuta aplikace pro demonstraci komunikace klavesnice FITKITU s terminalem a ovladani LCD.
 **/
#define A 0
#define B 1
#define C 2
#define D 3
#define E 4
#define F 5
#define G 6
#define H 7
#define I 8
#define J 9
#define K 10
#define L 11
#define M 12
#define N 13
#define O 14
#define P 15
#define Q 16
#define R 17
#define S 18
#define T 19
#define U 20
#define V 21
#define W 22
#define X 23
#define Y 24
#define Z 25
#define TOCHAR(x) x + 65
#define ROTOR_SIZE 26

char rotor_1[ROTOR_SIZE] 		=	{D,M,T,W,S,I,L,R,U,Y,Q,N,K,F,E,J,C,A,Z,B,P,G,X,O,H,V}; //posun po kazdem step
char rotor_2[ROTOR_SIZE]  		= 	{H,Q,Z,G,P,J,T,M,O,B,L,N,C,I,F,D,Y,A,W,V,E,U,S,R,K,X}; //posun kdyz rotor_1 prejde na W
char rotor_3[ROTOR_SIZE]  		= 	{U,Q,N,T,L,S,Z,F,M,R,E,H,D,P,X,K,I,B,V,Y,G,J,C,W,O,A}; //posun kdyz rotor_2 prejde na E

char reflector[ROTOR_SIZE]  	= 	{G,C,B,Q,N,I,A,R,F,T,P,W,Z,E,X,K,D,H,U,J,S,Y,L,O,V,M};

char plugboard[26] 				= 	{C,B,A,D,E,F,G,H,I,J,K,L,P,N,O,M,Q,R,S,T,U,V,W,X,Y,Z}; 

char cur_char;
int start;
//offsety pro rotaci rotoru
char rotor_1_offset;
char rotor_2_offset;
char rotor_3_offset;
//prepinac pro nastaveni hodnot rotoru
char rotor_set;
char char_cnt;
char last_ch; //naposledy precteny znak

//sifrovaci/desifrovaci funkce
char encrypt(char character){
	char result;
	int i;

	//prohnani znaku "mechanismem"
	result = plugboard[character];
	result = rotor_1[(ROTOR_SIZE + (result + rotor_1_offset)) % ROTOR_SIZE];
	result = rotor_2[(ROTOR_SIZE + result + rotor_2_offset) % ROTOR_SIZE];
	result = rotor_3[(ROTOR_SIZE + result + rotor_3_offset) % ROTOR_SIZE];
	result = reflector[result];
	//fory protoze uz na to nemam nervy
	for(i = 0; i < ROTOR_SIZE; i++){
		if(rotor_3[i] == result){
			result = (ROTOR_SIZE + i - rotor_3_offset) % ROTOR_SIZE;
			break;
		}
	}
	for(i = 0; i < ROTOR_SIZE; i++){
		if(rotor_2[i] == result){
			result = (ROTOR_SIZE + i - rotor_2_offset) % ROTOR_SIZE;
			break;
		}
	}
	for(i = 0; i < ROTOR_SIZE; i++){
		if(rotor_1[i] == result){
			result = (ROTOR_SIZE + i - rotor_1_offset) % ROTOR_SIZE;
			break;
		}
	}
	result = plugboard[result];
	//zvyseni a kontrola preteceni offsetu
	if(++(rotor_1_offset) > Z){
		rotor_1_offset = A;
	}
	if(rotor_1_offset == W){ 
		if(++(rotor_2_offset) > Z){
			rotor_2_offset = A;
		}
		if(rotor_2_offset == E){ 
			if(++(rotor_3_offset) > Z){
				rotor_3_offset = A;
			}
		}
	}
	return result;
}


void print_user_help(void)
{
	term_send_str("Nastaveni rotoru:\nTlacitka A/B/C pro vyber rotoru, nasledne napsat pocatecni znak na danem rotoru na klavesnici\nAktualni nastaveni rotoru lze zorazit stisknutim *\n");
}


int keyboard_idle()
{
  char encr;
  char ch;
  ch = key_decode(read_word_keyboard_4x4());
  if (ch != last_ch) // stav se zmenil
  {
    last_ch = ch;
    if (ch != 0) // vylucime pusteni klavesy
    {
		switch(ch){
			case '1':{
				print_user_help();
				break;
			}
			case '*':{
				term_send_str("Aktualni nastaveni rotoru 1: ");
				term_send_char(TOCHAR(rotor_1_offset));
				term_send_crlf();
				term_send_str("Aktualni nastaveni rotoru 2: ");
				term_send_char(TOCHAR(rotor_2_offset));
				term_send_crlf();
				term_send_str("Aktualni nastaveni rotoru 3: ");
				term_send_char(TOCHAR(rotor_3_offset));
				term_send_crlf();
				LCD_clear();
				LCD_append_string("Nastaveni rotoru:");
				LCD_append_char(TOCHAR(rotor_1_offset));
				LCD_append_char(TOCHAR(rotor_2_offset));
				LCD_append_char(TOCHAR(rotor_3_offset));
				break;
			}
			case 'A':{
				rotor_set = 1;
				term_send_str("Nastavujete rotor 1\n");
				LCD_clear();
				LCD_append_string("Nastavujete rotor 1");
				break;
			}
			case 'B':{
				rotor_set = 2;
				term_send_str("Nastavujete rotor 2\n");
				LCD_clear();
				LCD_append_string("Nastavujete rotor 2");
				break;
			}
			case 'C':{
				rotor_set = 3;
				term_send_str("Nastavujete rotor 3\n");
				LCD_clear();
				LCD_append_string("Nastavujete rotor 3");
				break;
			}
			case 'D':{
				rotor_1_offset = 0;
				rotor_2_offset = 0;
				rotor_3_offset = 0;
				term_send_str("Vsechny rotory nastaveny do vychozi konfigurace (AAA)\n");
				LCD_clear();
				LCD_append_string("Rotory nastaveny na AAA");
				break;
			}
			case '2':{
				if(start == 0){
					cur_char = 0;
					start = 1;
				}else{
					cur_char++;
					if(cur_char == 3){
						term_send_str("Neplatny vstup\n");
						LCD_clear();
						LCD_append_string("Neplatny vstup");
						start = 0;
					}
				}
				break;
			}
			case '3':{
				if(start == 0){
					cur_char = 3;
					start = 1;
				}else{
					cur_char++;
					if(cur_char == 6){
						term_send_str("Neplatny vstup\n");
						LCD_clear();
						LCD_append_string("Neplatny vstup");
						start = 0;
					}
				}
				break;
			}
			case '4':{
				if(start == 0){
					cur_char = 6;
					start = 1;
				}else{
					cur_char++;
					if(cur_char == 9){
						term_send_str("Neplatny vstup\n");
						LCD_clear();
						LCD_append_string("Neplatny vstup");
						start = 0;
					}
				}
				break;
			}
			case '5':{
				if(start == 0){
					cur_char = 9;
					start = 1;
				}else{
					cur_char++;
					if(cur_char == 12){
						term_send_str("Neplatny vstup\n");
						LCD_clear();
						LCD_append_string("Neplatny vstup");
						start = 0;
					}
				}
				break;
			}
			case '6':{
				if(start == 0){
					cur_char = 12;
					start = 1;
				}else{
					cur_char++;
					if(cur_char == 15){
						term_send_str("Neplatny vstup\n");
						LCD_clear();
						LCD_append_string("Neplatny vstup");
						start = 0;
					}
				}
				break;
			}
			case '7':{
				if(start == 0){
					cur_char = 15;
					start = 1;
				}else{
					cur_char++;
					if(cur_char == 19){
						term_send_str("Neplatny vstup\n");
						LCD_clear();
						LCD_append_string("Neplatny vstup");
						start = 0;
					}
				}
				break;
			}
			case '8':{
				if(start == 0){
					cur_char = 19;
					start = 1;
				}else{
					cur_char++;
					if(cur_char == 22){
						term_send_str("Neplatny vstup\n");
						LCD_clear();
						LCD_append_string("Neplatny vstup");
						start = 0;
					}
				}
				break;
			}
			case '9':{
				if(start == 0){
					cur_char = 22;
					start = 1;
				}else{
					cur_char++;
					if(cur_char == 26){
						term_send_str("Neplatny vstup\n");
						LCD_clear();
						LCD_append_string("Neplatny vstup");
						start = 0;
					}
				}
				break;
			}
			case '#':{
				if(start == 0){
					term_send_str("Neplatny vstup\n");
					LCD_clear();
					LCD_append_string("Neplatny vstup");
				}else{
					if(rotor_set){
						if(rotor_set == 1){
							rotor_1_offset = cur_char;
							term_send_str("Rotor 1 nastaven na: ");
							term_send_char(TOCHAR(cur_char));
							term_send_crlf();
							LCD_clear();
							LCD_append_string("Rotor 1 nastaven na: ");
							LCD_append_char(TOCHAR(cur_char));
						}else if(rotor_set == 2){
							rotor_2_offset = cur_char;
							term_send_str("Rotor 2 nastaven na: ");
							term_send_char(TOCHAR(cur_char));
							term_send_crlf();
							LCD_clear();
							LCD_append_string("Rotor 2 nastaven na: ");
							LCD_append_char(TOCHAR(cur_char));
						}else if(rotor_set == 3){
							rotor_3_offset = cur_char;
							term_send_str("Rotor 3 nastaven na: ");
							term_send_char(TOCHAR(cur_char));
							term_send_crlf();
							LCD_clear();
							LCD_append_string("Rotor 3 nastaven na: ");
							LCD_append_char(TOCHAR(cur_char));
						}
						rotor_set = 0;
						start = 0;
					}else{
						term_send_str("Sifrovana hodnota: ");
						encr = TOCHAR(encrypt(cur_char));
						term_send_char(encr);
						term_send_crlf();
						LCD_clear();
						LCD_append_string("Sifrovana hodnota: ");
						LCD_append_char(encr);
						start = 0;
					}
				}
				break;
			}
			default:{
				term_send_str("Neplatny vstup\n");
				LCD_clear();
				LCD_append_string("Neplatny vstup");
				start = 0;
				break;
			}
		}
		if(start){
			term_send_str("Momentalne nacitany znak: ");
			term_send_char(cur_char + 65);
			term_send_crlf();
			LCD_clear();
			LCD_append_string("Nacitany znak: ");
			LCD_append_char(cur_char + 65);
		}
    }
  }
  return 0;
}



/*******************************************************************************
 * Dekodovani a vykonani uzivatelskych prikazu
*******************************************************************************/
unsigned char decode_user_cmd(char *cmd_ucase, char *cmd)
{
  return CMD_UNKNOWN;
}

/*******************************************************************************
 * Inicializace periferii/komponent po naprogramovani FPGA
*******************************************************************************/
void fpga_initialized()
{
  LCD_init();
  LCD_clear();
}


/*******************************************************************************
 * Hlavni funkce
*******************************************************************************/
int main(void)
{
  unsigned int cnt = 0;
  char_cnt = 0;
  last_ch = 0;
  start = 0;
  rotor_set = 0;
  rotor_1_offset = 0;
  rotor_2_offset = 0;
  rotor_3_offset = 0;
  
  initialize_hardware();
  keyboard_init();

  set_led_d6(1);                       // rozsviceni D6
  set_led_d5(1);                       // rozsviceni D5

  while (1)
  {
    delay_ms(10);
    cnt++;
    if (cnt > 50)
    {
      cnt = 0;
      flip_led_d6();                   // negace portu na ktere je LED
    }

    keyboard_idle();                   // obsluha klavesnice
    terminal_idle();                   // obsluha terminalu
  }         
}
