/**
 * Projekt 1, IPK
 * @name webclient
 * @author Petr Flajsingr, xflajs00
 **/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <ctype.h>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

/**
 * Struktura pro ulozeni nformaci z URL
 **/
typedef struct URL{
	int port; //cislo portu
	string path; //cesta
	string host; //nazev hosta
	string filename; //nazev souboru
}URLInfo;

/**
 * Tisk napovedy
 **/
void printHelp(){
	cout << "Projekt 1: Webovy klient" << endl << "Program stahne a ulozi objekty zadany pomoci URL" << endl;
	cout << "Pouziti:" << endl << "webclient URL\t - \tStahne stranku a ulozi ji do index.html" << endl;
}

/**
 * Zpracovani URL
 * @param *argv
 * url predane v parametru
 * @param *info
 * struktura pro ulozeni dat
 **/
int getURL(char *argv, URLInfo *info){
	string url = argv; //ulozeni c_str jako c++string
	size_t temp;
	//kontrola http
	if((temp = url.find("http://")) != string::npos){
		url.erase(temp, 7);
	}else{
		return -1;
	}

	size_t port = url.find(":");
	size_t host = url.find("/");
	//ulozeni jmena host
	info->host = url.substr(0, (port < host ? port : host));
	url.erase(0, (port < host ? port : host));

	//ulozeni portu
	port = url.find(":");
	if(port != string::npos){
		if(url.size() == 1){
			return -1;
		}
		port++;
		if(!isdigit(url[port])){
			return -1;
		}
		info->port = 0;
		vector<int> num;
		while(isdigit(url[port])){
			num.push_back(url[port] - '0');
			port++;
		}
		url.erase(0, port);
		int mul = 1;
		for(size_t i = num.size(); i > 0; i--){
			info->port += mul * num[i - 1];
			mul *= 10;
		}
	}

	//ulozeni cesty
	if(url.size() > 0){
		info->path = url;
	}
	//kontrola mezer
	while((temp = info->path.find(" ")) != string::npos){
		info->path.replace(temp, 1, "%20");
	}
	//kontrola otazniku
	while((temp = info->path.find("?")) != string::npos){
		info->path.replace(temp, 1, "%3F");
	}
	//kontrola tildy
	while((temp = info->path.find("~")) != string::npos){
		info->path.replace(temp, 1, "%7E");
	}
	//kontrola vykricniku
	while((temp = info->path.find("!")) != string::npos){
		info->path.replace(temp, 1, "%21");
	}
	//kontrola hash
	while((temp = info->path.find("#")) != string::npos){
		info->path.replace(temp, 1, "%23");
	}
	//ulozeni jmena souboru
	if((temp = info->path.rfind("/")) != string::npos){
		if(info->path.length() - 1 != temp){
			info->filename = info->path.substr(temp + 1);
			while((temp = info->filename.find("%20")) != string::npos){
				info->filename.replace(temp, 3, " ");
			}
			while((temp = info->filename.find("%3F")) != string::npos){
				info->filename.replace(temp, 3, "?");
			}
			while((temp = info->filename.find("%7E")) != string::npos){
				info->filename.replace(temp, 3, "~");
			}
			while((temp = info->filename.find("%21")) != string::npos){
				info->filename.replace(temp, 3, "!");
			}
			while((temp = info->filename.find("%23")) != string::npos){
				info->filename.replace(temp, 3, "#");
			}
		}else{
			info->filename = "index.html";
		}
	}else{
		info->filename = "index.html";
	}

	return 0;
}

/**
 * Kontrola parametru
 * @param argc
 * pocet argumentu
 * @param **argv
 * argumenty
 * @param *info
 * struktura pro ulozeni dat
 **/
int param(int argc, char **argv, URLInfo *info){
	if(argc == 2 && !strcmp(argv[1], "--help")){ //vypis napovedy
		printHelp();
	}else if(int temp = getURL(argv[1], info)){ //ulozeni URL
			return temp;
	}
	return 0;
}

/**
 * Spojeni s hostem
 * @param *sock
 * socket pro spojeni
 * @param info
 * informace o url
 **/
int myConnect(int *sock, URLInfo info){
	struct sockaddr_in saddr;
	struct hostent *hstn;
	if((*sock = socket(PF_INET, SOCK_STREAM, 0)) < 0){ //otevreni socketu
		return -1;
	}

	saddr.sin_family = PF_INET;
	saddr.sin_port = htons(info.port);

	if(!(hstn = gethostbyname(info.host.c_str()))){
		return -1;
	}

	memcpy(&saddr.sin_addr, hstn->h_addr, hstn->h_length);

	if(connect(*sock, (struct sockaddr *)&saddr, sizeof(saddr)) < 0){
		return -1;
	}
	return 0;
}

/**
 * Odeslani zpravy
 * @param info
 * info o url
 * @param sock
 * socket pro spojeni
 * @param *response
 * odpoved serveru
 **/
