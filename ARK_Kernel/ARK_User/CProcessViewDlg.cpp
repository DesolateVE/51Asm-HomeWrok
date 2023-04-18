// CProcessViewDlg.cpp: 实现文件
//

#include "pch.h"
#include "ARK_User.h"
#include "afxdialogex.h"
#include "CProcessViewDlg.h"


// CProcessViewDlg 对话框

IMPLEMENT_DYNAMIC(CProcessViewDlg, CDialogEx)

CProcessViewDlg::CProcessViewDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROCESS_TAP, pParent)
{
	m_eProcess = nullptr;
}

CProcessViewDlg::~CProcessViewDlg()
{
}

void CProcessViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ProcessList);
	DDX_Control(pDX, IDC_LIST3, m_ThreadList);
	DDX_Control(pDX, IDC_LIST4, m_ModuleList);
	DDX_Control(pDX, IDC_LIST5, m_PXEList);
	DDX_Control(pDX, IDC_LIST6, m_PPEList);
	DDX_Control(pDX, IDC_LIST7, m_PDEList);
	DDX_Control(pDX, IDC_LIST8, m_PTEList);
	DDX_Control(pDX, IDC_EDIT1, m_HexValueEdit);
	DDX_Control(pDX, IDC_EDIT2, m_AddressEdit);
	DDX_Control(pDX, IDC_EDIT4, m_ValueEdit);
}


void CProcessViewDlg::UpdatePageList(PCPAGEINFO pInfo, DWORD nEntryNum, CListCtrl* pList)
{
	CString szTemp;

	pList->DeleteAllItems();
	for (size_t i = 0; i < nEntryNum; i++)
	{
		ULONG k = 1;

		szTemp.Format(L"%04X", pInfo[i].wIndex);
		pList->InsertItem(i, szTemp);

		szTemp.Format(L"%X", pInfo[i].bRW);
		pList->SetItemText(i, k++, szTemp);

		szTemp.Format(L"%X", pInfo[i].bAccess);
		pList->SetItemText(i, k++, szTemp);

		szTemp.Format(L"%X", pInfo[i].bUS);
		pList->SetItemText(i, k++, szTemp);

		szTemp.Format(L"%08X", pInfo[i].qwAddress);
		pList->SetItemText(i, k++, szTemp);

		szTemp.Format(L"%X", pInfo[i].bNX);
		pList->SetItemText(i, k++, szTemp);
	}
	free(pInfo);
}

void CProcessViewDlg::SetPageListStyle(CListCtrl* pListCtrl)
{
	DWORD dwStyle;
	DWORD nNum;
	DWORD dwWidth;
	CRect ListCtrlRect;

	dwStyle = pListCtrl->GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	pListCtrl->SetExtendedStyle(dwStyle);
	pListCtrl->GetClientRect(ListCtrlRect);
	dwWidth = ListCtrlRect.Width() / 10;
	nNum = 0;
	pListCtrl->InsertColumn(nNum++, L"Index", LVCFMT_LEFT, dwWidth * 1);
	pListCtrl->InsertColumn(nNum++, L"RW", LVCFMT_LEFT, dwWidth * 1);
	pListCtrl->InsertColumn(nNum++, L"Access", LVCFMT_LEFT, dwWidth * 1);
	pListCtrl->InsertColumn(nNum++, L"US", LVCFMT_LEFT, dwWidth * 1);
	pListCtrl->InsertColumn(nNum++, L"Address", LVCFMT_LEFT, dwWidth * 5);
	pListCtrl->InsertColumn(nNum++, L"NX", LVCFMT_LEFT, ListCtrlRect.Width() - dwWidth * 9);
}

