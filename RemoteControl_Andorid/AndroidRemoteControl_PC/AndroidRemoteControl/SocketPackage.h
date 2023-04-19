#pragma once
#include <basetsd.h>

struct CScreenInfo
{
	UINT32	m_ScreenWidth;
	UINT32	m_ScreenHeight;
};

struct CHeader
{
	UINT32	m_Type;
	UINT32	m_Length;
};

class CPackage
{
public:
	PBYTE	m_Buffer;
	UINT	m_Length;

public:
	CPackage(UINT32 package_type, PVOID body, UINT body_length)
	{
		// 申请存放包缓冲区
		m_Length = sizeof(CHeader) + body_length;
		m_Buffer = (PBYTE)new char[m_Length];

		// 初始化缓冲区数据
		CHeader* pHeader = (CHeader*)m_Buffer;
		pHeader->m_Type = package_type;
		pHeader->m_Length = body_length;
		memcpy(m_Buffer + sizeof(CHeader), body, body_length);
	}

	~CPackage() {
		delete[] m_Buffer;
	}

	//operator hv::Buffer* () {
	//	hv::Buffer buffer(m_Buffer, m_Length);
	//	return &buffer;
	//}
};

namespace PackageType {
	const UINT32 REQUEST_WINDOW_SHOW_ON = 100;
	const UINT32 REQUEST_WINDOW_SHOW_OFF = 110;
	const UINT32 REQUEST_ROOT_PERMISSION = 120;
	const UINT32 STREAM_WINDOW_SHOW = 200;
	const UINT32 STREAM_INPUT_CTRL = 210;
	const UINT32 STREAM_SHELL_DATA = 220;
	const UINT32 SOCKET_DISCONNECT = 300;
}