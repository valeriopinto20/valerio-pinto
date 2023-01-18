/*
 ============================================================================
 Name        : client.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#define BUFFERSIZE 512
#define PROTOPROT 27015

void ErrorHandler (char *errorMessage){
	printf ("%s",errorMessage);
}

void ClearWinSock() {
#if defined WIN32
	WSACleanup();
#endif
}

int main(void){
#if defined WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD (2,2), &wsaData);
	if (iResult!=0){
		printf ("errore a WSASturtup\n");
		return -1;
	}
#endif

	/* New Socket */
	int Csocket;
	Csocket = socket(PF_INET, SOCK_STREAM,IPPROTO_TCP);
	if (Csocket<0){
		ErrorHandler ("creazione socket fallita\n");
		closesocket(Csocket);
		ClearWinSock();
		return -1;
	}

	char * address="127.0.0.1";
	int port;
	printf ("Inserisci il numero di porta: ");
	scanf("%d",&port);
	getchar();

	//port = 5193;
	/* Indirizzo Server */
	struct sockaddr_in sad;
	memset(&sad,0,sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = inet_addr(address);
	sad.sin_port = htons(port);

	/* Connessione al server */
	if (connect(Csocket,(struct sockaddr *)&sad,sizeof(sad))<0){
		ErrorHandler ("Collegamento fallito.\n");
		system("PAUSE");
		closesocket(Csocket);
		ClearWinSock();
		return -1;
	}else {
		printf("Connesione effettuata\n");
	}

	int bytesRcvd;
	char buf[BUFFERSIZE];
	memset(&buf, 0, BUFFERSIZE);
	printf ("Stringa ricevuta dal server: ");
	if ((bytesRcvd = recv(Csocket,buf,BUFFERSIZE - 1,0))<0){
		ErrorHandler("recv() fallita o chiusa prematuramente\n");
		closesocket(Csocket);
		ClearWinSock();
		return -1;
	}
	printf("%s\n", buf);


	while(1){
		char string1[BUFFERSIZE];
		memset(&string1, 0, BUFFERSIZE);
		printf ("Inserisci prima stringa:");
		gets(string1);

		if (send(Csocket,string1,strlen(string1),0)!=strlen(string1)){
			ErrorHandler("Errore nell'invio messaggio");
			closesocket(Csocket);
			ClearWinSock();
			return -1;
		}

		char string2[BUFFERSIZE];
		memset(&string2, 0, BUFFERSIZE);
		printf ("Inserisci seconda stringa:");
		gets(string2);

		if (send(Csocket,string2,strlen(string2),0)!=strlen(string2)){
			ErrorHandler("Errore nell'invio messaggio");
			closesocket(Csocket);
			ClearWinSock();
			return -1;
		}

		/* Ricezione da server */
		char server_string[BUFFERSIZE*2];
		memset(&server_string, 0, BUFFERSIZE*2);
		printf ("Stringa ricevuta dal server:\n");
		if ((bytesRcvd = recv(Csocket,server_string,BUFFERSIZE*2 - 1,0))<0){
			ErrorHandler("recv() fallita o chiusa prematuramente\n");
			closesocket(Csocket);
			ClearWinSock();
			system("PAUSE");
			return -1;
		}
		printf("%s\n", server_string);
		if(strcmp(server_string, "bye") == 0 ){
			printf("Chiusura connessione con il server\n");
			closesocket(Csocket);
			ClearWinSock();
			system("PAUSE");
			return -1;
		}
		getchar();
		system("CLS");
	}


	closesocket(Csocket);
	ClearWinSock();
	printf ("\n");
	system("PAUSE");
	return 0;

}
