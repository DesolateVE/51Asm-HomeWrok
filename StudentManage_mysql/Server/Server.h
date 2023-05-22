
// Server.h: PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"		// 主符号
#include "../Common/CUMTSocket.h"

// CServerApp:
// 有关此类的实现，请参阅 Server.cpp
//

class CServerApp : public CWinApp
{
public:
	CServerApp();

public:
	CUMTSocket m_Sock;

public:
	CUMTSocket* getSocket();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CServerApp theApp;