int mySend(URLInfo info, int sock, string *response){
	string msg(
		"GET " + info.path + " HTTP/1.1\r\n"
		"Host: " + info.host + "\r\n"
		"Connection: close \r\n\r\n"
	);

	//odeslani zpravy
	if(send(sock, msg.c_str(), msg.size(), 0) < 0){
		return -1;
	}
	char buffer[1024]; //buffer pro nacitani odpovedi
	int a = 0; //counter pro velikost zpravy
	(*response).clear(); //vycisteni response (kvuli presmerovani)
	while((a = recv(sock, buffer, 1024, 0)) > 0){ //ulozeni odpovedi
		(*response).append(buffer, a);
	}
	string head = (*response).substr(0, 12); //izolace hlavicky
	//zpracovani odpovedi serveru
	if(head.find("200") != string::npos){
		return 200;
	}else if(head.find("301") != string::npos){
		return 301;
	}else if(head.find("302") != string::npos){
		return 302;
	}else if((head.find("404") != string::npos) || (head.find("505") != string::npos) || (head.find("400") != string::npos)){ //nerozeznany protokol
		msg = (
		"GET " + info.path + " HTTP/1.0\r\n"
		"Host: " + info.host + "\r\n"
		"Connection: close \r\n\r\n"
		);
		if(send(sock, msg.c_str(), msg.size(), 0) < 0){
			return -1;
		}
		int a = 0;
		(*response).clear();
		while((a = recv(sock, buffer, 1024, 0)) > 0){
			(*response).append(buffer, a);
		}
		if(head.find("200") != string::npos){
			return 200;
		}else if(head.find("301") != string::npos){
			return 301;
		}else if(head.find("302") != string::npos){
			return 302;
		}
	}
	return -1;
}

/**
 * Uprava odpovedi pokud je chunked
 * @param *response
 * odpoved serveru
 **/
void changeChunked(string *response){
	long int intTemp = -1; //pro ulozeni hex hodnoty
	string newResponse; //upravena odpoved
	string toConvert;
	size_t temp;
	while(intTemp != 0){ //do konce souboru
		if((temp = (*response).find("\r\n")) != string::npos){
			toConvert = ((*response).substr(0, temp));
			toConvert.append("\0");
			intTemp = strtol(toConvert.c_str(), NULL, 16);
			(*response).erase(0, temp + 2);
			newResponse.append((*response), 0, intTemp);
			(*response).erase(0, intTemp + 2);				
		}
	}
	(*response) = newResponse;
}

/**
 * Ulozeni odpovedi do souboru
 * @param *response
 * odpoved serveru
 * @param info
 * informace o URL
 **/
int writeToFile(string *response, URLInfo info){
	size_t temp;
	//kontrola jestli je soubor chunked
	bool chunked = false;
	chunked = ((*response).find("Transfer-Encoding: chunked\r\n") != string::npos); 
	//smazani hlavicky
	if((temp = (*response).find("\r\n\r\n")) != string::npos){
		(*response).erase(0, temp + 4);
	}

	if(chunked){
		changeChunked(response);
	}
	ofstream Out; //stream pro vystup
	Out.open(info.filename.c_str(), ios::out | ios::binary); //otevreni streamu
	if(Out.fail()){ //chyba otevreni
		return -1;
	}
	//vypis do souboru a zavreni streamu
	Out.write((*response).c_str(), (*response).length());
	Out.close();
	return 0;
}

/**
 * Presmerovani stranky
 * @param status
 * typ presmerovani
 * @param *response
 * odpoved serveru
 * @param info
 * info o URL
 * @param *sock
 * socket
 * @param n
 * counter rekurze
 **/
int redir(int status, string *response, URLInfo info, int *sock, int n){
	if(status == 200){ //zapis do souboru
		return writeToFile(response, info);
	}else if((status == 301 || status == 302) && n > 0){
		size_t temp = (*response).find("Location:"); //nalezeni noveho URL
		/*** Ulozeni noveho URL ***/
		(*response).erase(0, temp + 10); 
		temp = (*response).find("\n");
		(*response).erase(temp);
		(*response).erase((*response).length() - 1); //kombo protoze eva.fit.vutbr.cz jede na c++15B.C.
		char *url = (char*)(*response).c_str();
		/******/
		//opetovne kontaktovani serveru
		int temp2;
		if((temp2 = getURL(url, &info))){
			return temp2;
		}
		if((temp2 = myConnect(sock, info))){
			return temp2;
		}
		if((temp2 = mySend(info, *sock, response)) == -1){
			return temp2;
		}
		if((temp2 = redir(temp2, response, info, sock, n - 1))){
			return temp2;
		}
	}
	return 0;
}

/** 
 * Main
 **/
int main(int argc, char **argv){
	URLInfo info = {80, "/", "", "index.html"};
	int temp = 0;
	//zpracovani argumentu
	if((temp = param(argc, argv, &info))){
		cerr << "Chyba parametru" << endl;
		return temp;
	}
	//otevreni spojeni
	int sock;
	if((temp = myConnect(&sock, info))){
		cerr << "Chyba spojeni" << endl;
		return temp;
	}
	//komunikace
	string response;
	if((temp = mySend(info, sock, &response)) == -1){
		cerr << "Chyba v komunikaci" << endl;
		return temp;
	}
	//vypis, pripadny redirect
	if((temp = redir(temp, &response, info, &sock, 5))){
		cerr << "Chyba souboru" << endl;
		return temp;
	}
	return 0;
}