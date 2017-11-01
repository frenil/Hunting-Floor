#pragma once
#include "Player.h"
#define SERVERIP "127.0.0.1"
#define SENDPORT 9000
#define RECEIVEPORT 9050
#define CLIENT_ID 0

struct Position {
	int id;
	float position[3];
	float right[3];
	float up[3];
	float look[3];
};

class ClientUDP
{
	Position m_pos;
	Position other_pos;
	WSADATA wsa;
public:
	ClientUDP();
	void err_display(char *msg);
	void err_quit(char *msg);
	void UDPsend();
	void Positionset(CPlayer &a);
	void UDPReceive();
	~ClientUDP();
};

