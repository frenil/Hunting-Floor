#include "stdafx.h"
#include "ClientUDP.h"


ClientUDP::ClientUDP()
{
	WSAStartup(MAKEWORD(2, 2), &wsa);
}


ClientUDP::~ClientUDP()
{
	WSACleanup();
}

void ClientUDP::err_quit(char *msg)
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

void ClientUDP::err_display(char *msg)
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

void ClientUDP::UDPsend()
{
	int retval;

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);

	retval = sendto(sock, (char*)&m_pos, sizeof(m_pos), 0, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
	{
		err_display("sendto()");
	}

	closesocket(sock);
}

void ClientUDP::UDPReceive()
{
	int retval;
	int addrlen;
	SOCKADDR_IN peeraddr;

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	SOCKADDR_IN localaddr;
	ZeroMemory(&localaddr, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localaddr.sin_port = htons(SERVERPORT);
	retval = bind(sock, (SOCKADDR *)&localaddr, sizeof(localaddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	while (1)
	{
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, (char*)&other_pos, sizeof(other_pos), 0, (SOCKADDR *)&peeraddr, &addrlen);
		if (retval == SOCKET_ERROR)
		{
			err_display("recvfrom()");
			continue;
		}
	}

	closesocket(sock);
}

void ClientUDP::Positionset(CPlayer &a)
{
	m_pos.position[0] = a.GetPosition().x;
	m_pos.position[1] = a.GetPosition().y;
	m_pos.position[2] = a.GetPosition().z;

	m_pos.right[0] = a.GetRight().x;
	m_pos.right[1] = a.GetRight().y;
	m_pos.right[2] = a.GetRight().z;

	m_pos.look[0] = a.GetLook().x;
	m_pos.look[1] = a.GetLook().y;
	m_pos.look[2] = a.GetLook().z;

	m_pos.up[0] = a.GetUp().x;
	m_pos.up[1] = a.GetUp().y;
	m_pos.up[2] = a.GetUp().z;

	m_pos.id = CLIENT_ID;
}