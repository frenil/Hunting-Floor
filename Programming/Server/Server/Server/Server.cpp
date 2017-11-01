#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define RECEIVEPORT 9000
#define SENDPORT 9050
#define BUFSIZE   512
#define REMOTEIP "255.255.255.255"

struct Position {
	int id;
	float position[3];
	float right[3];
	float up[3];
	float look[3];
};

Position Rpos, Spos;
HANDLE hReadEvent;
HANDLE hWriteEvent;

// ���� �Լ� ���� ��� �� ����
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// ���� �Լ� ���� ���
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

DWORD WINAPI UDPSend(LPVOID arg)
{
	int retval;
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// ��ε�ĳ���� Ȱ��ȭ
	BOOL bEnable = TRUE;
	retval = setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
		(char *)&bEnable, sizeof(bEnable));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// ���� �ּ� ����ü �ʱ�ȭ
	SOCKADDR_IN remoteaddr;
	ZeroMemory(&remoteaddr, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = inet_addr(REMOTEIP);
	remoteaddr.sin_port = htons(SENDPORT);

	// ��ε�ĳ��Ʈ ������ ������
	while (1) {
		retval = WaitForSingleObject(hWriteEvent, INFINITE);
		if (retval != WAIT_OBJECT_0) break;
		// ������ ������
		retval = sendto(sock, (char*)&Spos, sizeof(Spos), 0,
			(SOCKADDR *)&remoteaddr, sizeof(remoteaddr));
		if (retval == SOCKET_ERROR) {
			err_display("sendto()");
			continue;
		}
		printf("[UDP] %d����Ʈ�� ���½��ϴ�.\n", retval);
		SetEvent(hReadEvent);
	}

	// closesocket()
	closesocket(sock);

	return 0;
}

int main(int argc, char *argv[])
{
	int retval;
	HANDLE hSend;
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hReadEvent == NULL) return 1;
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hWriteEvent == NULL) return 1;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	SOCKADDR_IN peeraddr;
	int addrlen;

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	SOCKADDR_IN localaddr;
	ZeroMemory(&localaddr, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localaddr.sin_port = htons(RECEIVEPORT);
	retval = bind(sock, (SOCKADDR *)&localaddr, sizeof(localaddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	hSend = CreateThread(NULL, 0, UDPSend, 0, 0, NULL);
	while (1) {
		// ������ �ޱ�
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, (char*)&Rpos, sizeof(Rpos), 0,
			(SOCKADDR *)&peeraddr, &addrlen);
		if (retval == SOCKET_ERROR) {
			err_display("recvfrom()");
			continue;
		}
		printf("ID : %d/x = %f/y = %f/z = %f/look x = %f/y = %f/z = %f\n", Rpos.id, Rpos.position[0], Rpos.position[1], Rpos.position[2], Rpos.look[0], Rpos.look[1], Rpos.look[2]);
		retval = WaitForSingleObject(hReadEvent, INFINITE);
		if (retval != WAIT_OBJECT_0) break;
		memcpy(&Spos, &Rpos, sizeof(Rpos));
		SetEvent(hWriteEvent);
	}

	// closesocket()
	closesocket(sock);
	CloseHandle(hSend);
	CloseHandle(hWriteEvent);
	CloseHandle(hReadEvent);

	// ���� ����
	WSACleanup();
	return 0;
}