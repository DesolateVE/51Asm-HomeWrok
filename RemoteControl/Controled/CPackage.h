#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <utility>
#include <memory>
#include <atltypes.h>

enum class eSockType :char
{
	// 请求
	C2S_SCREENDATA,
	C2S_MOUSE,
	C2S_KEYBORD,
	C2S_CMD,
	C2S_FILE,
	C2S_FILETRANS,
	// 返回
	S2C_SCREENDATA,
	S2C_MOUSE,
	S2C_KEYBORD,
	S2C_CMD,
	S2C_FILE,
	// 通用
	COM_FILEBYTE,
};

#pragma pack(push,1)
struct CPackageHeader
{
	WORD		m_Length;
	eSockType	m_PackType;
};

struct CFileTransRequest : CPackageHeader
{
	bool		m_bDownload;
	char		m_szFileName[0];

	CFileTransRequest(bool isDownload, const char* szFileName)
	{
		m_PackType = eSockType::C2S_FILETRANS;
		m_bDownload = isDownload;
		strcpy(m_szFileName, szFileName);
		m_Length = sizeof(CFileTransRequest) + strlen(m_szFileName) + 1;
	}
};

struct CFileByte : CPackageHeader
{
	bool		m_bSign;
	char		m_Data[0];

	CFileByte(bool Sign, char* pBuf, int BufSize)
	{
		m_PackType = eSockType::COM_FILEBYTE;
		m_bSign = Sign;
		memcpy(m_Data, pBuf, BufSize);
		m_Length = sizeof(CFileByte) + BufSize;
	}
};

struct CScreenInfo : CPackageHeader
{
	RECT m_Rect;		// 屏幕矩形

	CScreenInfo(RECT rect)
	{
		m_PackType = eSockType::S2C_SCREENDATA;
		m_Length = sizeof(CScreenInfo);
		m_Rect = rect;
	}
};

struct CScreenDataRequest : CPackageHeader
{
	bool m_bSwitch;		// 开关

	CScreenDataRequest(bool Switch)
	{
		m_PackType = eSockType::C2S_SCREENDATA;
		m_Length = sizeof(CScreenDataRequest);
		m_bSwitch = Switch;
	}
};

struct CFileDataRequest : CPackageHeader
{
	bool	m_bIsDirectory;
	char	m_szFileName[0];

	CFileDataRequest(bool IsDirectory, const char* szFileName)
	{
		m_PackType = eSockType::C2S_FILE;
		m_bIsDirectory = IsDirectory;
		strcpy(m_szFileName, szFileName);
		m_Length = sizeof(CFileDataRequest) + strlen(szFileName) + 1;
	}
};

struct CFileDataSend : CPackageHeader
{
	enum class FileSign : char { END, FILE } m_eFileSign;
	char	m_szFileName[0];

	CFileDataSend(FileSign sign, const char* szFileName)
	{
		m_PackType = eSockType::S2C_FILE;
		m_eFileSign = sign;
		strcpy(m_szFileName, szFileName);
		m_Length = sizeof(CFileDataSend) + strlen(szFileName) + 1;
	}
};

struct CKeyBoardRequest : CPackageHeader
{
	char	m_cKeyValue;
	CKeyBoardRequest(char Key)
	{
		m_PackType = eSockType::C2S_KEYBORD;
		m_Length = sizeof(CKeyBoardRequest);
		m_cKeyValue = Key;
	}
};

struct CConsoleRequest : CPackageHeader
{
	char	m_szCmdText[0];
	CConsoleRequest(const char* szCmdText)
	{
		m_PackType = eSockType::C2S_CMD;
		strcpy(m_szCmdText, szCmdText);
		m_Length = sizeof(CConsoleRequest) + strlen(szCmdText) + 1;
	}
};

struct CConsoleSend : CPackageHeader
{
	char	m_szRtnText[0];
	CConsoleSend(const char* szRtnText)
	{
		m_PackType = eSockType::S2C_CMD;
		strcpy(m_szRtnText, szRtnText);
		m_Length = sizeof(CConsoleSend) + strlen(szRtnText) + 1;
	}
};

struct CMouseRequest : CPackageHeader
{
	CPoint m_MousePoint;
	enum class MouseEvent :char { RBUTTONDOWN, LBUTTONDOWN, LBUTTONDBLCLK }m_eMouseEvent;

	CMouseRequest(CPoint MousePoint, MouseEvent Event)
	{
		m_eMouseEvent = Event;
		m_MousePoint = MousePoint;
		m_PackType = eSockType::C2S_MOUSE;
		m_Length = sizeof(CMouseRequest);
	}
};
#pragma pack(pop)

template<typename T, class... ARGS>
void SendPackage(SOCKET sock, ARGS&&... args)
{
	char* pBuf = new char[1500];
	std::shared_ptr<T>pData(new (pBuf)T(std::forward<ARGS>(args)...));
	auto pHeader = reinterpret_cast<CPackageHeader*>(pBuf);

	int size = pHeader->m_Length;
	int SendSize = send(sock, (char*)pData.get(), size, 0);
}
