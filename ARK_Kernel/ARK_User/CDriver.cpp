#include "pch.h"
#include "CDriver.h"

CMyDriver theDriver;

void DbgPrint(LPCSTR format, ...)
{
	CHAR szbufFormat[0x100];
	va_list argList;												//指向参数的指针
	va_start(argList, format);										//参数列表初始化
	vsprintf_s(szbufFormat, format, argList);						//使用参数列表发送格式化输出到字符串
	va_end(argList);												//参数列表结束
	OutputDebugStringA(szbufFormat);								//输出调试信息
}

BOOL CDriver::Install(PCTSTR pSysPath, PCTSTR pServiceName, PCTSTR pDisplayName)
{
	m_pSysPath = pSysPath;
	m_pServiceName = pServiceName;
	m_pDisplayName = pDisplayName;
	m_hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (NULL == m_hSCManager)
	{
		m_dwLastError = GetLastError();
		return FALSE;
	}

	m_hService = CreateService(
		m_hSCManager,
		m_pServiceName,
		m_pDisplayName,
		SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		m_pSysPath,
		NULL, NULL, NULL, NULL, NULL
	);

	if (NULL == m_hService)
	{
		m_dwLastError = GetLastError();
		if (ERROR_SERVICE_EXISTS == m_dwLastError)
		{
			m_hService = OpenService(m_hSCManager, m_pServiceName, SERVICE_ALL_ACCESS);
			if (NULL == m_hService)
			{
				CloseServiceHandle(m_hSCManager);
				return FALSE;
			}
		}
		else
		{
			CloseServiceHandle(m_hSCManager);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CDriver::Start()
{
	if (!StartService(m_hService, NULL, NULL))
	{
		m_dwLastError = GetLastError();
		return FALSE;
	}

	return TRUE;
}

BOOL CDriver::Stop()
{
	SERVICE_STATUS ss;
	RefreshSvcHandle();
	if (!ControlService(m_hService, SERVICE_CONTROL_STOP, &ss))
	{
		m_dwLastError = GetLastError();
		return FALSE;
	}
	return TRUE;
}

BOOL CDriver::Remove()
{
	RefreshSvcHandle();
	if (!DeleteService(m_hService))
	{
		m_dwLastError = GetLastError();
		return FALSE;
	}
	return TRUE;
}

BOOL CDriver::Open(PCTSTR pLinkName)
{
	m_hDriver = CreateFile(
		pLinkName,
		GENERIC_ALL,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (m_hDriver != INVALID_HANDLE_VALUE)
		return TRUE;
	else
		return FALSE;
}

BOOL CDriver::Close()
{
	if (m_hDriver != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hDriver);
		m_hDriver = INVALID_HANDLE_VALUE;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CDriver::RefreshSvcHandle()
{
	m_hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == m_hSCManager)
	{
		m_dwLastError = GetLastError();
		return FALSE;
	}

	m_hService = OpenService(m_hSCManager, m_pServiceName, SERVICE_ALL_ACCESS);
	if (NULL == m_hService)
	{
		CloseServiceHandle(m_hSCManager);
		return FALSE;
	}

	return TRUE;
}

ULONG CMyDriver::FileSearch(PWSTR szFilePath)
{
	DWORD dwBytes;
	DWORD dwEntryNum = NULL;

	DeviceIoControl(m_hDriver, IOCTL_FILE_FIND, szFilePath, (DWORD)((wcslen(szFilePath) + 1) * 2), &dwEntryNum, sizeof(DWORD), &dwBytes, NULL);

	return dwEntryNum;
}

PCFILEINFO CMyDriver::GetFileInfo(DWORD dwEntryNum)
{
	DWORD dwBytes;
	DWORD dwBufferSize = sizeof(CFILEINFO) * dwEntryNum;
	PCFILEINFO pEntryInfo = (PCFILEINFO)malloc(dwBufferSize);

	DeviceIoControl(m_hDriver, IOCTL_FILE_INFO, NULL, NULL, pEntryInfo, dwBufferSize, &dwBytes, NULL);

	return pEntryInfo;
}

DWORD CMyDriver::SSDTSearch()
{
	DWORD dwBytes;
	DWORD dwEntryNum = NULL;

	DeviceIoControl(m_hDriver, IOCTL_SSDT_FIND, NULL, NULL, &dwEntryNum, sizeof(DWORD), &dwBytes, NULL);

	return dwEntryNum;
}

PCSSDTINFO CMyDriver::GetSSDTInfo(DWORD dwEntryNum)
{
	DWORD dwBytes;
	DWORD dwBufferSize = sizeof(CSSDTINFO) * dwEntryNum;
	PCSSDTINFO pEntryInfo = (PCSSDTINFO)malloc(dwBufferSize);

	DeviceIoControl(m_hDriver, IOCTL_SSDT_INFO, NULL, NULL, pEntryInfo, dwBufferSize, &dwBytes, NULL);

	return pEntryInfo;
}

DWORD CMyDriver::DriverSearch()
{
	DWORD dwBytes;
	DWORD dwEntryNum = NULL;

	DeviceIoControl(m_hDriver, IOCTL_DRIVER_FIND, NULL, NULL, &dwEntryNum, sizeof(DWORD), &dwBytes, NULL);

	return dwEntryNum;
}

PCMODULE CMyDriver::GetDriverInfo(DWORD dwEntryNum)
{
	DWORD dwBytes;
	DWORD dwBufferSize = sizeof(CMODULE) * dwEntryNum;
	PCMODULE pEntryInfo = (PCMODULE)malloc(dwBufferSize);

	DeviceIoControl(m_hDriver, IOCTL_DRIVER_INFO, NULL, NULL, pEntryInfo, dwBufferSize, &dwBytes, NULL);

	return pEntryInfo;
}

DWORD CMyDriver::ProcessSearch()
{
	DWORD dwBytes;
	DWORD dwEntryNum = NULL;

	DeviceIoControl(m_hDriver, IOCTL_PROCESS_FIND, NULL, NULL, &dwEntryNum, sizeof(DWORD), &dwBytes, NULL);

	return dwEntryNum;
}

PCPROCESS CMyDriver::GetProcessInfo(DWORD dwEntryNum)
{
	DWORD dwBytes;
	DWORD dwBufferSize = sizeof(CPROCESS) * dwEntryNum;
	PCPROCESS pEntryInfo = (PCPROCESS)malloc(dwBufferSize);

	DeviceIoControl(m_hDriver, IOCTL_PROCESS_INFO, NULL, NULL, pEntryInfo, dwBufferSize, &dwBytes, NULL);

	return pEntryInfo;
}

DWORD CMyDriver::ThreadSearch(HANDLE eProcess)
{
	DWORD dwBytes;
	DWORD dwEntryNum = NULL;

	DeviceIoControl(m_hDriver, IOCTL_THREAD_FIND, &eProcess, sizeof(HANDLE), &dwEntryNum, sizeof(DWORD), &dwBytes, NULL);

	return dwEntryNum;
}

PCTHREAD CMyDriver::GetThreadInfo(DWORD dwEntryNum)
{
	DWORD dwBytes;
	DWORD dwBufferSize = sizeof(CTHREAD) * dwEntryNum;
	PCTHREAD pEntryInfo = (PCTHREAD)malloc(dwBufferSize);

	DeviceIoControl(m_hDriver, IOCTL_THREAD_INFO, NULL, NULL, pEntryInfo, dwBufferSize, &dwBytes, NULL);

	return pEntryInfo;
}

DWORD CMyDriver::ModuleSearch(HANDLE eProcess)
{
	DWORD dwBytes;
	DWORD dwEntryNum = NULL;

	DeviceIoControl(m_hDriver, IOCTL_MODULE_FIND, &eProcess, sizeof(HANDLE), &dwEntryNum, sizeof(DWORD), &dwBytes, NULL);

	return dwEntryNum;
}

PCMODULE CMyDriver::GetModuleInfo(DWORD dwEntryNum)
{
	DWORD dwBytes;
	DWORD dwBufferSize = sizeof(CMODULE) * dwEntryNum;
	PCMODULE pEntryInfo = (PCMODULE)malloc(dwBufferSize);

	DeviceIoControl(m_hDriver, IOCTL_MODULE_INFO, NULL, NULL, pEntryInfo, dwBufferSize, &dwBytes, NULL);

	return pEntryInfo;
}

DWORD CMyDriver::RegisterSearch(PWSTR szRegPath)
{
	DWORD dwBytes;
	DWORD dwEntryNum = NULL;

	DeviceIoControl(m_hDriver, IOCTL_REGISTER_FIND, szRegPath, (DWORD)((wcslen(szRegPath) + 1) * 2), &dwEntryNum, sizeof(DWORD), &dwBytes, NULL);

	return dwEntryNum;
}

PCREGISTERINFO CMyDriver::GetRegisterInfo(DWORD dwEntryNum)
{
	DWORD dwBytes;
	DWORD dwBufferSize = sizeof(CREGISTERINFO) * dwEntryNum;
	PCREGISTERINFO pEntryInfo = (PCREGISTERINFO)malloc(dwBufferSize);

	DeviceIoControl(m_hDriver, IOCTL_REGISTER_INFO, NULL, NULL, pEntryInfo, dwBufferSize, &dwBytes, NULL);

	return pEntryInfo;
}




void CMyDriver::KillProcess(HANDLE pid)
{
	DWORD dwBytes;
	DeviceIoControl(m_hDriver, IOCTL_PROCESS_CLOSE, &pid, sizeof(HANDLE), NULL, NULL, &dwBytes, NULL);
}

void CMyDriver::MyDeleteFile(PWSTR szFilePath)
{
	DWORD dwBytes;
	DeviceIoControl(m_hDriver, IOCTL_FILE_DELETE, szFilePath, (DWORD)((wcslen(szFilePath) + 1) * 2), NULL, NULL, &dwBytes, NULL);
}

PCGDTINFO CMyDriver::GetGDTInfo(PDWORD dwEntryNum)
{
	DWORD dwBufferSize = 0x1000 / 8 * sizeof(CGDTINFO);
	PCGDTINFO pEntryInfo = (PCGDTINFO)malloc(dwBufferSize);

	DeviceIoControl(m_hDriver, IOCTL_GDT_FIND, NULL, NULL, pEntryInfo, dwBufferSize, dwEntryNum, NULL);
	*dwEntryNum = *dwEntryNum / sizeof(CGDTINFO);

	return pEntryInfo;
}

PCIDTINFO CMyDriver::GetIDTInfo(PDWORD dwEntryNum)
{
	DWORD dwBufferSize = 0x1000 / 16 * sizeof(CIDTINFO);
	PCIDTINFO pEntryInfo = (PCIDTINFO)malloc(dwBufferSize);

	DeviceIoControl(m_hDriver, IOCTL_IDT_FIND, NULL, NULL, pEntryInfo, dwBufferSize, dwEntryNum, NULL);
	*dwEntryNum = *dwEntryNum / sizeof(CIDTINFO);

	return pEntryInfo;
}

PCPAGEINFO CMyDriver::GetPageInfo(PCPAGEQUERY pQuery, PDWORD dwEntryNum)
{
	DWORD dwBufferSize = 0x200 * sizeof(CPAGEINFO);
	PCPAGEINFO pEntryInfo = (PCPAGEINFO)malloc(dwBufferSize);

	DeviceIoControl(m_hDriver, IOCTL_PAGE_FIND, pQuery, sizeof(CPAGEQUERY), pEntryInfo, dwBufferSize, dwEntryNum, NULL);
	*dwEntryNum = *dwEntryNum / sizeof(CPAGEINFO);

	return pEntryInfo;
}

PUCHAR CMyDriver::ReadMemory(PCMEMORYQUERY pQuery)
{
	DWORD dwBytes;
	PUCHAR pBuffer = (PUCHAR)malloc(pQuery->length);

	DeviceIoControl(m_hDriver, IOCTL_MEMORY_READ, pQuery, sizeof(CMEMORYQUERY), pBuffer, pQuery->length, &dwBytes, NULL);

	return pBuffer;
}

void CMyDriver::WriteMemory(PCMEMORYQUERY pQuery)
{
	DWORD dwBytes;
	DWORD dwBufferSize = sizeof(CMEMORYQUERY) + pQuery->length;

	DeviceIoControl(m_hDriver, IOCTL_MEMORY_WRITE, pQuery, dwBufferSize, NULL, NULL, &dwBytes, NULL);
}

