#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <DirectXMath.h>

#define PORT 9000
#define BUFSIZE   512
#define REMOTEIP "127.0.0.1"

DirectX::XMFLOAT4X4 Rpos, Spos;



// 소켓 함수 오류 출력 후 종료
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
	int retval;
	SOCKET sock = (SOCKET)arg;
	SOCKADDR_IN clientaddr;
	int addrlen;

	addrlen = sizeof(clientaddr);
	getpeername(sock, (SOCKADDR *)&clientaddr, &addrlen);

	while (1) {		
		retval = recvn(sock, (char*)&Rpos, sizeof(Rpos), 0);
		if (retval == SOCKET_ERROR) {
			err_display("recvn()");
			continue;
		}

		printf("%f %f %f %f\n", Rpos._41, Rpos._42, Rpos._43, Rpos._44);


		memcpy(&Spos, &Rpos, sizeof(Rpos));

	}

	// closesocket()
	closesocket(sock);

	return 0;
}

int main(int argc, char *argv[])
{
	int retval;
	HANDLE hReceive;


	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;

	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(PORT);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");
	
	addrlen = sizeof(clientaddr);

	while (1) {
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if(client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}
		
		hReceive = CreateThread(NULL, 0, Receive, (LPVOID)client_sock, 0, NULL);
		if (hReceive == NULL) closesocket(client_sock);
		else CloseHandle(hReceive);

		Spos._41 = Spos._41 + 5;
		Spos._42 = Spos._42 + 5;

		retval = send(client_sock, (char*)&Spos, sizeof(Spos), 0);

	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}