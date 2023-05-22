#include "MyTool.h"

void Printf_Debug(LPCTSTR format, ...)
{
#ifdef _DEBUG
	TCHAR szbufFormat[0x100];
	TCHAR szbufFormat_prefix[0x100] = TEXT("[Easy_Debug]:");    //随意设置前缀，方便筛选
	va_list argList;                                            //指向参数的指针
	va_start(argList, format);                                  //参数列表初始化
	//va_arg(argList,type)                                      //指向下一变量
	_vstprintf_s(szbufFormat, format, argList);                 //使用参数列表发送格式化输出到字符串
	va_end(argList);                                            //参数列表结束 
	_tcscat_s(szbufFormat_prefix, szbufFormat);                 //拼接字符串
	OutputDebugString(szbufFormat_prefix);                      //输出调试信息
#endif
}

void ErrorExit(LPCTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process (if you want exit)
	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen(lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf) / sizeof(TCHAR), TEXT("%s failed with error %d: %s"), lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(dw);
}
