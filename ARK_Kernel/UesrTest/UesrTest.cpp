// UesrTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>
#include <winioctl.h>

#include "../ARK_Kernel/IOCTLSTD.h"

void DisplayErrorText()
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);

	wprintf((LPTSTR)lpMsgBuf);

	// Free the buffer.
	LocalFree(lpMsgBuf);
}

// 选择子
typedef struct _SEGMENT_SELECTOR
{
	USHORT	RPL : 2;
	USHORT	TI : 1;
	USHORT	SI : 13;
}SEGMENT_SELECTOR, * PSEGMENT_SELECTOR;

// 中断门和陷阱门描述符表
typedef struct _INTERRPUT_TRAP_DESCRIPTOR
{
	USHORT				TargetOffestLow1;
	SEGMENT_SELECTOR	TargetSelector;
	ULONG				IST : 3;
	ULONG : 5;
	ULONG				Type : 4;
	ULONG : 1;
	ULONG				DPL : 2;
	ULONG				P : 1;
	ULONG				TargetOffestLow2 : 16;
	ULONG				TargetOffestHigh;
	ULONG : 32;
} INTERRPUT_TRAP_DESCRIPTOR, * PINTERRPUT_TRAP_DESCRIPTOR;

#define CALL_GATE_TYPE			0xC
#define INTERRPUT_GATE_TYPE		0xE
#define TRAP_GATE_TYPE			0xF

int main()
{
	sizeof(INTERRPUT_TRAP_DESCRIPTOR);

	ULONGLONG DESCRIPTOR[] = {
		0x7a008e000010b800 ,0x00000000fffff805,
		0x7a008e040010bb40 ,0x00000000fffff805,
		0x7a008e030010c040 ,0x00000000fffff805,
		0x7a00ee000010c500 ,0x00000000fffff805,
		0x7a00ee000010c840 ,0x00000000fffff805,
		0x7a008e000010cb80 ,0x00000000fffff805,
		0x7a008e000010d0c0 ,0x00000000fffff805,
		0x7a008e000010d5c0 ,0x00000000fffff805,
		0x7a008e010010d8c0 ,0x00000000fffff805,
		0x7a008e000010dbc0 ,0x00000000fffff805,
		0x7a008e000010dec0 ,0x00000000fffff805,
		0x7a008e000010e1c0 ,0x00000000fffff805,
		0x7a008e000010e580 ,0x00000000fffff805,
		0x7a008e000010e8c0 ,0x00000000fffff805,
		0x7a008e000010ec00 ,0x00000000fffff805,
		0x7a008e0000103678 ,0x00000000fffff805
	};

	PINTERRPUT_TRAP_DESCRIPTOR pTemp = (PINTERRPUT_TRAP_DESCRIPTOR)DESCRIPTOR;
	PCTSTR szType;

	for (ULONG i = 0; i < ARRAYSIZE(DESCRIPTOR) / 2; i++)
	{

		switch (pTemp->Type)
		{
		case CALL_GATE_TYPE:
			szType = L"CALL_GATE_TYPE";
			break;
		case INTERRPUT_GATE_TYPE:
			szType = L"INTERRPUT_GATE_TYPE";
			break;
		case TRAP_GATE_TYPE:
			szType = L"TRAP_GATE_TYPE";
			break;
		default:
			szType = L"UNKNOWN_TYPE";
			break;
		}

		ULONGLONG addr = (ULONGLONG)pTemp->TargetOffestLow1 | (ULONGLONG)pTemp->TargetOffestLow2 << 16 | (ULONGLONG)pTemp->TargetOffestHigh << 32;
		wprintf(L"<%X> <%X> <%X> <%X> <%p> <%s>\n", i, pTemp->P, pTemp->IST, *(PUSHORT)&pTemp->TargetSelector, addr, szType);

		pTemp++;
	}







	HANDLE hFile = CreateFile(
		SYMBOL_PATH,
		GENERIC_ALL,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		DisplayErrorText();
	}
	else
	{
		wprintf(L"[51asm] hFile --> %p\n", hFile);

	}

	WCHAR FilePath[] = LR"(C:\Windows)";

	DWORD dwBytes;
	ULONG nFileNum;
	DeviceIoControl(hFile, IOCTL_FILE_FIND, FilePath, sizeof(FilePath), &nFileNum, sizeof(nFileNum), &dwBytes, NULL);

	wprintf(L"[51asm] nFileNum --> %u\n", nFileNum);

	PCFILEINFO pInfo = (PCFILEINFO)malloc(1024);

	PCWCHAR szType;
	ULONG ulFileSize;

	for (size_t i = 0; i < nFileNum; i++)
	{
		DeviceIoControl(hFile, IOCTL_FILE_INFO, nullptr, NULL, pInfo, 1024, &dwBytes, NULL);

		if (pInfo->bFileType)
		{
			szType = L"Directory";
		}
		else
		{
			szType = L"File";

			ulFileSize = pInfo->qwFileSize.QuadPart / 1024;
			if (!ulFileSize) ulFileSize = 1;
		}

		WCHAR szFileTime[0x20];
		FILETIME LocalTime;
		SYSTEMTIME SystemTime;

		FileTimeToLocalFileTime((PFILETIME)&pInfo->qwChangeTime, &LocalTime);
		FileTimeToSystemTime(&LocalTime, &SystemTime);
		swprintf_s(szFileTime, L"%d/%d/%d %d:%d", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute);


		wprintf(L"[51asm] <%s> <%d KB> <%s> FileName --> %s\n", szType, ulFileSize, szFileTime, pInfo->szFileName);
	}
	free(pInfo);

	CloseHandle(hFile);

	//SYSTEMTIME SystemTime;
	//WCHAR szFileTime[0x20];
	//FILETIME LocalTime;
	//FILETIME LastTime;

	//LONGLONG Input[] = { 0x01D8C16E742C90CE,0x01D8C16E7431438D,0x01D8C16E73C5FE18,0x01D8C16E7436019E };


	//for (auto var : Input)
	//{
	//	LastTime.dwHighDateTime = ((LARGE_INTEGER*)&var)->HighPart;
	//	LastTime.dwLowDateTime = ((LARGE_INTEGER*)&var)->LowPart;

	//	FileTimeToLocalFileTime(&LastTime, &LocalTime);
	//	FileTimeToSystemTime(&LocalTime, &SystemTime);
	//	swprintf_s(szFileTime, L"%d/%d/%d %d:%d:%d %d", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds);

	//	wprintf(L"%s\n", szFileTime);
	//}

	system("pause");
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
