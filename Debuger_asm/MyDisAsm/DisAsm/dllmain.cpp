// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

#include "capstone/capstone.h"
#pragma comment(lib,"capstone/x32/capstone.lib")

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#ifdef __cplusplus
extern "C" {
#endif

	void DisAsmEngineInit();
	void DisAsmEngineDestory();
	cs_insn* GetDisAsm(uint8_t* buff, uint64_t addr, size_t size, size_t count);
	void FreeDisAsmInfo();
	void DoDump(char* szFileName, HANDLE hProcess, DWORD dwBaseOfImage);

#ifdef __cplusplus
}
#endif // __cpluplus

csh g_hCapstone;
cs_err g_errCapstone;
cs_insn* g_pInsn;
size_t g_nCodeRead;

void DisAsmEngineInit()
{
	// 初始化反汇编引擎
	g_errCapstone = cs_open(
		CS_ARCH_X86,	//x86指令集
		CS_MODE_32,		//使用32位模式解析opcode
		&g_hCapstone	//输出的反汇编句柄
	);
}

void DisAsmEngineDestory()
{
	// 释放引擎句柄
	cs_close(&g_hCapstone);
}

cs_insn* GetDisAsm(uint8_t* buff, uint64_t addr, size_t size, size_t count)
{
	g_nCodeRead = cs_disasm(g_hCapstone,		// 反汇编器句柄,从cs_open函数得到
		buff,									// 需要反汇编的opcode的缓冲区首地址
		size,									// 读取的字节数
		addr,									// 内存地址
		count,									// 需要反汇编的指令条数,如果是0,则反汇编出全部
		&g_pInsn								// 反汇编输出
	);
	return g_pInsn;
}

void FreeDisAsmInfo()
{
	cs_free(g_pInsn, g_nCodeRead);
}

void DoDump(char* szFileName, HANDLE hProcess, DWORD dwBaseOfImage)
{
	IMAGE_DOS_HEADER dos;//dos头
	IMAGE_NT_HEADERS nt;

	HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

	ReadProcessMemory(hProcess, (LPVOID)dwBaseOfImage, &dos, sizeof(IMAGE_DOS_HEADER), NULL);
	ReadProcessMemory(hProcess, (BYTE*)dwBaseOfImage + dos.e_lfanew, &nt, sizeof(IMAGE_NT_HEADERS), NULL);

	//读取节区并计算节区大小
	DWORD secNum = nt.FileHeader.NumberOfSections;
	PIMAGE_SECTION_HEADER Sections = new IMAGE_SECTION_HEADER[secNum];
	//读取节区
	ReadProcessMemory(hProcess, (BYTE*)dwBaseOfImage + dos.e_lfanew + sizeof(IMAGE_NT_HEADERS), Sections, secNum * sizeof(IMAGE_SECTION_HEADER), NULL);


	//计算所有节区的大小
	DWORD allsecSize = 0;
	DWORD maxSec;//最大的节区

	maxSec = 0;

	for (int i = 0; i < secNum; ++i)
	{
		allsecSize += Sections[i].SizeOfRawData;

	}

	//dos
	//nt
	//节区总大小
	DWORD topsize = secNum * sizeof(IMAGE_SECTION_HEADER) + sizeof(IMAGE_NT_HEADERS) + dos.e_lfanew;

	//使头大小按照文件对齐
	if ((topsize & nt.OptionalHeader.FileAlignment) != topsize)
	{
		topsize &= nt.OptionalHeader.FileAlignment;
		topsize += nt.OptionalHeader.FileAlignment;
	}
	DWORD ftsize = topsize + allsecSize;

	//创建文件映射
	HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, ftsize, 0);

	//创建视图
	LPVOID lpmem = MapViewOfFile(hMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

	PBYTE bpMem = (PBYTE)lpmem;
	memcpy(lpmem, &dos, sizeof(IMAGE_DOS_HEADER));
	//计算dossub 大小

	DWORD subSize = dos.e_lfanew - sizeof(IMAGE_DOS_HEADER);

	ReadProcessMemory(hProcess, (BYTE*)dwBaseOfImage + sizeof(IMAGE_DOS_HEADER), bpMem + sizeof(IMAGE_DOS_HEADER), subSize, NULL);


	nt.OptionalHeader.ImageBase = dwBaseOfImage;
	//保存NT头
	memcpy(bpMem + dos.e_lfanew, &nt, sizeof(IMAGE_NT_HEADERS));

	//保存节区
	memcpy(bpMem + dos.e_lfanew + sizeof(IMAGE_NT_HEADERS), Sections, secNum * sizeof(IMAGE_SECTION_HEADER));

	for (int i = 0; i < secNum; ++i)
	{
		ReadProcessMemory(hProcess, (BYTE*)dwBaseOfImage + Sections[i].VirtualAddress, bpMem + Sections[i].PointerToRawData, Sections[i].SizeOfRawData, NULL);
	}

	FlushViewOfFile(lpmem, 0);
	delete[] Sections;
	CloseHandle(hMap);
	UnmapViewOfFile(lpmem);
	CloseHandle(hFile);

	printf("------------- DUMP SUCCESS -------------");
}