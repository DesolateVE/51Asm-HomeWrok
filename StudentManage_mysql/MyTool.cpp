#include "MyTool.h"

void Printf_Debug(LPCTSTR format, ...)
{
#ifdef _DEBUG
	TCHAR szbufFormat[0x100];
	TCHAR szbufFormat_prefix[0x100] = TEXT("[Easy_Debug]:");    //��������ǰ׺������ɸѡ
	va_list argList;                                            //ָ�������ָ��
	va_start(argList, format);                                  //�����б��ʼ��
	//va_arg(argList,type)                                      //ָ����һ����
	_vstprintf_s(szbufFormat, format, argList);                 //ʹ�ò����б��͸�ʽ��������ַ���
	va_end(argList);                                            //�����б���� 
	_tcscat_s(szbufFormat_prefix, szbufFormat);                 //ƴ���ַ���
	OutputDebugString(szbufFormat_prefix);                      //���������Ϣ
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
