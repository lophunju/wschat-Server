#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <stdlib.h>

void ErrorHandling(char* message);

int main(int argc, char* argv[]) {

	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAddr, clntAddr;

	int szClntAddr;
	char message[] = "Hello~!";

	if (argc != 2) {
		printf("Usage : %s <port>\n", argv[0]); //실행시 port번호 필요
		exit(1);
	}

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

	send(hClntSock, message, sizeof(message), 0);

	closesocket(hClntSock);
	WSACleanup();

	return 0;
}

void ErrorHandling(char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}