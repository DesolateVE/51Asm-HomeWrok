#pragma once

#include <winsock2.h>			// socket2Ö§³Ö
#include <ws2tcpip.h>
#include <assert.h> 

#pragma  comment(lib,"ws2_32.lib") 

struct CSocketInfo
{
	SOCKET			m_Socket;
	addrinfo*		m_AddrInfo;
};

class CSocketTool
{
public:
	CSocketTool();
	~CSocketTool();

public:
	CSocketInfo InitTCPSock(PCSTR szNodeName, PCSTR szServerName, bool isBind);
	CSocketInfo InitUDPSock(PCSTR szNodeName, PCSTR szServerName, bool isBind);

private:
	WSADATA m_wsaData;
};

