#pragma once
#include <WinSock2.h>

class CInitSockLib
{
public:
	CInitSockLib();
	~CInitSockLib();

private:
	static CInitSockLib m_initsocklib;
};

