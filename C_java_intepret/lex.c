/** 
 * @file lex.c
 * @author Dominik Dvorak(xdvora1t)
 * @date 27 September 2016
 * @brief Sobor obsahujici zdrojovy kod lexikalniho analyzatoru
 **/

#include "lex.h"

char **arg;

int file_cursor;

/**
 * @brief Funkce pro vraceni prvniho nalezeneho tokenu
 * @param sourceFile Zdrojovy kod
 * @param tokken Ukazatel na token
 **/

/* !!
		// 3. parametr volat jako false
											!! */
 int GetNewToken(FILE *sourceFile, Tok_T *tokken, bool next){
	char buffer[5000];
	char identifier_2_buffer[5000];
	static int buffer_position = 0;
	static int buffer_position_second = 1;
	static int state = 0;
	static int character = 0;
	static int octa_counter;
	int new_file_cursor = 0;
	static int bitecounter = 0;
	static bool char_read = false;
	bool end_of_token = false;
	static bool start = false;
	static bool underscore = false;
	int keyword = 0;

	//Loop dokud se nenajde konec tokenu nebo neni konec souboru
	while(!end_of_token && !feof(sourceFile)){
		if(!char_read){
			character = fgetc(sourceFile);
			if(next == false){
				file_cursor++;
			}else{
				new_file_cursor++;
			}
			if(character == EOF){
				EndOfToken(&end_of_token, "", tokken, EOF_T, 0, sourceFile, next, new_file_cursor);
				return(0);
			}
		}
		char_read = false;
		if(state != BLOCK_COMMENT){
			buffer[buffer_position] = character;
		}
		switch(state){
			//Pocatecni stav, rozdeleni podle nacteneho znaku v dalsim loopu
			case START:{
				GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				//Stavy ktere se v automatu posouvaji dale
				if(character == '/'){
					state = COMMENT;
				}else if(character == '\''){
					state = T_CHAR;
				}else if(character == '0'){
					state = T_INT_BASE;
				}else if(((character >= 'A') && (character <= 'z')) || (character == '_') || (character == '$')){
					state = IDENTIFIER;
				}else if((character > '0') && (character <= '9')){
					state = T_INT;
				}else if(character == '"'){
					state = T_STRING;
				}else if(character == '='){
					state = ASSIGN;
				}else if(character == '+'){
					state = PLUS;
				}else if(character == '-'){
					state = MINUS;
				}else if(character == '.'){
					state = T_DOUBLE_AFTER_DOT;
				}else if(character == '<'){
					state = LESS;
				}else if(character == '>'){
					state = GREATER;
				}else if(character == '!'){
					state = NEG;
				//Stavy ktere hned posilaji jako tokkeny
				}else if(character == ','){
					state = COMMA;
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					tokken->syntax_character[0] = ',';
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}else if(character == '*'){
					state = MUL;
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					tokken->syntax_character[0] = '*';
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}else if(character == ')'){
					state = RIGHT_BRACKET;
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					tokken->syntax_character[0] = ')';
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}else if(character == '('){
					state = LEFT_BRACKET;
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					tokken->syntax_character[0] = '(';
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}else if(character == '{'){
					state = LEFT_DOUBLE_BRACKET;
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					tokken->syntax_character[0] = '{';
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}else if(character == '}'){
					state = RIGHT_DOUBLE_BRACKET;
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					tokken->syntax_character[0] = '}';
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}else if(character == ';'){
					state = SEMICOLON;
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					tokken->syntax_character[0] = ';';
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}else if(character == ','){
					state = COLON;
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					tokken->syntax_character[0] = ',';
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}
				buffer[buffer_position] = character;
				start = false;
				break;
			}
			//Komentar - dale se deli na line/block
			case COMMENT:{
				if(character == '/'){
					state = LINE_COMMENT;
				}else if(character == '*'){
					state = BLOCK_COMMENT;
				}else{
					char_read = true;
					state = DIV;
					buffer[buffer_position] = '\0';
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}
				break;
			}
			case LINE_COMMENT:{
				if(character == '\n'){
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}
				break;
			}
			case BLOCK_COMMENT:{
				if(character == '*'){
					state = END_BLOCK_COMMENT;
				}
				break;
			}
			//Konec komentare
			case END_BLOCK_COMMENT:{
				if(character == '/'){
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}else if(character == '*'){
					state = END_BLOCK_COMMENT;
				}else{
					state = BLOCK_COMMENT;
				}
				break;
			}
			//Identifikator
			case IDENTIFIER:{
				if(((character >= '0') && (character <= '9')) ||
				((character >= 'A') && (character <= 'z')) ||
				(character == '_') || (character == '$')){
					state = IDENTIFIER;
				}else if(character == '.'){
					state = IDENTIFIER_2_START;
					memset(identifier_2_buffer, '\0', 5000);
				}else{
					keyword = checkKeyword(buffer);
					char_read = true;
					EndOfToken(&end_of_token, buffer, tokken, state, keyword, sourceFile, next, new_file_cursor);
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}
				break;
			}
			//Mozny zacatek id cislo 2 - blabla.blabla
			case IDENTIFIER_2_START:{
				identifier_2_buffer[0] = character;
				if(((character >= 'A') && (character <= 'z')) ||
				(character == '_') || (character == '$')){
					state = IDENTIFIER_2;
				}else{
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}
				break;
			}
			//Identifikator cislo 2
			case IDENTIFIER_2:{
				if(((character >= '0') && (character <= '9')) ||
				((character >= 'A') && (character <= 'z')) ||
				(character == '_') || (character == '$')){
					state = IDENTIFIER_2;
					identifier_2_buffer[buffer_position_second] = character;
				}else{
					keyword = checkKeyword(buffer);
					char_read = true;
					buffer_position_second = 0;
					EndOfToken(&end_of_token, buffer, tokken, state, keyword, sourceFile, next, new_file_cursor);
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}
				buffer_position_second++;
				break;
			}
			//Datovy typ - integer
			case T_INT:{
				if(character == '.'){
					state = T_DOUBLE;
				}else if((character == 'e') || (character == 'E')){
					state = T_DOUBLE_EXP_SIGN;
				}else{
					if(!isdigit(character)){
						char_read = true;
						EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
						GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
					}
				}
				break;
			}
			//Rozsireni base
			case T_INT_BASE:{
				if((character >= '0') && (character <= '7')){
					bitecounter++;
					state = T_INT_OCTA;
				}else if(character == 'b'){
					state = T_INT_BIN;
				}else if(character == 'x'){
					state = T_INT_HEXA;
				}else if(character == '.'){
					state = T_DOUBLE;
				}else{
					char_read = true;
					state = T_INT;
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}
				break;
			}
			//Rozsireni base - octa
			case T_INT_OCTA:{
				if(underscore){
					if((character <= '0') || (character > '7')){
						return(1);
					}else{
						underscore = false;
					}
				}
				if((character >= '0') && (character <= '7')){
					//return 1;
				}else if(character == '_'){
					underscore = true;
				}else if(!isdigit(character) && (checkEndOfNumber(character, sourceFile)) != false){
					char_read = true;
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}else{
					return(1);
				}
				break;
			}
			//Rozsireni base - bin
			case T_INT_BIN:{
				if(underscore){
					if((character != '0') && (character != '1')){
						return(1);
					}else{
						underscore = false;
					}
				}
				if((character == '0') || (character == '1')){

				}else if(character == '_'){
					underscore = true;
				}else if(!isdigit(character) && (checkEndOfNumber(character, sourceFile)) != false){
					char_read = true;
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}else{
					return(1);
				}
				break;
			}
			//Rozsireni base - hexa
			case T_INT_HEXA:{
				if(underscore){
					if(((character >= '0') && (character <= '9')) || ((character >= 'A') && (character <= 'F'))){
						underscore = false;
					}else{
						return 1;
					}
				}
				if(((character >= '0') && (character <= '9')) || ((character >= 'A') && (character <= 'F'))){

				}else if(character == '.'){
					state = T_INT_HEXA_FL;
				}else if(character == '_'){
					underscore = true;
				}else if(!isdigit(character) && (checkEndOfNumber(character, sourceFile))){
					char_read = true;
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}else{
					return(1);
				}
				break;
			}
			case T_INT_HEXA_FL:{
				if(underscore){
					if(((character >= '0') && (character <= '9')) || ((character >= 'A') && (character <= 'F'))){
						underscore = false;
					}else{
						return 1;
					}
				}
				if(((character >= '0') && (character <= '9')) || ((character >= 'A') && (character <= 'F'))){

				}else if(character == '_'){
					underscore = true;
				}else if(character == 'p' || character == 'P'){
					state = T_INT_HEXA_EXP;
				}else{
					return(1);
				}
				break;
			}
			case T_INT_HEXA_EXP:{
				if(character == '-'){
					underscore = true;
				}else if(underscore && isdigit(character)){

				}else if(underscore && !isdigit(character) && (checkEndOfNumber(character, sourceFile))){
					char_read = true;
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}else{
					return(1);
				}
				break;
			}
			//Datovy typ - double
			case T_DOUBLE:{
				if((character == 'e') || (character == 'E')){
					state = T_DOUBLE_EXP_SIGN;
				}else{
					if(character == '.'){
						return 1;
					}else if(!isdigit(character)){
						char_read = true;
						EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
						GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
					}
				}
				break;
			}
			//Datovy typ - double + exponent - znamenko
			case T_DOUBLE_EXP_SIGN:{
				if(isdigit(character) || (character == '+') || (character == '-')){
					state = T_DOUBLE_EXP;
				}else{
					return(1);
				}
				break;
			}
			//Datovy typ - double + exponent - hodnota exponentu
			case T_DOUBLE_EXP:{
				if(character == '.'){
						return 1;
				}else if(!isdigit(character)){
					char_read = true;
					EndOfToken(&end_of_token, buffer, tokken, T_DOUBLE, 0, sourceFile, next, new_file_cursor);
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}
				break;
			}
			//Datovy typ - double, nejdriv tecka a musi nasledovat cislo
			case T_DOUBLE_AFTER_DOT:{
				if(isdigit(character)){
					state = T_DOUBLE;
				}else{
					return(1);
				}
				break;
			}
			//Datovy typ - char
			case T_CHAR:{
				if(character == '\\'){
					state = T_CHAR_ESC;
				}
				else if((character >= 0) && (character <= 255)){
					character = fgetc(sourceFile);
					if(character == '\''){
						buffer_position++;
						buffer[buffer_position] = character;
						EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
						GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
					}else{
						return(1);
					}
				}else{
					return(1);
				}
				break;
			}
			case T_CHAR_ESC:{
				//Validita prvniho znaku
				if((character == 'a') || (character == 'n') || (character == 't')
					|| (character == '\\') || (character == '\'')){
					character = fgetc(sourceFile);
					if(character == '\''){
						buffer_position++;
						buffer[buffer_position] = character;
						EndOfToken(&end_of_token, buffer, tokken, T_CHAR_ESC, 0, sourceFile, next, new_file_cursor);
						GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
					}else{
						return(1);
					}
				//Octa escape
				}else if((character >= '0') && (character <= '3')){
					state = T_CHAR_ESC_OCTA;
					break;
				}else{
					return(1);
				}
				break;				
			}
			//Datovy typ - char - octa cislo
			case T_CHAR_ESC_OCTA:{
				octa_counter++;
				if(octa_counter <= 2){
					;
				}else{
					char octa_buffer[3];
					memset(octa_buffer, '\0', 3);
					for(int i = 0; i < 3; i++){
						octa_buffer[i] = buffer[i + 2];
					}
					int number = strtol(octa_buffer, NULL, 10);
					if((number >= 1) && (number <= 377) && (buffer[5] == '\'')){
						EndOfToken(&end_of_token, buffer, tokken, T_CHAR_ESC_OCTA, 0, sourceFile, next, new_file_cursor);
						GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
					}else{
						return(1);
					}
				}
				break;
			}
			//Datovy typ - string
			case T_STRING:{
				if(character == '\\'){
					state = T_STRING_ESC;
				}else if(character == '"'){
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}else if(character == '\n'){
					return(1);
				}
				break;
			}
			//Datovy typ - string - esc sekvence + kontrola validity takove sekvence (a; n; t, par dalsich znaku, 001 - 377)
			case T_STRING_ESC:{
				if(character == '\n'){
					buffer[strlen(buffer) - 1] = '\0';
					buffer_position -= 2;
					state = T_STRING;
					break;
				}
				
				//Validita prvniho znaku
				if((character == 'a') || (character == 'n') || (character == 't')
					|| (character == '\\') || (character == '"') || (character == '\'')){
					state = T_STRING;
				//Octa escape
				}else if((character >= '0') && (character <= '3')){
					state = T_STRING_ESC_OCTA;
					break;
				}else{
					return(1);
				}
				break;
			}
			//Datovy typ - string - octa cislo
			case T_STRING_ESC_OCTA:{
				octa_counter++;
				if(octa_counter <= 1){
					;
				}else{
					char octa_buffer[3];
					memset(octa_buffer, '\0', 3);
					for(int i = 0; i < 3; i++){
						octa_buffer[i] = buffer[i + buffer_position - 2];
					}
					int number = strtol(octa_buffer, NULL, 10);
					if((number >= 1) && (number <= 377)){
						state = T_STRING;
					}else{
						return(1);
					}
				}
				break;
			}
			//Rovnitko nebo porovnani
			case ASSIGN:{
				if(character == '='){
					state = EQUALS;
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}else{
					if(character != '\n'){
						char_read = true;
					}
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}
				GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				break;
			}
			//Scitani nebo +1
			case PLUS:{
				if(character == '+'){
					state = PLUS_PLUS;
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}else{
					if(character != '\n'){
						char_read = true;
					}
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}
				break;
			}
			//Odecitani nebo 1
			case MINUS:{
				if(character == '-'){
					state = MINUS_MINUS;
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}else{
					if(character != '\n'){
						char_read = true;
					}
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}
				break;
			}
			//Leva ostra zavorka nebo mensi rovno
			case LESS:{
				if(character == '='){
					state = LESS_EQUAL;
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}else{
					if(character != '\n'){
						char_read = true;
					}
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}
				break;
			}
			//Prava ostra zavorka nebo vetsi rovno
			case GREATER:{
				if(character == '='){
					state = GREATER_EQUAL;
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}else{
					if(character != '\n'){
						char_read = true;
					}
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}
				break;
			}
			//Negace nebo nerovnost
			case NEG:{
				if(character == '='){
					state = NOT_EQUAL;
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}else{
					if(character != '\n'){
						char_read = true;
					}
					EndOfToken(&end_of_token, buffer, tokken, state, 0, sourceFile, next, new_file_cursor);
					GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
				}
				break;
			}
			//?? Jak ??
			default:{
				return(1);
			}
		}
		//Pokud se nepreskakuje - bile znaky - tak pocitadlo ++, jinak jedeme znovu
		if(!start){
			buffer_position++;
		}else{
			GoToStart(buffer, &buffer_position, &state, &start, &octa_counter);
		}
	}
	return(0);
}

