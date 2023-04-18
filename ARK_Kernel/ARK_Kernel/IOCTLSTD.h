#pragma once

#define DEVICE_NAME				L"\\Device\\KrnlHW64"
#define SYMBOL_NAME				L"\\DosDevices\\Global\\KrnlHW64"
#define SYMBOL_PATH				L"\\\\.\\KrnlHW64"

#define IOCTL_CODE_BASE			0x800
#define IOCTL_CODE(code)		CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTL_CODE_BASE + code, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_FILE_FIND			IOCTL_CODE(0)
#define IOCTL_FILE_INFO			IOCTL_CODE(1)

#define IOCTL_PROCESS_FIND		IOCTL_CODE(2)
#define IOCTL_PROCESS_INFO		IOCTL_CODE(3)

#define IOCTL_THREAD_FIND		IOCTL_CODE(4)
#define IOCTL_THREAD_INFO		IOCTL_CODE(5)

#define IOCTL_MODULE_FIND		IOCTL_CODE(6)
#define IOCTL_MODULE_INFO		IOCTL_CODE(7)

#define IOCTL_DRIVER_FIND		IOCTL_CODE(8)
#define IOCTL_DRIVER_INFO		IOCTL_CODE(9)

#define IOCTL_SSDT_FIND			IOCTL_CODE(10)
#define IOCTL_SSDT_INFO			IOCTL_CODE(11)

#define IOCTL_IDT_FIND			IOCTL_CODE(12)
#define IOCTL_GDT_FIND			IOCTL_CODE(13)
#define IOCTL_PAGE_FIND			IOCTL_CODE(14)

#define IOCTL_MEMORY_READ		IOCTL_CODE(15)
#define IOCTL_MEMORY_WRITE		IOCTL_CODE(16)

#define IOCTL_REGISTER_FIND		IOCTL_CODE(17)
#define IOCTL_REGISTER_INFO		IOCTL_CODE(18)

#define IOCTL_FILE_DELETE		IOCTL_CODE(19)
#define IOCTL_PROCESS_CLOSE		IOCTL_CODE(20)


#ifndef MAX_PATH
#define MAX_PATH				260
#endif

typedef struct _CFILEINFO {
	LARGE_INTEGER	qwFileSize;					// 文件大小
	LARGE_INTEGER	qwChangeTime;				// 文件修改时间戳
	BOOLEAN			bFileType;					// 文件类型
	WCHAR			szFileName[MAX_PATH];		// 文件名
}CFILEINFO, * PCFILEINFO;

typedef struct _CPROCESS {
	PVOID			pEProcess;					// 进程对象
	HANDLE			hPID;						// 进程ID
	HANDLE			hPPID;						// 父进程ID
	WCHAR			szProcessName[MAX_PATH];	// 进程名
}CPROCESS, * PCPROCESS;

typedef struct _CTHREAD {
	HANDLE			hTID;						// 线程ID
	PVOID			pEThread;					// 线程的起始地址
	UCHAR			dwBasePriority;				// 优先级
	UCHAR			dwStatus;					// 状态
}CTHREAD, * PCTHREAD;

typedef struct _CMODULE {
	WCHAR			szFullDllName[MAX_PATH];	// 模块完整路径
	WCHAR			szBaseDllName[MAX_PATH];	// 模块名
	PVOID			pDllBase;					// 模块基地址
	PVOID			pEntryPoint;				// 模块入口点
	ULONG			dwSize;						// 模块大小
}CMODULE, * PCMODULE;

typedef struct _CSSDTINFO {
	WCHAR			szFunName[MAX_PATH];		// 函数名
	ULONG			dwFunNum;					// 编号
	ULONGLONG		qwFunaddr;					// 地址
}CSSDTINFO, * PCSSDTINFO;

typedef struct _CIDTINFO {
	ULONG			dwIndex;					// 下标
	ULONGLONG		qwFunaddr;					// 函数地址
	USHORT			wDPL;						// 特权级别
	USHORT			wTargetSelector;			// 选择子
} CIDTINFO, * PCIDTINFO;

typedef struct _CGDTINFO {
	ULONG			dwIndex;					// 下标
	ULONG			dwLimit;					// 范围
	ULONGLONG		qwBase;						// 地址
	USHORT			wType;						// 类型
	USHORT			wDPL;						// 特权级别
	BOOLEAN			bS;							// S = 0 系统段 S = 1 代码/数据段
	BOOLEAN			bL;							// L = 0 x32 , L = 1 x64
}CGDTINFO, * PCGDTINFO;

typedef struct _CPAGEINFO {
	BOOLEAN			bRW;						// 能否写入
	BOOLEAN			bAccess;					// 是否被访问
	BOOLEAN			bUS;						// 权限
	BOOLEAN			bNX;						// 能否执行
	USHORT			wIndex;						// 当前表数组下标
	ULONGLONG		qwAddress;					// 下级表物理地址
}CPAGEINFO, * PCPAGEINFO;

typedef struct _CPAGEINDEX {
	ULONGLONG		wPXEIndex : 9;				// PXE 下标
	ULONGLONG		wPPEIndex : 9;				// PPE 下标
	ULONGLONG		wPDEIndex : 9;				// PDE 下标
	ULONGLONG : 5;
}CPAGEINDEX, * PCPAGEINDEX;

typedef enum _PAGE_TYPE {
	PAGE_PXE,
	PAGE_PPE,
	PAGE_PDE,
	PAGE_PTE
}PAGE_TYPE;

typedef struct _CPAGEQUERY {
	HANDLE			eProcess;					// _eProcess
	CPAGEINDEX		Index;						// 查询页表下标
	PAGE_TYPE		Type;						// 查询页表类型
}CPAGEQUERY, * PCPAGEQUERY;

typedef struct _CMEMORYQUERY {
	HANDLE			eProcess;					// _eProcess
	PVOID			address;					// 查询的虚拟地址
	DWORD			length;						// 查询的长度
	UCHAR			value[1];					// 值
}CMEMORYQUERY, * PCMEMORYQUERY;

typedef struct _CREGISTERINFO {
	ULONG			bType;						// 键/值（类型）
	ULONG			DateLength;					// 数据长度
	WCHAR			szName[MAX_PATH];			// 名称
	UCHAR			ucDate[MAX_PATH];			// 数据
}CREGISTERINFO, * PCREGISTERINFO;