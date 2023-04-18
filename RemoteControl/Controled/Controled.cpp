// Controled.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <thread>
#include <string>
#include <afx.h>

#include "SocketTool.h"
#include "CPackage.h"

using namespace std;

void GraphicTransThread(LPARAM lpParameter);
void CmdThread(LPARAM lpParameter);
void FileTransThread(LPARAM lpParameter, bool isDownload, CString szFileName);

void InitCmdProcess();

bool g_bGraphicTrans;

HANDLE  g_hRead;
HANDLE  g_hWrite;
HANDLE  g_hCmdProcess;

int main()
{
	CSocketTool SocketTool;
	CSocketInfo ListenSockInfo = SocketTool.InitTCPSock("", "8848", true);
	CSocketInfo udpSockInfo = SocketTool.InitUDPSock("192.168.124.12", "8849", false);

	InitCmdProcess();	// 控制台初始化

	char name[32];
	int namelen = 32;
	gethostname(name, namelen);
	PHOSTENT hostinfo = gethostbyname(name);

	cout << "本机IP[" << inet_ntoa(*(struct in_addr*)*hostinfo->h_addr_list) << "]" << endl;
	cout << "正在监听端口，等待连接..." << endl;

	sockaddr_in Destaddr;
	int addrlen = sizeof(sockaddr_in);
	SOCKET tcpClientSocket = accept(ListenSockInfo.m_Socket, (sockaddr*)&Destaddr, &addrlen);

	cout << "目标[" << inet_ntoa(Destaddr.sin_addr) << "]已连接" << endl;

	char SockBuffer[0xFF];
	while (true)
	{
		auto pHeader = reinterpret_cast<CPackageHeader*>(SockBuffer);
		int iResult = recv(tcpClientSocket, SockBuffer, 0xFF, MSG_PEEK);
		int nlength = pHeader->m_Length;

		if (iResult >= nlength && (pHeader->m_PackType != eSockType::COM_FILEBYTE))
		{
			iResult = recv(tcpClientSocket, SockBuffer, nlength, 0);
			switch (pHeader->m_PackType)
			{
			case eSockType::C2S_SCREENDATA:
				if (reinterpret_cast<CScreenDataRequest*>(SockBuffer)->m_bSwitch)
				{
					cout << "目标请求屏幕共享" << endl;

					RECT rectSreen;
					HDC hdc = GetDC(NULL);
					int Screen_Width = GetDeviceCaps(hdc, DESKTOPHORZRES);
					int Screen_Height = GetDeviceCaps(hdc, DESKTOPVERTRES);
					ReleaseDC(NULL, hdc);
					rectSreen = { 0,0,Screen_Width,Screen_Height };
					CScreenInfo screeninfo(rectSreen);
					send(tcpClientSocket, (char*)&screeninfo, sizeof(CScreenInfo), 0);
					cout << "发送屏幕参数：宽[" << rectSreen.right << "] 高[" << rectSreen.bottom << "]" << endl;

					g_bGraphicTrans = true;
					thread tGraphic(GraphicTransThread, (LPARAM)&udpSockInfo);
					tGraphic.detach();
					cout << "启动屏幕共享线程" << endl;
				}
				else
				{
					g_bGraphicTrans = false;
					cout << "目标关闭了屏幕共享" << endl;
				}
				break;
			case eSockType::C2S_FILE:
			{
				auto pFilePack = reinterpret_cast<CFileDataRequest*>(SockBuffer);
				cout << "已接收文件目录查询请求" << endl;
				if (!strcmp(pFilePack->m_szFileName, "我的电脑"))
				{
					char szBuf[0xFF];
					GetLogicalDriveStrings(MAXBYTE, szBuf);
					LPTSTR lpszVariable = szBuf;
					while (*lpszVariable)
					{
						SendPackage<CFileDataSend>(tcpClientSocket, CFileDataSend::FileSign::FILE, lpszVariable);
						lpszVariable += strlen(lpszVariable) + 1;
						cout << "正在发送文件信息" << endl;
					}
				}
				else
				{
					CFileFind file;
					BOOL bContinue = file.FindFile(pFilePack->m_szFileName);    //查找包含字符串的文件
					while (bContinue)
					{
						bContinue = file.FindNextFile();						//查找下一个文件
						if ((file.IsDirectory() || !pFilePack->m_bIsDirectory) && !file.IsDots())
						{
							SendPackage<CFileDataSend>(tcpClientSocket, CFileDataSend::FileSign::FILE, file.GetFileName());
							cout << "正在发送文件信息" << endl;
						}
					}
				}
				SendPackage<CFileDataSend>(tcpClientSocket, CFileDataSend::FileSign::END, "");
				cout << "已终止发送文件目录" << endl;
				break;
			}
			case eSockType::C2S_KEYBORD:
			{
				cout << "键盘消息" << endl;
				auto pKeyboardPack = reinterpret_cast<CKeyBoardRequest*>(SockBuffer);
				keybd_event(pKeyboardPack->m_cKeyValue, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
				keybd_event(pKeyboardPack->m_cKeyValue, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
				break;
			}
			case eSockType::C2S_CMD:
			{
				cout << "控制台消息" << endl;
				static bool bCmdInit = false;
				if (!bCmdInit)
				{
					thread tCmdThread(CmdThread, tcpClientSocket);
					tCmdThread.detach();
					bCmdInit = true;
				}
				auto pCmdPack = reinterpret_cast<CConsoleRequest*>(SockBuffer);
				WriteFile(g_hWrite, pCmdPack->m_szCmdText, pCmdPack->m_Length - sizeof(CConsoleRequest) - 1, NULL, NULL);
				break;
			}
			case eSockType::C2S_MOUSE:
			{
				cout << "鼠标消息" << endl;
				auto pMousePack = reinterpret_cast<CMouseRequest*>(SockBuffer);

				::SetCursorPos(pMousePack->m_MousePoint.x, pMousePack->m_MousePoint.y);

				switch (pMousePack->m_eMouseEvent)
				{
				case CMouseRequest::MouseEvent::LBUTTONDBLCLK:
					cout << "双击了坐标 [" << pMousePack->m_MousePoint.x << "," << pMousePack->m_MousePoint.y << "]" << endl;
					mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
					mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
					break;
				case CMouseRequest::MouseEvent::LBUTTONDOWN:
					cout << "单击了坐标 [" << pMousePack->m_MousePoint.x << "," << pMousePack->m_MousePoint.y << "]" << endl;
					mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
					break;
				case CMouseRequest::MouseEvent::RBUTTONDOWN:
					cout << "右键单击了坐标 [" << pMousePack->m_MousePoint.x << "," << pMousePack->m_MousePoint.y << "]" << endl;
					mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
					break;
				}
				break;
			}
			case eSockType::C2S_FILETRANS:
			{
				cout << "文件传输请求" << endl;
				auto pFileTransPack = reinterpret_cast<CFileTransRequest*>(SockBuffer);
				thread tFileTrans(FileTransThread, tcpClientSocket, pFileTransPack->m_bDownload, pFileTransPack->m_szFileName);
				tFileTrans.detach();
				break;
			}
			}
		}
		if (iResult <= 0)
		{
			cout << "对方已断开链接" << endl;
			shutdown(tcpClientSocket, SD_SEND);
			closesocket(tcpClientSocket);

			cout << "等待新的链接" << endl;

			tcpClientSocket = accept(ListenSockInfo.m_Socket, (sockaddr*)&Destaddr, &addrlen);
			cout << "目标[" << inet_ntoa(Destaddr.sin_addr) << "]已连接" << endl;

			//break;
		}
	}

	if (g_hCmdProcess)
	{
		TerminateProcess(g_hCmdProcess, 0);
	}
	system("pause");
}

void GraphicTransThread(LPARAM lpParameter)
{
	auto pSockInfo = reinterpret_cast<CSocketInfo*>(lpParameter);

	HDC hDcScreen = ::GetDC(NULL);
	int Screen_Width = GetDeviceCaps(hDcScreen, DESKTOPHORZRES);
	int Screen_Height = GetDeviceCaps(hDcScreen, DESKTOPVERTRES);
	HDC hDcMem = CreateCompatibleDC(hDcScreen);
	HBITMAP hBmpMem = CreateCompatibleBitmap(hDcScreen, Screen_Width, Screen_Height);
	SelectObject(hDcMem, hBmpMem);

	int nBytesCnt = GetDeviceCaps(hDcScreen, BITSPIXEL) / 8;//一个像素点所占的字节数
	DWORD nBitsBufSize = Screen_Width * Screen_Height * nBytesCnt;
	LPVOID pBitBuf = malloc(nBitsBufSize);

	while (g_bGraphicTrans)
	{
		BitBlt(hDcMem, 0, 0, Screen_Width, Screen_Height, hDcScreen, 0, 0, SRCCOPY);
		GetBitmapBits(hBmpMem, nBitsBufSize, pBitBuf);

		for (size_t i = 0; i < nBitsBufSize / 1200; i++)	// 1200 刚好可以被 1080p 和 2k 屏整除
		{
			// 不进行二次拷贝以节省性能开销，直接使用像素值保留位置
			*((char*)pBitBuf + i * 1200 + 3) = i / 100;		// 存储高位
			*((char*)pBitBuf + i * 1200 + 7) = i % 100;		// 存储低位
			sendto(pSockInfo->m_Socket, (char*)pBitBuf + i * 1200, 1200, 0, pSockInfo->m_AddrInfo->ai_addr, pSockInfo->m_AddrInfo->ai_addrlen);
		}
	}

	free(pBitBuf);
}

void InitCmdProcess()
{
	HANDLE  hCmdRead;
	HANDLE  hCmdWrite;

	SECURITY_ATTRIBUTES sa;
	ZeroMemory(&sa, sizeof(sa));
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;

	CreatePipe(&g_hRead, &hCmdWrite, &sa, NULL);
	CreatePipe(&hCmdRead, &g_hWrite, &sa, NULL);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));

	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	si.hStdInput = hCmdRead;
	si.hStdOutput = hCmdWrite;
	si.hStdError = hCmdWrite;

	CreateProcess("C:\\WINDOWS\\system32\\cmd.exe", NULL, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi);
	g_hCmdProcess = pi.hProcess;
}

