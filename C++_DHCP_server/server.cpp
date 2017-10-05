/**
 * Projekt 2, IPK
 * @name server
 * @author Petr Flajsingr, xflajs00
 * program zastavajici roli serveru pro zpracovani pozadavku klienta
 **/

#include <stdio.h>
#include <cstdlib>
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
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "server.hh"
#include "ipk2.hh"

using namespace std;

int main(int argc, char** argv){
	errorCode err;
	string port;
	//kontrola a zpracovani parametru
	if((err = param(argc, argv, &port)) != E_OK){

		if(err == E_OK_DONE){
			return 0;
		}
		printError(err);
		return err;
	}
	//hlavni funkce serveru
	if((err = serverMain(port)) != E_OK){
		printError(err);
		return err;
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
errorCode param(int argc, char **argv, string* port){
	if(argc == 2 && !strcmp(argv[1], "--help")){ //vypis napovedy
		printHelp();
		return E_OK_DONE;
	}

	if(argc != 3){
		return E_ARG_ERROR;
	}

	if(!strcmp(argv[1], "-p")){
		(*port).assign(argv[2], argv[2] + strlen(argv[2]));
	}else{
		return E_ARG_ERROR;
	}
	return E_OK;
}

/**
 * Tisk napovedy
 **/
void printHelp(){
	cout << "Projekt 2: Prenos souboru" << endl << "Server pro prenos souboru s klientem" << endl;
	cout << "Pouziti:" << endl << endl;
	cout << "--help\t\t\t-\tvypis napovedy" << endl;
	cout << "-p [cislo portu]\t-\turceni pouziteho portu" << endl;
}

/**
 * Hlavni funkce serveru
 **/
errorCode serverMain(string port){
	int sock, child;

	struct addrinfo hints, *res;
	struct sockaddr_storage clientAddr;

	socklen_t sin_size;

	char buffer[1024];
	string response;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_INET;
	hints.ai_socktype = SOCK_STREAM;
	
	//informace pro bind
	if(getaddrinfo(NULL, port.c_str(), &hints, &res)){
		return E_ADDINFO_ERROR;
	}

	//otevreni socketu
	int setOption = 1;
	if((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1){
		return E_SOCKET_ERROR;
	}

	//reuse socketu
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &setOption, sizeof(setOption)) == -1){
		close(sock);
		return E_SETSOCK_ERROR;
	}

	//prirazeni jmena k socketu
	if(bind(sock, res->ai_addr, res->ai_addrlen) == -1){
		close(sock);
		return E_BIND_ERROR;
	}


	freeaddrinfo(res);

	//nastaveni pro accept
	if(listen(sock, 10) == -1){
		return E_LISTEN_ERROR;
	}

	//cyklus pro cekani na spojeni
	while(1){
		sin_size = sizeof clientAddr;
		//kontrola spojeni
		child = accept(sock, (struct sockaddr *)&clientAddr, &sin_size);

		if(child == -1){ //bez spojeni
			continue;
		}

		//child proces pro nove spojeni
		if(!fork()){
			close(sock);

			int a = 0;
			//ulozeni prichozich dat
			while(true){
				a = recv(child, buffer, 1024, 0);
				if(a == -1){
					return E_RECV_ERR;
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
			//zpracovani prichozich dat
			string fileName;
			comType mode;
			if(response.find("FILENAME: ") != string::npos){ //ulozeni nazvu souboru
				a = response.find("\r\n\r\n");
				fileName = response.substr(10, a - 10);
			}else{
				return E_MSG_ERROR;
			}
			if((size_t)(a = response.find("MODE: ")) != string::npos){ //typ prenosu
				if(response.substr(a + 6) == "DOWNLOAD"){
					mode = M_DOWNLOAD;
				}else if(response.substr(a + 6) == "UPLOAD"){
					mode = M_UPLOAD;
				}else{
					return E_MSG_ERROR;
				}
			}
			//zprava pro potvrzeni
			string msg;
			if(mode == M_UPLOAD){
				msg = "OK\r\n\r\nSENDDATA\r\n\r\n\r\n";
				if(send(child, msg.c_str(), msg.size(), 0) < 0){
					return E_SEND_ERR;
				}
			}

			string temp;
			errorCode err;
			//posilani dat klientovi
			if(mode == M_DOWNLOAD){
				if((err = uploadData(fileName, child)) != E_OK){
					return err;
				}
			}
			//prijimani dat od klienta
			if(mode == M_UPLOAD){
				cout << "Receiving file..." << endl;
				if((err = downloadData(fileName, child)) != E_OK){
					return err;
				}
				cout << "File received..." << endl;
			}
			close(child);
			exit(0);
		}
		close(child);
	}
	return E_OK;
}