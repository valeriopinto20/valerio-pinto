#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ECHOMAX 255
#define PP 31000


void pulisciStringa(char buf[],int size){
	int i;
	for(i=0;i<size;i++)
		buf[i]='\0';
}

void stampaErrore(char *msg){
	printf("%s\n",msg);
}

void pulisciWINSOCK(){
#if defined WIN32
	WSACleanup();
#endif
}

int main(void) {

#if defined WIN32
	//system("cls");
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2),&wsaData);
	if(iResult!=0){
		stampaErrore("Errore Startup");
		return EXIT_FAILURE;
	}
#else
	system("clear");
#endif


	int clientSocket;
	struct sockaddr_in indirizzoServerInvio;
	struct sockaddr_in indirizzoServerRicezione;
	int ricezioneLung;
	char messRic[ECHOMAX];
	char messInv[ECHOMAX];
	char strAlfan[ECHOMAX];
	int i;
	int lunghezzaStrInv;
	int lunghezzaStrRic;

	char nomeServer[ECHOMAX];
	int numeroPorta;
	printf("-> Inserisci il nome del server: ");
	memset(&nomeServer, 0, ECHOMAX);
	scanf("%s",nomeServer);

	do{

		printf("-> Inserisci il numero di porta: ");
		scanf("%d",&numeroPorta);

	}while(numeroPorta!=PP);

	memset(&messInv, 0, ECHOMAX);
	printf("-> Inserisci la stringa da inviare: ");
	scanf("%s",messInv);
	getchar();

	if((lunghezzaStrInv = strlen(messInv))> ECHOMAX){
		stampaErrore("-> Stringa troppo lunga\n");
	}

	clientSocket=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(clientSocket<0){
		stampaErrore("-> Errore creazione socket\n");
	}

	struct hostent *DNS;
	DNS = gethostbyname(nomeServer);	 //ricavo l'indirizzo del server dal nome

	memset(&indirizzoServerInvio,0,sizeof(indirizzoServerInvio));	 //assegnazione struct indirizzo
	indirizzoServerInvio.sin_family=PF_INET;
	indirizzoServerInvio.sin_port=htons(numeroPorta);
	indirizzoServerInvio.sin_addr.s_addr=inet_addr(inet_ntoa(*(struct in_addr*)DNS->h_addr));

	if(sendto(clientSocket,messInv,lunghezzaStrInv,0,(struct sockaddr*)&indirizzoServerInvio,sizeof(indirizzoServerInvio))!=lunghezzaStrInv){ 	//invio stringa
		stampaErrore("-> Errore invio stringa\n");
		closesocket(clientSocket);
		pulisciWINSOCK();
	}
	pulisciStringa(messInv,ECHOMAX);

	ricezioneLung = sizeof(indirizzoServerRicezione);
	lunghezzaStrRic = recvfrom(clientSocket,messRic,ECHOMAX,0,(struct sockaddr*)&indirizzoServerRicezione,&ricezioneLung);
	if(indirizzoServerInvio.sin_addr.s_addr != indirizzoServerRicezione.sin_addr.s_addr){
		stampaErrore("-> Pacchetto da fonte sconosciuta\n");
	}
	messRic[lunghezzaStrRic]='\0';
	printf("-> Risposta ricevuta: %s\n",messRic);

	int contV=0;
	char strV[ECHOMAX];
	char vocaliTrovate[ECHOMAX];

	memset(&strV, 0, ECHOMAX);
	memset(&vocaliTrovate, 0, ECHOMAX);

	do{
		printf("-> Inserisci una stringa alfanumerica per il controllo delle vocali: ");
		memset(&strAlfan, 0, ECHOMAX);
		fgets(strAlfan, ECHOMAX, stdin);

		for(i=0;i<strlen(strAlfan);i++){ 	//controllo presenza vocali

			if(toupper(strAlfan[i])=='A' || toupper(strAlfan[i])=='E' || toupper(strAlfan[i])=='I' || toupper(strAlfan[i])=='O' || toupper(strAlfan[i])=='U'){
				vocaliTrovate[contV]=strAlfan[i];
				contV++;
			}
		}

		if(contV==0)
			printf("-> Inserisci una stringa che contenga vocali\n\n");

	}while(contV==0);


	printf("-> Vocali trovate: ");	  //stampa vocali trovate in maiuscolo
	for(i=0;i<contV;i++){
		printf("%c\n\n",vocaliTrovate[i]);
	}

	itoa(contV, strV, 10);//casting da intero a stringa
	printf("%s", strV);

	if(sendto(clientSocket, strV,strlen(strV),0,(struct sockaddr*)&indirizzoServerInvio,sizeof(indirizzoServerInvio))!=strlen(strV)){ 	//invio numero vocali
		stampaErrore("-> Errore invio stringa");
		closesocket(clientSocket);
		pulisciWINSOCK();
	}else{ printf("-> Invio vocali al server\n\n");
	}


	for(i=0;i<contV;i++) 	//invio vocali al server
	{
		pulisciStringa(messInv,ECHOMAX);
		messInv[0]=vocaliTrovate[i];
		messInv[1]='\0';

		if(sendto(clientSocket,messInv,strlen(messInv),0,(struct sockaddr*)&indirizzoServerInvio,sizeof(indirizzoServerInvio))!=strlen(messInv)){
			stampaErrore("-> Errore invio stringa\n");
			closesocket(clientSocket);
			pulisciWINSOCK();
		}

		pulisciStringa(messRic,ECHOMAX); 	//ricezione vocali in maiuscolo
		ricezioneLung = sizeof(indirizzoServerRicezione);
		lunghezzaStrRic = recvfrom(clientSocket,messRic,ECHOMAX,0,(struct sockaddr*)&indirizzoServerRicezione,&ricezioneLung);
		if(indirizzoServerInvio.sin_addr.s_addr != indirizzoServerRicezione.sin_addr.s_addr){
			stampaErrore("-> Pacchetto da fonte sconosciuta\n");
		}
		messRic[lunghezzaStrRic]='\0';
		printf("-> Vocale maiuscola: %s\n",messRic);
	}

	getchar();
	closesocket(clientSocket); 	//chiudo il socket
	pulisciWINSOCK();

	return 0;
}
