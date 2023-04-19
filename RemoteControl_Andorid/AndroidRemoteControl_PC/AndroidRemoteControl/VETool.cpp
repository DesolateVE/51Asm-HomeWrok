#include "pch.h"
#include "VETool.h"

VETool::CIPInfo::CIPInfo()
{
	int ret = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
	if (ret == 0)
	{
		// 获取主机名
		ret = gethostname(m_szHostName, sizeof(m_szHostName));
		if (ret != SOCKET_ERROR)
		{
			// 获取主机ip
			HOSTENT* host = gethostbyname(m_szHostName);
			if (host != NULL)
			{
				strcpy(m_szIP, inet_ntoa(*(in_addr*)*host->h_addr_list));
			}
		}
	}
}

VETool::CIPInfo::~CIPInfo()
{
	WSACleanup();
}

LPCSTR VETool::CIPInfo::getIPInfo()
{
	return m_szIP;
}

LPCSTR VETool::CIPInfo::getHostName()
{
	return m_szHostName;
}
