#include "SocketTool.h"

CSocketTool::CSocketTool()
{
	int iResult = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
	assert(iResult == 0);
}

CSocketTool::~CSocketTool()
{
	WSACleanup();
}

CSocketInfo CSocketTool::InitTCPSock(PCSTR szNodeName, PCSTR szServerName, bool isServer)
{
	struct addrinfo* result = NULL;
	struct addrinfo  hints;
	int iResult;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	if (isServer)
	{
		hints.ai_flags = AI_PASSIVE;
	}

	iResult = getaddrinfo(szNodeName, szServerName, &hints, &result);
	assert(iResult == 0);

	SOCKET RtSock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	assert(RtSock != INVALID_SOCKET);

	if (isServer)
	{
		iResult = bind(RtSock, result->ai_addr, (int)result->ai_addrlen);
		assert(iResult != SOCKET_ERROR);
	}

	if (isServer)
	{
		iResult = listen(RtSock, SOMAXCONN);
		assert(iResult != SOCKET_ERROR);
	}

	CSocketInfo RtInfo;
	RtInfo.m_Socket = RtSock;
	RtInfo.m_AddrInfo = result;

	return RtInfo;
}

CSocketInfo CSocketTool::InitUDPSock(PCSTR szNodeName, PCSTR szServerName, bool isServer)
{
	struct addrinfo* result = nullptr;
	struct addrinfo  hints;
	int iResult;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	if (isServer)
	{
		hints.ai_flags = AI_PASSIVE;
	}

	iResult = getaddrinfo(szNodeName, szServerName, &hints, &result);
	assert(iResult == 0);

	SOCKET RtSock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	assert(RtSock != INVALID_SOCKET);

	if (isServer)
	{
		iResult = bind(RtSock, result->ai_addr, (int)result->ai_addrlen);
		assert(iResult != SOCKET_ERROR);
	}

	CSocketInfo RtInfo;
	RtInfo.m_Socket = RtSock;
	RtInfo.m_AddrInfo = result;

	return RtInfo;
}
