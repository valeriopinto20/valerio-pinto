/*
 ============================================================================
 Name        : server.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#define BUFFERSIZE 512		// Dimensione Buffer
#define PROTOPORT 7250		// Numero di porta
#define QLEN 6

void ErrorHandler (char *errorMessage){
	printf ("%s",errorMessage);
}

void ClearWinSock (){
#if defined WIN32
	WSACleanup();
#endif
}

int main(int argc, char *argv[]){

	int port;
	if (argc > 1){
		port = atoi(argv[1]);
	}
	else port = PROTOPORT;	// Default
	if (port < 0){
		printf ("numero di porta errato %s \n",argv[1]);
		return -1;
	}

#if defined WIN32
	WSADATA WSAData;
	int iresult = WSAStartup(MAKEWORD(2,2), &WSAData);
	if (iresult!=0){
		ErrorHandler("Errore a WSAStartup\n");
		return -1;
	}
#endif

	// New Socket
	int NewSocket;
	NewSocket=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (NewSocket < 0){
		ErrorHandler("Creazione Socket fallita.\n");
		closesocket(NewSocket);
		ClearWinSock();
		return -1;
	}

	// Indirizzo per la socket
	struct sockaddr_in sad;
	memset(&sad,0,sizeof(sad));
	sad.sin_family=AF_INET;
	sad.sin_addr.s_addr=inet_addr("127.0.0.1");
	sad.sin_port=htons(port);

	// Binding e errorhandler
	if (bind(NewSocket,(struct sockaddr*)&sad,sizeof(sad))<0){
		ErrorHandler("Bind fallito.\n");
		closesocket(NewSocket);
		ClearWinSock();
		return 0;
	}

	// Listen di NewSocket
	if (listen(NewSocket,QLEN)<0){
		ErrorHandler("Listen fallita\n");
		closesocket(NewSocket);
		ClearWinSock();
		return -1;
	}

	// New Connection
	struct sockaddr_in cad;
	int clientSocket;
	int clientLen;

	printf ("Ascolto aperto il client..");

	while(1){
		clientLen=sizeof(cad);
		if ((clientSocket=accept(NewSocket,(struct sockaddr*)&cad,&clientLen))<0){
			ErrorHandler("Accept fallita\n");
			closesocket(NewSocket);
			ClearWinSock();
			return-1;
		}
		printf ("Connessione stabilita con il client %s\n",inet_ntoa(cad.sin_addr));

		char * connessioneavvenuta = "Connessione avvenuta...";
		int connessioneavvenuta_len = strlen(connessioneavvenuta);

		if (send(clientSocket, connessioneavvenuta, connessioneavvenuta_len,0)!= connessioneavvenuta_len){
			ErrorHandler("Send ha inviato un numero diverso di byte\n");
			closesocket(clientSocket);
			ClearWinSock();
			system("PAUSE");
			return -1;
		}
		int continue_loop = 1;
		do{
			char buf[BUFFERSIZE];
			int bytesRcvd;
			char string1[BUFFERSIZE];
			memset(buf, 0, BUFFERSIZE);
			printf ("Riceve la prima stringa:\n");
			if ((bytesRcvd = recv(clientSocket,buf,BUFFERSIZE - 1,0))<0){
				ErrorHandler("recv() fallita o chiusa prematuramente\n");
				closesocket(clientSocket);
				ClearWinSock();
				return -1;
			}
			printf("%s \n", buf);

			memset(string1, 0, BUFFERSIZE);
			printf ("Riceve la seconda stringa:\n");
			if ((bytesRcvd = recv(clientSocket,string1,BUFFERSIZE - 1,0))<0){
				ErrorHandler("recv() fallita o chiusa prematuramente\n");
				closesocket(clientSocket);
				ClearWinSock();
				return -1;
			}
			printf("%s \n", string1);

			char stringConcat[BUFFERSIZE*2];
			memset(&stringConcat, 0, BUFFERSIZE*2);

			if(strcmp(string1, "quit") == 0 || strcmp(buf, "quit") == 0){
				memset(&stringConcat, 0, BUFFERSIZE*2);
				strcpy(stringConcat, "bye");
				int string_len = strlen(stringConcat);
				if (send(clientSocket,stringConcat,string_len,0)!=string_len){
					ErrorHandler("Send ha inviato un numero diverso di byte");
					closesocket(clientSocket);
					ClearWinSock();
					system("PAUSE");
					return -1;
				}
				continue_loop = 0;
			}else{
				strcat(buf, string1);
				strcpy(stringConcat, buf);

				int strcat_len = strlen(stringConcat);

				if (send(clientSocket,stringConcat,strcat_len,0)!=strcat_len){
					ErrorHandler("Send ha inviato un numero diverso di byte");
					closesocket(clientSocket);
					ClearWinSock();
					system("PAUSE");
					return -1;
				}
			}
		}while(continue_loop);
	}
}