void CProcessViewDlg::UpdateHexEdit(PBYTE pByte)
{
	CString szHexEdit;
	CString szTemp;

	for (size_t i = 0; i < hexHeight; i++)
	{
		szTemp.Format(L"+ %02X |    ", i * hexWidth);
		szHexEdit += szTemp;
		for (size_t k = 0; k < hexWidth; k++)
		{
			szTemp.Format(L"%02X  ", pByte[i * hexWidth + k]);
			szHexEdit += szTemp;
		}
		szHexEdit += L"\r\n";
	}
	m_HexValueEdit.SetWindowTextW(szHexEdit);

	szTemp.Format(L"%p", *(PUINT64)pByte);
	m_ValueEdit.SetWindowTextW(szTemp);

	free(pByte);
}

int CALLBACK CProcessViewDlg::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CListCtrl* pListCtrl = (CListCtrl*)lParamSort;
	CString    strItem1 = pListCtrl->GetItemText(static_cast<int>(lParam1), 0);
	CString    strItem2 = pListCtrl->GetItemText(static_cast<int>(lParam2), 0);

	return strItem1.CompareNoCase(strItem2);
}

BEGIN_MESSAGE_MAP(CProcessViewDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_COMMAND(ID_NOTHING_32772, &CProcessViewDlg::OnClickKill)
	ON_BN_CLICKED(IDC_BUTTON1, &CProcessViewDlg::OnBnClickedRead)
	ON_BN_CLICKED(IDC_BUTTON2, &CProcessViewDlg::OnBnClickedWrite)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CProcessViewDlg::OnDblclkProcessList)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CProcessViewDlg::OnRclickProcessList)
END_MESSAGE_MAP()


// CProcessViewDlg 消息处理程序


void CProcessViewDlg::OnBnClickedRead()
{
	// TODO: 在此添加控件通知处理程序代码

	CString szTemp;
	PWCHAR	szEnd;
	QWORD LineAddr;

	m_AddressEdit.GetWindowTextW(szTemp);
	LineAddr = _wcstoui64(szTemp, &szEnd, 16);

	if (!LineAddr || !m_eProcess)
	{
		return;
	}

	LinearAddress Input(LineAddr);
	CPAGEQUERY PageQuerry;
	DWORD EntryNum;
	PCPAGEINFO pPageInfo;

	PageQuerry.eProcess = m_eProcess;
	PageQuerry.Index.wPXEIndex = Input.getPML4();
	PageQuerry.Index.wPPEIndex = Input.getDirectoryPtr();
	PageQuerry.Index.wPDEIndex = Input.getDirectory();

	PageQuerry.Type = PAGE_PXE;
	pPageInfo = theDriver.GetPageInfo(&PageQuerry, &EntryNum);
	UpdatePageList(pPageInfo, EntryNum, &m_PXEList);

	PageQuerry.Type = PAGE_PPE;
	pPageInfo = theDriver.GetPageInfo(&PageQuerry, &EntryNum);
	UpdatePageList(pPageInfo, EntryNum, &m_PPEList);

	PageQuerry.Type = PAGE_PDE;
	pPageInfo = theDriver.GetPageInfo(&PageQuerry, &EntryNum);
	UpdatePageList(pPageInfo, EntryNum, &m_PDEList);

	PageQuerry.Type = PAGE_PTE;
	pPageInfo = theDriver.GetPageInfo(&PageQuerry, &EntryNum);
	UpdatePageList(pPageInfo, EntryNum, &m_PTEList);



	CMEMORYQUERY MemoryQuery;
	MemoryQuery.address = (PVOID)LineAddr;
	MemoryQuery.eProcess = m_eProcess;
	MemoryQuery.length = hexWidth * hexHeight;
	PUCHAR pByte = theDriver.ReadMemory(&MemoryQuery);
	UpdateHexEdit(pByte);
}


