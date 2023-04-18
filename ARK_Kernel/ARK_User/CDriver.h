#pragma once

class CDriver
{
public:
	CDriver()
	{
		m_dwLastError = NULL;
		m_hSCManager = nullptr;
		m_hService = nullptr;
		m_hDriver = INVALID_HANDLE_VALUE;
	}
	virtual ~CDriver()
	{
		if (m_hService)								CloseServiceHandle(m_hService);
		if (m_hSCManager)							CloseServiceHandle(m_hSCManager);
		if (m_hDriver != INVALID_HANDLE_VALUE)		CloseHandle(m_hDriver);
	}

public:
	BOOL Install(PCTSTR pSysPath, PCTSTR pServiceName, PCTSTR pDisplayName);
	BOOL Start();
	BOOL Stop();
	BOOL Remove();
	BOOL Open(PCTSTR pLinkName);
	BOOL Close();
	BOOL RefreshSvcHandle();

protected:
	DWORD		m_dwLastError;
	CString		m_pSysPath;
	CString		m_pServiceName;
	CString		m_pDisplayName;
	HANDLE		m_hDriver;
	SC_HANDLE	m_hSCManager;
	SC_HANDLE	m_hService;
};

class CMyDriver
	: public CDriver
{
public:
	CMyDriver() {}
	virtual ~CMyDriver() {}

public:
	// �ļ�
	DWORD			FileSearch(PWSTR szFilePath);
	PCFILEINFO		GetFileInfo(DWORD dwEntryNum);

	// SSDT
	DWORD			SSDTSearch();
	PCSSDTINFO		GetSSDTInfo(DWORD dwEntryNum);

	// ����ģ��
	DWORD			DriverSearch();
	PCMODULE		GetDriverInfo(DWORD dwEntryNum);

	// ����
	DWORD			ProcessSearch();
	PCPROCESS		GetProcessInfo(DWORD dwEntryNum);

	// �߳�
	DWORD			ThreadSearch(HANDLE eProcess);
	PCTHREAD		GetThreadInfo(DWORD dwEntryNum);

	// ģ��
	DWORD			ModuleSearch(HANDLE eProcess);
	PCMODULE		GetModuleInfo(DWORD dwEntryNum);

	PCGDTINFO		GetGDTInfo(PDWORD dwEntryNum);							// GDT
	PCIDTINFO		GetIDTInfo(PDWORD dwEntryNum);							// IDT
	PCPAGEINFO		GetPageInfo(PCPAGEQUERY pQuery, PDWORD dwEntryNum);		// ҳ��

	// �ڴ�
	PUCHAR			ReadMemory(PCMEMORYQUERY pQuery);
	void			WriteMemory(PCMEMORYQUERY pQuery);

	// ע���
	DWORD			RegisterSearch(PWSTR szPath);
	PCREGISTERINFO	GetRegisterInfo(DWORD dwEntryNum);

	// ��������
	void			KillProcess(HANDLE pid);

	// ɾ���ļ�
	void			MyDeleteFile(PWSTR szFilePath);
};

extern CMyDriver theDriver;
void DbgPrint(LPCSTR format, ...);