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
	LARGE_INTEGER	qwFileSize;					// �ļ���С
	LARGE_INTEGER	qwChangeTime;				// �ļ��޸�ʱ���
	BOOLEAN			bFileType;					// �ļ�����
	WCHAR			szFileName[MAX_PATH];		// �ļ���
}CFILEINFO, * PCFILEINFO;

typedef struct _CPROCESS {
	PVOID			pEProcess;					// ���̶���
	HANDLE			hPID;						// ����ID
	HANDLE			hPPID;						// ������ID
	WCHAR			szProcessName[MAX_PATH];	// ������
}CPROCESS, * PCPROCESS;

typedef struct _CTHREAD {
	HANDLE			hTID;						// �߳�ID
	PVOID			pEThread;					// �̵߳���ʼ��ַ
	UCHAR			dwBasePriority;				// ���ȼ�
	UCHAR			dwStatus;					// ״̬
}CTHREAD, * PCTHREAD;

typedef struct _CMODULE {
	WCHAR			szFullDllName[MAX_PATH];	// ģ������·��
	WCHAR			szBaseDllName[MAX_PATH];	// ģ����
	PVOID			pDllBase;					// ģ�����ַ
	PVOID			pEntryPoint;				// ģ����ڵ�
	ULONG			dwSize;						// ģ���С
}CMODULE, * PCMODULE;

typedef struct _CSSDTINFO {
	WCHAR			szFunName[MAX_PATH];		// ������
	ULONG			dwFunNum;					// ���
	ULONGLONG		qwFunaddr;					// ��ַ
}CSSDTINFO, * PCSSDTINFO;

typedef struct _CIDTINFO {
	ULONG			dwIndex;					// �±�
	ULONGLONG		qwFunaddr;					// ������ַ
	USHORT			wDPL;						// ��Ȩ����
	USHORT			wTargetSelector;			// ѡ����
} CIDTINFO, * PCIDTINFO;

typedef struct _CGDTINFO {
	ULONG			dwIndex;					// �±�
	ULONG			dwLimit;					// ��Χ
	ULONGLONG		qwBase;						// ��ַ
	USHORT			wType;						// ����
	USHORT			wDPL;						// ��Ȩ����
	BOOLEAN			bS;							// S = 0 ϵͳ�� S = 1 ����/���ݶ�
	BOOLEAN			bL;							// L = 0 x32 , L = 1 x64
}CGDTINFO, * PCGDTINFO;

typedef struct _CPAGEINFO {
	BOOLEAN			bRW;						// �ܷ�д��
	BOOLEAN			bAccess;					// �Ƿ񱻷���
	BOOLEAN			bUS;						// Ȩ��
	BOOLEAN			bNX;						// �ܷ�ִ��
	USHORT			wIndex;						// ��ǰ�������±�
	ULONGLONG		qwAddress;					// �¼��������ַ
}CPAGEINFO, * PCPAGEINFO;

typedef struct _CPAGEINDEX {
	ULONGLONG		wPXEIndex : 9;				// PXE �±�
	ULONGLONG		wPPEIndex : 9;				// PPE �±�
	ULONGLONG		wPDEIndex : 9;				// PDE �±�
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
	CPAGEINDEX		Index;						// ��ѯҳ���±�
	PAGE_TYPE		Type;						// ��ѯҳ������
}CPAGEQUERY, * PCPAGEQUERY;

typedef struct _CMEMORYQUERY {
	HANDLE			eProcess;					// _eProcess
	PVOID			address;					// ��ѯ�������ַ
	DWORD			length;						// ��ѯ�ĳ���
	UCHAR			value[1];					// ֵ
}CMEMORYQUERY, * PCMEMORYQUERY;

typedef struct _CREGISTERINFO {
	ULONG			bType;						// ��/ֵ�����ͣ�
	ULONG			DateLength;					// ���ݳ���
	WCHAR			szName[MAX_PATH];			// ����
	UCHAR			ucDate[MAX_PATH];			// ����
}CREGISTERINFO, * PCREGISTERINFO;