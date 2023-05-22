#include "InitSockLib.h"

CInitSockLib::CInitSockLib()
{
	//初始化
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return ;
	}
	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2) {
		WSACleanup();
		return ;
	}
}


CInitSockLib::~CInitSockLib()
{
	//反初始化
	WSACleanup();
}

//静态成员,实例化一个对象,用来调用构造和析构,
//从而实现初始化socket库和反初始化
//省略了在其他地方实例化对象
//CInitSockLib CInitSockLib::m_initsocklib;