void CProcessViewDlg::OnBnClickedWrite()
{
	// TODO: 在此添加控件通知处理程序代码

	CString szTemp;
	PWCHAR	szEnd;
	QWORD LineAddr;
	QWORD InputValue;

	m_AddressEdit.GetWindowTextW(szTemp);
	LineAddr = _wcstoui64(szTemp, &szEnd, 16);
	m_ValueEdit.GetWindowTextW(szTemp);
	InputValue = _wcstoui64(szTemp, &szEnd, 16);

	if (!LineAddr || !m_eProcess)
	{
		return;
	}

	PCMEMORYQUERY pMemoryQuery = (PCMEMORYQUERY)malloc(sizeof(CMEMORYQUERY) + 8);
	pMemoryQuery->address = (PVOID)LineAddr;
	pMemoryQuery->eProcess = m_eProcess;
	pMemoryQuery->length = 8;
	*((PUINT64)pMemoryQuery->value) = InputValue;
	theDriver.WriteMemory(pMemoryQuery);
	free(pMemoryQuery);

	CMEMORYQUERY MemoryQuery;
	MemoryQuery.address = (PVOID)LineAddr;
	MemoryQuery.eProcess = m_eProcess;
	MemoryQuery.length = hexWidth * hexHeight;
	PUCHAR pByte = theDriver.ReadMemory(&MemoryQuery);
	UpdateHexEdit(pByte);
}


void CProcessViewDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (!bShow)
	{
		return;
	}

	UpdateProcessList();

	// TODO: 在此处添加消息处理程序代码
}

void CProcessViewDlg::UpdateProcessList()
{
	m_ProcessList.DeleteAllItems();
	DWORD EntryNum = theDriver.ProcessSearch();
	PCPROCESS pInfo = theDriver.GetProcessInfo(EntryNum);
	CString szTemp;

	for (size_t i = 0; i < EntryNum; i++)
	{
		ULONG k = 1;

		szTemp = pInfo[i].szProcessName;
		if ((ULONG)pInfo[i].hPID == 4) { szTemp = "System"; }
		m_ProcessList.InsertItem(i, szTemp.Right(szTemp.GetLength() - szTemp.ReverseFind('\\') - 1));

		szTemp.Format(L"%04X", pInfo[i].hPID);
		m_ProcessList.SetItemText(i, k++, szTemp);

		szTemp.Format(L"%04X", pInfo[i].hPPID);
		m_ProcessList.SetItemText(i, k++, szTemp);

		szTemp.Format(L"%p", pInfo[i].pEProcess);
		m_ProcessList.SetItemText(i, k++, szTemp);
	}
	free(pInfo);

	m_ProcessList.SortItemsEx(CompareFunc, (DWORD_PTR)&m_ProcessList);
}


BOOL CProcessViewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	CRect ListCtrlRect;
	DWORD dwWidth;
	DWORD nNum;
	DWORD dwStyle;
	CListCtrl* pListCtrl;

	pListCtrl = &m_ProcessList;
	dwStyle = pListCtrl->GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	pListCtrl->SetExtendedStyle(dwStyle);
	pListCtrl->GetClientRect(ListCtrlRect);
	dwWidth = ListCtrlRect.Width() / 20;
	nNum = 0;
	pListCtrl->InsertColumn(nNum++, L"Name", LVCFMT_LEFT, dwWidth * 7);
	pListCtrl->InsertColumn(nNum++, L"PID", LVCFMT_LEFT, dwWidth * 3);
	pListCtrl->InsertColumn(nNum++, L"PPID", LVCFMT_LEFT, dwWidth * 3);
	pListCtrl->InsertColumn(nNum++, L"EProcess", LVCFMT_LEFT, ListCtrlRect.Width() - dwWidth * 13);

	pListCtrl = &m_ThreadList;
	dwStyle = pListCtrl->GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	pListCtrl->SetExtendedStyle(dwStyle);
	pListCtrl->GetClientRect(ListCtrlRect);
	dwWidth = ListCtrlRect.Width() / 8;
	nNum = 0;
	pListCtrl->InsertColumn(nNum++, L"ID", LVCFMT_LEFT, dwWidth * 2);
	pListCtrl->InsertColumn(nNum++, L"Priority", LVCFMT_LEFT, dwWidth * 2);
	pListCtrl->InsertColumn(nNum++, L"Status", LVCFMT_LEFT, dwWidth * 2);
	pListCtrl->InsertColumn(nNum++, L"EThread", LVCFMT_LEFT, ListCtrlRect.Width() - dwWidth * 6);

	pListCtrl = &m_ModuleList;
	dwStyle = pListCtrl->GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	pListCtrl->SetExtendedStyle(dwStyle);
	pListCtrl->GetClientRect(ListCtrlRect);
	dwWidth = ListCtrlRect.Width() / 20;
	nNum = 0;
	pListCtrl->InsertColumn(nNum++, L"模块名", LVCFMT_LEFT, dwWidth * 3);
	pListCtrl->InsertColumn(nNum++, L"模块路径", LVCFMT_LEFT, dwWidth * 6);
	pListCtrl->InsertColumn(nNum++, L"模块基址", LVCFMT_LEFT, dwWidth * 4);
	pListCtrl->InsertColumn(nNum++, L"模块入口", LVCFMT_LEFT, dwWidth * 4);
	pListCtrl->InsertColumn(nNum++, L"大小", LVCFMT_LEFT, ListCtrlRect.Width() - dwWidth * 17);

	SetPageListStyle(&m_PXEList);
	SetPageListStyle(&m_PPEList);
	SetPageListStyle(&m_PDEList);
	SetPageListStyle(&m_PTEList);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CProcessViewDlg::OnClickKill()
{
	theDriver.KillProcess(m_pid);
	UpdateProcessList();
}

