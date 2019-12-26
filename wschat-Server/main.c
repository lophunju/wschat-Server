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
	char welcome[] = "����� ����Ǿ����ϴ�.\n";
	char msg[50];
	int strlen;
	int sendflag;
	int recvflag;

	if (argc != 2) {
		printf("Usage : %s <port>\n", argv[0]); //����� port��ȣ �ʿ�
		exit(1);
	}



	//�����
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
	sendflag = 1;
	recvflag = 1;



	//��ȭ��
	Handler HforTh;
	HforTh.socket = &hClntSock;
	HforTh.sendflag = &sendflag;
	HforTh.recvflag = &recvflag;

	_beginthreadex(NULL, 0, Thread_Send, (void*)&HforTh, 0, NULL); //�۽� ������ ����
	_beginthreadex(NULL, 0, Thread_Recv, (void*)&HforTh, 0, NULL); //���� ������ ����

	while (sendflag == 1 || recvflag == 1);


	//���� ������
	printf("������ �����մϴ�.\n");
	closesocket(hClntSock);
	WSACleanup();

	return 0;
}

unsigned __stdcall Thread_Send(void* h) {
	char msg[50];
	Handler* handler = (Handler*)h;

	while (*(handler->recvflag) == 1) {
		printf("��: ");
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
			printf("�������κ��� �޼����� ���������� ���ŵ��� �ʾҽ��ϴ�.\n");
		else if (strcmp(msg, "quit") == 0 && *(handler->sendflag) == 1) {
			printf("������ ������ �����߽��ϴ�.\n");
			*(handler->recvflag) = 0;
			return 0;
		}
		else if (*(handler->sendflag) == 1)
			printf("����: %s \n", msg);
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