/**
 * @brief Funkce pro kontrolu klicovych slov
 * @param keyword Kontrolovany identifikĂĆ’Ă‚Âˇtor
 * @param second Prepinac pro druhy identifikator
 **/
 int checkKeyword(char *keyword){
	char keyword_buffer[5000];
	strcpy(keyword_buffer, keyword);
	keyword_buffer[strlen(keyword_buffer) - 1] = '\0';
	if(strcmp(keyword_buffer, "boolean") == 0){
		return(BOOLEAN);
	}else if(strcmp(keyword_buffer, "break") == 0){
		return(BREAK);
	}else if(strcmp(keyword_buffer, "class") == 0){
		return(CLASS);
	}else if(strcmp(keyword_buffer, "continue") == 0){
		return(CONTINUE);
	}else if(strcmp(keyword_buffer, "do") == 0){
		return(DO);
	}else if(strcmp(keyword_buffer, "double") == 0){
		return(DOUBLE);
	}else if(strcmp(keyword_buffer, "else") == 0){
		return(ELSE);
	}else if(strcmp(keyword_buffer, "false") == 0){
		return(FALSE);
	}else if(strcmp(keyword_buffer, "for") == 0){
		return(FOR);
	}else if(strcmp(keyword_buffer, "if") == 0){
		return(IF);
	}else if(strcmp(keyword_buffer, "int") == 0){
		return(INT);
	}else if(strcmp(keyword_buffer, "return") == 0){
		return(RETURN);
	}else if(strcmp(keyword_buffer, "String") == 0){
		return(STRING);
	}else if(strcmp(keyword_buffer, "static") == 0){
		return(STATIC);
	}else if(strcmp(keyword_buffer, "true") == 0){
		return(TRUE);
	}else if(strcmp(keyword_buffer, "void") == 0){
		return(VOID);
	}else if(strcmp(keyword_buffer, "while") == 0){
		return(WHILE);
	}else if(strcmp(keyword_buffer, "ifj16.readInt") == 0){
		return(READINT);
	}else if(strcmp(keyword_buffer, "ifj16.readDouble") == 0){
		return(READDOUBLE);
	}else if(strcmp(keyword_buffer, "ifj16.readString") == 0){
		return(READSTRING);
	}else if(strcmp(keyword_buffer, "ifj16.print") == 0){
		return(PRINT);
	}else if(strcmp(keyword_buffer, "ifj16.length") == 0){
		return(LENGTH);
	}else if(strcmp(keyword_buffer, "ifj16.substr") == 0){
		return(SUBSTR);
	}else if(strcmp(keyword_buffer, "ifj16.compare") == 0){
		return(COMPARE);
	}else if(strcmp(keyword_buffer, "ifj16.find") == 0){
		return(FIND);
	}else if(strcmp(keyword_buffer, "ifj16.sort") == 0){
		return(SORT);
	}
	else{
		return(0);
	}
}