void CProcessViewDlg::OnDblclkProcessList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	PWCHAR szEnd;
	CString szTemp;
	HANDLE eProcess;
	DWORD EntryNum;

	szTemp = m_ProcessList.GetItemText(pNMItemActivate->iItem, 3);
	eProcess = (HANDLE)_wcstoui64(szTemp.GetBuffer(), &szEnd, 16);
	m_eProcess = eProcess;

	m_ThreadList.DeleteAllItems();
	EntryNum = theDriver.ThreadSearch(eProcess);
	PCTHREAD pThreadInfo = theDriver.GetThreadInfo(EntryNum);
	for (size_t i = 0; i < EntryNum; i++)
	{
		ULONG k = 1;

		szTemp.Format(L"%04X", pThreadInfo[i].hTID);
		m_ThreadList.InsertItem(i, szTemp);

		szTemp.Format(L"%04X", pThreadInfo[i].dwBasePriority);
		m_ThreadList.SetItemText(i, k++, szTemp);

		szTemp.Format(L"%04X", pThreadInfo[i].dwStatus);
		m_ThreadList.SetItemText(i, k++, szTemp);

		szTemp.Format(L"%p", pThreadInfo[i].pEThread);
		m_ThreadList.SetItemText(i, k++, szTemp);
	}
	free(pThreadInfo);

	m_ModuleList.DeleteAllItems();
	EntryNum = theDriver.ModuleSearch(eProcess);
	PCMODULE pModuleInfo = theDriver.GetModuleInfo(EntryNum);
	for (size_t i = 0; i < EntryNum; i++)
	{
		ULONG k = 1;

		m_ModuleList.InsertItem(i, pModuleInfo[i].szBaseDllName);
		m_ModuleList.SetItemText(i, k++, pModuleInfo[i].szFullDllName);

		szTemp.Format(L"%p", pModuleInfo[i].pDllBase);
		m_ModuleList.SetItemText(i, k++, szTemp);

		szTemp.Format(L"%p", pModuleInfo[i].pEntryPoint);
		m_ModuleList.SetItemText(i, k++, szTemp);

		szTemp.Format(L"%i", pModuleInfo[i].dwSize);
		m_ModuleList.SetItemText(i, k++, szTemp);
	}
	free(pModuleInfo);

}

void CProcessViewDlg::OnRclickProcessList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	PWCHAR strEnd;
	m_pid = (HANDLE)_wcstoui64(m_ProcessList.GetItemText(pNMItemActivate->iItem, 1), &strEnd, 16);

	CPoint point(pNMItemActivate->ptAction);
	m_ProcessList.ClientToScreen(&point);

	CMenu mn;
	mn.LoadMenu(IDR_MENU2);      //加载的资源ID
	CMenu* pSubMn = mn.GetSubMenu(0);
	pSubMn->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}