void CmdThread(LPARAM lpParameter)
{
	auto tcpSocket = (SOCKET)lpParameter;
	DWORD dwRead;
	CString szBuf;

	while (true)
	{
		if (PeekNamedPipe(g_hRead, NULL, 0, NULL, &dwRead, NULL))
		{
			if (dwRead > 0)
			{
				if (dwRead > 1400) { dwRead = 1400; }	// 再多传不了，缓存要超了
				if (ReadFile(g_hRead, szBuf.GetBufferSetLength(dwRead), dwRead, NULL, NULL))
				{
					SendPackage<CConsoleSend>(tcpSocket, szBuf);
					cout << "返回控制台消息，长度为：" << dwRead << endl;
				}
			}
		}
	}
}

void FileTransThread(LPARAM lpParameter, bool isDownload, CString szFileName)
{
	auto tcpSocket = (SOCKET)lpParameter;

	HANDLE hFile;
	char SockBuffer[1500];

	if (!isDownload)
	{
		CString szFilePath = "C:\\Users\\VE\\Desktop\\";
		szFilePath += szFileName;
		hFile = CreateFile(szFilePath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		cout << "正在上传文件：" << szFilePath << endl;
	}
	else
	{
		hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		cout << "正在下载文件：" << szFileName << endl;
	}

	if (!isDownload)
	{
		while (true)
		{
			int iResult = recv(tcpSocket, SockBuffer, 1500, MSG_PEEK);
			int nlength = reinterpret_cast<CPackageHeader*>(SockBuffer)->m_Length;
			eSockType SockType = reinterpret_cast<CPackageHeader*>(SockBuffer)->m_PackType;
			if ((iResult >= nlength) && (SockType == eSockType::COM_FILEBYTE))
			{
				iResult = recv(tcpSocket, SockBuffer, nlength, 0);
				auto pPack = reinterpret_cast<CFileByte*>(SockBuffer);
				if (!pPack->m_bSign)
				{
					cout << "上传结束" << endl;
					break;
				}
				else
				{
					DWORD dwWrite;
					WriteFile(hFile, pPack->m_Data, pPack->m_Length - sizeof(CFileByte), &dwWrite, NULL);
				}
			}
		}
	}
	else
	{
		while (true)
		{
			DWORD dwRead = 1;
			if (ReadFile(hFile, SockBuffer, 1200, &dwRead, NULL)&& dwRead)
			{
				SendPackage<CFileByte>(tcpSocket, true, SockBuffer, (int)dwRead);
			}
			else
			{
				SendPackage<CFileByte>(tcpSocket, false, nullptr, NULL);
				cout << "下载结束" << endl;
				break;
			}
		}
	}

	CloseHandle(hFile);
}