/**
 * @brief Funkce pro zacatek noveho loopu - nenasel se tokken
 * @param buffer NechĂĆ’Ă‚Âˇpu, jak jsi ted pĂâ€¦Ă˘â€žÂ˘iĂâ€¦Ă‚ÂˇChar * buffer pro tokken
 * @param position Nastaveni pozice 0
 * @param state Nastaveni stavu na START
 * @param start Nastaveni prepinace start na true - novy loop
 **/
 void GoToStart(char buffer[], __attribute__((unused)) int *position,
				__attribute__((unused)) int *state, __attribute__((unused)) bool *start, int *octa_counter){
	memset(buffer, '\0', 5000);
	*position = 0;
	*state = START;
	*start = true;
	*octa_counter = 0;
}

/**
 * @brief Funkce pro ukonceni jednoho cyklu hledani tokenu a vraceni tokkenu
 * @param end_of_token Nastaveni prepinace konce tokkenu na true
 * @param buffer Char * buffer pro tokken
 * @param line_counter Cislo radku
 * @param tokken Struktura s tokkenem
 * @param state Stav automatu
 **/
 void EndOfToken(__attribute__((unused)) bool *end_of_token, char buffer[], Tok_T *tokken, int state, int keyword, FILE *file, bool next, int new_file_cursor){
 	if(next == true){
		fseek(file, -(new_file_cursor + 1), SEEK_CUR);
	}
	NullTokken(tokken);
	*end_of_token = true;
	int compare = strlen(buffer) - 1;
	switch(state){
		case T_INT_BIN:{ 
			char* tmp = malloc(sizeof(char) * (strlen(buffer)));
			int j = 0;
			for(int i = 2; i < strlen(buffer); ++i){
				if(buffer[i] != '_'){
					tmp[j] = buffer[i];
					j++;
				}
			}
			tmp[j] = '\0';
			tokken->t_int = strtol(tmp, NULL, 2);
			state = T_INT;
			free(tmp);
			break;
		}
		case T_INT_OCTA:{
			char* tmp = malloc(sizeof(char) * (strlen(buffer)));
			int j = 0;
			for(int i = 1; i < strlen(buffer); ++i){
				if(buffer[i] != '_'){
					tmp[j] = buffer[i];
					j++;
				}
			}
			tmp[j] = '\0';
			tokken->t_int = strtol(tmp, NULL, 8);
			state = T_INT;
			free(tmp);
			break;
		}
		case T_INT_HEXA:{
			char* tmp = malloc(sizeof(char) * (strlen(buffer)));
			int j = 0;
			for(int i = 2; i < strlen(buffer); ++i){
				if(buffer[i] != '_'){
					tmp[j] = buffer[i];
					j++;
				}
			}
			tmp[j] = '\0';
			tokken->t_int = strtol(tmp, NULL, 16);
			state = T_INT;
			free(tmp);
			break;
		}
		case T_INT_HEXA_EXP:{
			char* tmp = malloc(sizeof(char) * (strlen(buffer)));
			int j = 0, i = 2;
			for(; i < strlen(buffer); ++i){
				if(buffer[i] != '_'){
					if(buffer[i] == '.'){
						i++;
						break;
					}
					tmp[j] = buffer[i];
					j++;
				}
			}
			tmp[j] = '\0';
			int dec = strtol(tmp, NULL, 16);
			j = 0;
			for(; i < strlen(buffer); ++i){
				if(buffer[i] != '_'){
					if(buffer[i] == 'p' || buffer[i] == 'P'){
						break;
					}
					tmp[j] = buffer[i];
					j++;
				}
			}i++;
			tmp[j] = '\0';
			int dec2 = strtol(tmp, NULL, 16);
			j = 0;
			for(; i < strlen(buffer); ++i){
				tmp[j] = buffer[i];
				j++;
			}
			tmp[j] = '\0';
			double dob = 1;
			for(int a = (-1 * strtol(tmp, NULL, 10)); a > 0; a--){
				dob /= 10;
			}
			tokken->t_double = dec + dec2 * dob;
			state = T_DOUBLE;
			free(tmp);
		}
		case T_INT:{
			tokken->t_int = atoi(buffer);
			break;
		}
		case T_DOUBLE_EXP:
		case T_DOUBLE:{
			tokken->t_double = strtod(buffer, NULL);
			break;
		}
		case IDENTIFIER:
		case IDENTIFIER_2:{
			for(int position = 0; position != compare; position++){
				tokken->identifier_name[position] = buffer[position];
			}
			break;
		}
		case T_CHAR:{
			tokken->t_int = buffer[1];
			state = T_INT;
			break;
		}
		case T_CHAR_ESC:{
			switch(buffer[2]){
				case 'a':{
					tokken->t_int = '\a';
					break;
				}
				case 'n':{
					tokken->t_int = '\n';
					break;
				}
				case 't':{
					tokken->t_int = '\t';
					break;
				}
				case '\\':{
					tokken->t_int = '\\';
					break;
				}
				case '\'':{
					tokken->t_int = '\'';
					break;
				}
				case '\"':{
					tokken->t_int = '\"';
					break;
				}
			}
			state = T_INT;
			break;
		}
		case T_CHAR_ESC_OCTA:{
			char octa_temp[4];
			octa_temp[0] = '0';
			octa_temp[1] = buffer[2];
			octa_temp[2] = buffer[3];
			octa_temp[3] = buffer[4];
			tokken->t_int = strtol(octa_temp, NULL, 8);
			state = T_INT;
			break;
		}
		case T_STRING:{
			memset(tokken->t_string, 0, 5000);
			int pos2 = 0;
			for(int position = 1; position != compare; position++, pos2++){
				if(buffer[position] == '\\'){
					position++;
					switch(buffer[position]){
						case 'n': tokken->t_string[pos2] = '\n'; break;
						case 't': tokken->t_string[pos2] = '\t'; break;
						case 'a': tokken->t_string[pos2] = '\a'; break;
						case '\\': tokken->t_string[pos2] = '\\'; break;
						case '\'': tokken->t_string[pos2] = '\''; break;
						case '\"': tokken->t_string[pos2] = '\"'; break;
						default: break;
					}
					char tempO[4];
					if(isdigit(buffer[position])){
						for(int i = 0; i < 3; ++i){
							tempO[i] = buffer[position];
							position++;
						}
						position--;
						tempO[3] = '\0';
						tokken->t_string[pos2] = strtol(tempO, NULL, 8);
					}
					

				}else{
					tokken->t_string[pos2] = buffer[position];
				}
			}
			break;
		}
		case ASSIGN:
		case PLUS:
		case MINUS:
		case LESS:
		case GREATER:
		case NEG:{
			tokken->syntax_character[0] = buffer[0];
			break;
		}
		default:{
			for(int position = 0; position != compare + 1; position++){
				tokken->syntax_character[position] = buffer[position];
			}
		}
	}
	if(keyword != 0){
		tokken->type = keyword;
	}else{
		tokken->type = state;
	}
}

/**
 * @brief Funkce pro vynulovani vsech hodnot v tokkenu a nastaveni radku
 * @param tokken Struktura s tokkenem
 **/
 void NullTokken(Tok_T *tokken){
	memset(tokken->syntax_character, '\0', 5000);
	memset(tokken->t_string, '\0', 5000);
	memset(tokken->identifier_name, '\0', 5000);
	tokken->t_double = 0;
	tokken->t_int = 0;
	tokken->type = 0;
}

bool checkEndOfNumber(int character, FILE *sourceFile){
	switch(character){
		case '!':
		case '=':{
			character = fgetc(sourceFile);
			if(character == '='){
				return(true);
				fseek(sourceFile, -1, SEEK_CUR);
			}else{
				return(false);
			}
		}
		case '+':
		case '-':
		case '*':
		case '/':
		case ';':
		case ')':
		case ',':
		case ' ':{
			return(true);
		}
		default:{
			return(false);
		}
	}
}
