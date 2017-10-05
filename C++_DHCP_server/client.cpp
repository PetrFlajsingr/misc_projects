/**
 * Projekt 2, IPK
 * @name client
 * @author Petr Flajsingr, xflajs00
 * klient ke stahovani a odesilani dat na server
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
#include "client.hh"
#include "ipk2.hh"
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;


int main(int argc, char** argv){
	conInfo* info = new conInfo;
	errorCode err;
	//kontrola a nacteni argumentu
	if((err = param(argc, argv, info)) != E_OK){
		if(err == E_OK_DONE){
			return 0;
		}
		printError(err);
		return err;
	}
	//vytvoreni spojeni
	int sock;
	if((err = myConnect(&sock, info)) != E_OK){
		printError(err);
		return err;
	}
	//komunikace
	if((err = mySend(sock, info->mode, info->fileName)) != E_OK){
		printError(err);
		return err;
	}
	return 0;
}



/**
 * Tisk napovedy
 **/
void printHelp(){
	cout << "Projekt 2: Prenos souboru" << endl << "Program pro prenos souboru se serverem" << endl;
	cout << "Pouziti:" << endl << endl;
	cout << "--help\t\t\t-\tvypis napovedy" << endl;
	cout << "-p [cislo portu]\t-\turceni pouziteho portu" << endl;
	cout << "-h [nazev hosta]\t-\turceni hosta" << endl;
	cout << "-u [soubor]\t\t-\tupload souboru" << endl;
	cout << "-d [soubor]\t\t-\tdownload souboru" << endl;
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
errorCode param(int argc, char **argv, conInfo* info){
	if(argc == 2 && !strcmp(argv[1], "--help")){ //vypis napovedy
		printHelp();
		return E_OK_DONE;
	}

	if(argc != 7){
		return E_ARG_ERROR;
	}
	bool setH = false, setP = false, setUD = false;
	for(int i = 1; i < argc; i += 2){
		if(!strcmp(argv[i], "-h")){
			if(setH){
				return E_ARG_ERROR;
			}
			setH = true;
			info->host.assign(argv[i+1], strlen(argv[i+1]));
		}else if(!strcmp(argv[i], "-p")){
			if(setP){
				return E_ARG_ERROR;
			}
			setP = true;
			info->port = atoi(argv[i+1]);
		}else if(!strcmp(argv[i], "-u") || !strcmp(argv[i], "-d")){
			if(setUD){
				return E_ARG_ERROR;
			}
			setUD = true;
			if(!strcmp(argv[i], "-u")){
				info->mode = M_UPLOAD;
			}else{
				info->mode = M_DOWNLOAD;
			}
			info->fileName.assign(argv[i+1], strlen(argv[i+1]));
		}else{
			return E_ARG_ERROR;
		}
	}
	return E_OK;
}

/**
 * Spojeni s hostem
 * @param *sock
 * socket pro spojeni
 * @param info
 * informace o spojeni
 **/
errorCode myConnect(int* sock, conInfo* info){
	struct sockaddr_in saddr;
	struct hostent *hstn;
	if((*sock = socket(PF_INET, SOCK_STREAM, 0)) < 0){ //otevreni socketu
		return E_SOCKET_ERROR;
	}

	saddr.sin_family = PF_INET;
	saddr.sin_port = htons(info->port);

	if(!(hstn = gethostbyname(info->host.c_str()))){
		return E_HOST_ERROR;
	}

	memcpy(&saddr.sin_addr, hstn->h_addr, hstn->h_length);

	if(connect(*sock, (struct sockaddr *)&saddr, sizeof(saddr)) < 0){
		return E_SOCKET_ERROR;
	}
	return E_OK;
}

/**
 *komunikace se serverem
 *@param sock
 *socket pro spojeni
 *@param mode
 *typ spojeni (download/upload)
 *@param fileName
 *jmeno souboru
 **/
errorCode mySend(int sock, comType mode, string fileName){
	string response;
	//zprava o typu prenosu
	string msg = ("FILENAME: " + fileName + "\r\n\r\nMODE: ");
	if(mode == M_DOWNLOAD){
		msg = msg + "DOWNLOAD\r\n\r\n\r\n";
	}
	if(mode == M_UPLOAD){
		msg = msg + "UPLOAD\r\n\r\n\r\n";
	}
	if(send(sock, msg.c_str(), msg.size(), 0) < 0){
		return E_SEND_ERR;
	}

	char buffer[1024];
	int a = 0;
	string temp;
	errorCode err;
	//stazeni souboru ze serveru
	if(mode == M_DOWNLOAD){
		if((err = downloadData(fileName, sock)) != E_OK){
			return err;
		}
	}
	//upload na server
	if(mode == M_UPLOAD){
		//kontrola odpovedi
		while(true){
			a = recv(sock, buffer, 1024, 0);
			if(a == -1){
				return E_SEND_ERR;
			}
			if(a == 0){
				break;
			}
			response.append(buffer, a);
			if((size_t)(a = response.find("\r\n\r\n\r\n")) != string::npos){
				response.erase(a);
				break;
			}
		}
		if((err = uploadData(fileName, sock)) != E_OK){
			return err;
		}
	}
	close(sock);
	return E_OK;
}

