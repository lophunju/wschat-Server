#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <stdlib.h>

void ErrorHandling(char* message);
void removeEnter(char* str);

int main(int argc, char* argv[]) {

	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAddr, clntAddr;

	int szClntAddr;
	char welcome[] = "상대방과 연결되었습니다.\n";
	char msg[50];
	int strlen;

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

	//대화시작
	while (1) {
		//받기
		strlen = recv(hClntSock, msg, sizeof(msg), 0);
		if (strlen == -1)
			printf("상대방으로부터 메세지가 정상적으로 수신되지 않았습니다.\n");
		else if (strcmp(msg,"quit") == 0)
			break;
		else
			printf("상대방: %s \n", msg);

		//보내기
		printf("나: ");
		rewind(stdin);
		fgets(msg, sizeof(msg), stdin);
		removeEnter(msg);
		send(hClntSock, msg, sizeof(msg), 0);

		if (strcmp(msg, "quit") == 0)
			break;
	}

	//연결 해제부
	printf("연결을 종료합니다.\n");
	closesocket(hClntSock);
	WSACleanup();

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