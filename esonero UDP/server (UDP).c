#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <string.h> /* for memset() */
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

int main(int argc, char *argv[]) {

	int porta;
	int Ssock;
		if (argc > 1){
			porta = atoi(argv[1]);
			}
			else porta = PORT;
		if (porta < 0){
			printf ("Il numero di porta è sbagliato %s \n",argv[1]);
			return -1;
			}

		#if defined WIN32
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2 ,2), &wsaData);
		if (iResult != 0) {
			printf ("error at WSASturtup\n");
			return EXIT_FAILURE;
			}
		#endif

struct sockaddr_in echoServAddr;
struct sockaddr_in echoClntAddr;
unsigned int cliAddrLen;
char echoBuffer[ECHOMAX];
int recvMsgSize;

nomepc = gethostbyaddr(&echoClntAddr.sin_addr, cliAddrLen, AF_INET);
printf("Messaggio ricevuto %s dal client con nomehost %s con ip: %s\n", echoBuffer, nomepc->h_name, inet_ntoa(echoClntAddr.sin_addr));
printf("Ricevuto: %s\n", echoBuffer);


// CREAZIONE DELLA SOCKET
if ((Ssock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
ErrorHandler("socket() failed");

// COSTRUZIONE DELL'INDIRIZZO DEL SERVER
memset(&echoServAddr, 0, sizeof(echoServAddr));
echoServAddr.sin_family = AF_INET;
echoServAddr.sin_port = htons(PORT);
echoServAddr.sin_addr.s_addr = inet_addr("ipserver");

// BIND DELLA SOCKET
if ((bind(Ssock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr))) < 0)
ErrorHandler("bind() failed");

// RICEZIONE DELLA STRINGA ECHO DAL CLIENT
while(1) {
cliAddrLen = sizeof(echoClntAddr);
recvMsgSize = recvfrom(Ssock, echoBuffer, ECHOMAX, 0, (struct sockaddr*)&echoClntAddr, &cliAddrLen);
printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
printf("Received: %s\n", echoBuffer);

// RINVIA LA STRINGA ECHO AL CLIENT
if (sendto(Ssock, echoBuffer, recvMsgSize, 0, (struct sockaddr *)&echoClntAddr,sizeof(echoClntAddr)) != recvMsgSize)
ErrorHandler("sendto() sent different number of bytes than expected");
}
}
