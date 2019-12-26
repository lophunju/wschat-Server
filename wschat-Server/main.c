#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <stdlib.h>
#include <process.h>

void ErrorHandling(char* message);
void removeEnter(char* str);
unsigned __stdcall Thread_Send(void* h);
unsigned __stdcall Thread_Recv(void* h);

typedef struct handler {
	SOCKET* socket;
	int* sendflag;
	int* recvflag;
} Handler;

int main(int argc, char* argv[]) {

	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAddr, clntAddr;

	int szClntAddr;
	char welcome[] = "상대방과 연결되었습니다.\n";
	char msg[50];
	int strlen;
	int sendflag;
	int recvflag;

	if (argc != 2) {
		printf("Usage : %s <port>\n", argv[0]); //실행시 port번호 필요
		exit(1);
	}



	//연결부
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	if (hServSock == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(atoi(argv[1]));

	if (bind(hServSock, (SOCKADDR*)& servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("bind() error!");

	if (listen(hServSock, 1) == SOCKET_ERROR) // 대기열 1개만
		ErrorHandling("listen() error");

	printf("서버가 [port: %s] 에서 실행되어 연결요청 대기중\n", argv[1]);

	szClntAddr = sizeof(clntAddr);
	hClntSock = accept(hServSock, (SOCKADDR*)& clntAddr, &szClntAddr);
	if (hClntSock == INVALID_SOCKET)
		ErrorHandling("accept() error!");

	closesocket(hServSock);

	printf("상대방과 연결되었습니다.\n\n");

	send(hClntSock, welcome, sizeof(welcome), 0);
	sendflag = 1;
	recvflag = 1;



	//대화부
	Handler HforTh;
	HforTh.socket = &hClntSock;
	HforTh.sendflag = &sendflag;
	HforTh.recvflag = &recvflag;

	_beginthreadex(NULL, 0, Thread_Send, (void*)&HforTh, 0, NULL); //송신 스레드 생성
	_beginthreadex(NULL, 0, Thread_Recv, (void*)&HforTh, 0, NULL); //수신 스레드 생성

	while (sendflag == 1 || recvflag == 1);


	//연결 해제부
	printf("연결을 종료합니다.\n");
	closesocket(hClntSock);
	WSACleanup();

	return 0;
}

unsigned __stdcall Thread_Send(void* h) {
	char msg[50];
	Handler* handler = (Handler*)h;

	while (*(handler->recvflag) == 1) {
		printf("나: ");
		rewind(stdin);
		fgets(msg, sizeof(msg), stdin);
		removeEnter(msg);
		send(*(handler->socket), msg, sizeof(msg), 0);

		if (strcmp(msg, "quit") == 0) {
			*(handler->sendflag) = 0;
			return 0;
		}
	}
	*(handler->sendflag) = 0;
	return 0;
}

unsigned __stdcall Thread_Recv(void* h) {
	char msg[50];
	int strlen;
	Handler* handler = (Handler*)h;

	while (*(handler->sendflag) == 1) {
		strlen = recv(*(handler->socket), msg, sizeof(msg), 0);
		if (strlen == -1 && *(handler->sendflag) == 1)
			printf("상대방으로부터 메세지가 정상적으로 수신되지 않았습니다.\n");
		else if (strcmp(msg, "quit") == 0 && *(handler->sendflag) == 1) {
			printf("상대방이 연결을 종료했습니다.\n");
			*(handler->recvflag) = 0;
			return 0;
		}
		else if (*(handler->sendflag) == 1)
			printf("상대방: %s \n", msg);
	}
	*(handler->recvflag) = 0;
	return 0;
}

void removeEnter(char* str) {
	str[strlen(str) - 1] = '\0';
}

void ErrorHandling(char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}