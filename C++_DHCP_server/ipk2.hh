 #pragma once
/**
 * Projekt 2, IPK
 * @name ipk2
 * @author Petr Flajsingr, xflajs00
 **/
 #include <string>

using namespace std;

 //chybove kody
enum errorCode{
	E_OK = 0,
	E_OK_DONE,
	E_ARG_ERROR,
	E_SOCKET_ERROR,
	E_HOST_ERROR,
	E_ADDINFO_ERROR,
	E_BIND_ERROR,
	E_SETSOCK_ERROR,
	E_LISTEN_ERROR,
	E_SIGACTION_ERR,
	E_SEND_ERR,
	E_RECV_ERR,
	E_MSG_ERROR,
	E_FILE_ERROR
};

//typ komunikace
enum comType{
	M_DOWNLOAD = 0,
	M_UPLOAD
};

//prototypy funkci

void printError(errorCode);

errorCode downloadData(string, int);

errorCode uploadData(string, int);