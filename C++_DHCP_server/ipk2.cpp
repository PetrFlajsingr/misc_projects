/**
 * Projekt 2, IPK
 * @name ipk2
 * @author Petr Flajsingr, xflajs00
 * soubor obsahujici funkce vyuzite jak serverem tak klientem
 **/

#include <iostream>
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
#include <string>
#include <fstream>


using namespace std;
 /**
 * Vypis chybove hlasky
 **/
void printError(errorCode err){
	switch(err){
		case E_ARG_ERROR:
			cerr << "Chyba argumentu" << endl;
			break;
		case E_SOCKET_ERROR:
			cerr << "Chyba pri tvorbe socketu" << endl;
			break;
		case E_HOST_ERROR:
			cerr << "Chyba pri spojeni s hostem" << endl;
			break;
		case E_ADDINFO_ERROR:
			cerr << "Chyba pri getaddrinfo" << endl;
			break;
		case E_BIND_ERROR:
			cerr << "Chyba pri bind" << endl;
			break;
		case E_SETSOCK_ERROR:
			cerr << "Chyba pri setsockopt" << endl;
			break;
		case E_LISTEN_ERROR:
			cerr << "Chyba pri listen" << endl;
			break;
		case E_SIGACTION_ERR:
			cerr << "Chyba pri sigaction" << endl;
			break;
		case E_SEND_ERR:
			cerr << "Chyba pri send" << endl;
			break;
		case E_RECV_ERR:
			cerr << "Chyba pri recv" << endl;
			break;
		case E_MSG_ERROR:
			cerr << "Nerozpoznana zprava" << endl;
			break;
		case E_FILE_ERROR:
			cerr << "Chyba otevreni souboru" << endl;
			break;
		default:
			break;
	}
}

/**
 * Prijem dat na socketu
 *@param fileName
 *nazev souboru pro ulozeni dat
 *@param sock
 *socket pro komunikaci
 **/
errorCode downloadData(string fileName, int sock){
	char buffer[1024];
	int a;
	string temp;
	if((a = recv(sock, buffer, 1024, 0)) == -1){
		return E_SEND_ERR;
	}
	temp.append(buffer, a);
	if(temp.find("FILE: NOT FOUND") != string::npos){
		return E_OK;
	}
	//otevreni vystupniho souboru
	ofstream file;
	file.open(fileName.c_str());
	if(!file){
		return E_FILE_ERROR;
	}
	file << temp;
	//prijimani dat ze serveru a ukladani do souboru
	while(true){
		temp.clear();
		a = recv(sock, buffer, 1024, 0);
		if(a == -1){
			return E_SEND_ERR;
		}
		if(a == 0){
			break;
		}
		temp.append(buffer, a);
		if((size_t)(a = temp.find("\r\n\r\n\r\n")) != string::npos){
			temp.erase(a);
			file << temp;
			break;
		}
		file << temp;
	}
	file.close();
	return E_OK;
}

/**
 * Odeslani dat na socketu
 *@param fileName
 *nazev souboru pro cteni dat
 *@param sock
 *socket pro komunikaci
 **/
errorCode uploadData(string fileName, int sock){
	string temp;
	ifstream file;
	file.open(fileName.c_str());
	if(!file){
		string msg = ("FILE: NOT FOUND\r\n\r\n\r\n");
		if(send(sock, msg.c_str(), msg.length(), 0) < 0){
			return E_SEND_ERR;
		}
		cout << "Soubor nenalezen..." << endl;
	}
	
	while(getline(file, temp)){
		temp.append("\n");
		if(send(sock, temp.c_str(), temp.length(), 0) < 0){
			return E_SEND_ERR;
		}
	}
	file.close();
	return E_OK;
}