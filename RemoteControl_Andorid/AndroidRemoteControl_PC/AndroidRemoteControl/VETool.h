#pragma once

#define WIN32_LEAN_AND_MEAN

#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

namespace VETool {

	class CIPInfo {

	private:
		WSADATA		m_wsaData;
		CHAR		m_szIP[MAX_PATH];
		CHAR		m_szHostName[MAX_PATH];

	public:
		CIPInfo();
		~CIPInfo();

		LPCSTR getIPInfo();
		LPCSTR getHostName();
	};

}

