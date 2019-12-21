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
	char welcome[] = "����� ����Ǿ����ϴ�.\n";
	char msg[30];
	int strlen;

	if (argc != 2) {
		printf("Usage : %s <port>\n", argv[0]); //����� port��ȣ �ʿ�
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

	if (listen(hServSock, 1) == SOCKET_ERROR) // ��⿭ 1����
		ErrorHandling("listen() error");

	printf("������ [port: %s] ���� ����Ǿ� �����û �����\n", argv[1]);

	szClntAddr = sizeof(clntAddr);
	hClntSock = accept(hServSock, (SOCKADDR*)& clntAddr, &szClntAddr);
	if (hClntSock == INVALID_SOCKET)
		ErrorHandling("accept() error!");

	closesocket(hServSock);

	printf("����� ����Ǿ����ϴ�.\n\n");

	send(hClntSock, welcome, sizeof(welcome), 0);

	while (1) {
		strlen = recv(hClntSock, msg, sizeof(msg), 0);
		if (strlen == -1)
			printf("�������κ��� �޼����� ���������� ���ŵ��� �ʾҽ��ϴ�.\n");
		else if (strcmp(msg,"quit") == 0)
			break;
		else
			printf("����: %s \n", msg);
	}

	printf("������ �����մϴ�.\n");
	closesocket(hClntSock);
	WSACleanup();

	return 0;
}

void ErrorHandling(char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}