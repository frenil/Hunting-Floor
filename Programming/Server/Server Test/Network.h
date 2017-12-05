#pragma once
#include "stdafx.h"

#define SERVERIP "127.0.0.1"
#define PORT 9000

XMFLOAT4X4 Rpos, Spos;
WSADATA wsa;
SOCKET sock;
SOCKADDR_IN sockaddr;
int addrlen;

void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCWSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
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

int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

DWORD WINAPI Receive(LPVOID arg)
{
	while (1) {
		int retval = recvn(sock, (char*)&Rpos, sizeof(Rpos), 0);
		if (retval == SOCKET_ERROR) {
			err_display("recvn()");
			continue;
		}
		
		memcpy(&Spos, &Rpos, sizeof(Rpos));

	}

	// closesocket()
	closesocket(sock);

	return 0;
}

int NetInit()
{
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("sock()");

	ZeroMemory(&sockaddr, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = inet_addr(SERVERIP);
	sockaddr.sin_port = htons(PORT);

	int retval = connect(sock, (SOCKADDR*)&sockaddr, sizeof(sockaddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	HANDLE hReceive = CreateThread(NULL, 0, Receive, 0, 0, NULL);
	if (hReceive != NULL) CloseHandle(hReceive);
}