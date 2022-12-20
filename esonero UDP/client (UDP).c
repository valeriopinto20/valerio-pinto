#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <string.h>
#define ECHOMAX 255
#define PORT 45000

void ErrorHandler(char *errorMessage) {
printf(errorMessage);
}

void ClearWinSock() {
#if defined WIN32
WSACleanup();
#endif
}

int main() {

	#if defined WIN32
	WSADATA wsaData;
	int iResult = WSAStartup (MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		printf ("error at WSASturtup\n");
		return EXIT_FAILURE;
		}
	#endif

struct sockaddr_in echoServAddr;
struct sockaddr_in fromAddr;
struct hostent *DatiServer;
unsigned int fromSize;
char nomeserver[ECHOMAX];
char echoBuffer[ECHOMAX];
char ipserver[15];
int echoStringLen;
int respStringLen;
int porta;
int Csock;

// ACQUISIZIONE NOME SERVER
printf("Inserisci il nome del server: ");
gets(nomeserver);
DatiServer = gethostbyname(nomeserver);
printf("Inserisci il numero della porta:\n");
scanf("%d", &porta);
getchar();

// ACQUISIZIONE IP SERVER
strpcy(ipserver, inet_ntoa(*((struct in_addr*)DatiServer->h_addr_list[0])));

// BIND DELLA SOCKET
if ((bind(Csock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr))) < 0)
ErrorHandler("bind() failed");

// CREAZIONE DELLA SOCKET
if ((Csock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
ErrorHandler("socket() failed");

// COSTRUZIONE DELL'INDIRIZZO DEL SERVER
memset(&echoServAddr, 0, sizeof(echoServAddr));
echoServAddr.sin_family = PF_INET;
echoServAddr.sin_port = htons(porta);
echoServAddr.sin_addr.s_addr = inet_addr("ipserver");

// INVIO DELLA STRINGA ECHO AL SERVER
char* msg = "Ciao Server!";

if ((echoStringLen = strlen(msg)) > ECHOMAX)
ErrorHandler("echo word too long");

if (sendto(Csock, msg, echoStringLen, 0, (struct sockaddr*)&echoServAddr, sizeof(echoServAddr)) != echoStringLen)
ErrorHandler("sendto() sent different number of bytes than expected");

// RITORNO DELLA STRINGA ECHO
fromSize = sizeof(fromAddr);
respStringLen = recvfrom(Csock, echoBuffer, ECHOMAX, 0, (struct sockaddr*)&fromAddr, &fromSize);

}


