// CStartupViewDlg.cpp: 实现文件
//

#include "pch.h"
#include "ARK_User.h"
#include "afxdialogex.h"
#include "CStartupViewDlg.h"


// CStartupViewDlg 对话框

IMPLEMENT_DYNAMIC(CStartupViewDlg, CDialogEx)

CStartupViewDlg::CStartupViewDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_STARTUP_LIST, pParent)
{

}

CStartupViewDlg::~CStartupViewDlg()
{
}

void CStartupViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_AppList);
	DDX_Control(pDX, IDC_LIST3, m_ServerList);
}


void CStartupViewDlg::addServerList(PWCHAR szName)
{
	CString szTemp = L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services\\";
	szTemp += szName;
	DWORD nEntry = theDriver.RegisterSearch(szTemp.GetBuffer());
	PCREGISTERINFO pInfo = theDriver.GetRegisterInfo(nEntry);

	bool Type = false;
	CString szStartType;
	CString szImagePath;
	CString	szDescription;

	for (size_t i = 0; i < nEntry; i++)
	{
		if (pInfo[i].bType == 0xFFFF)
		{
			continue;
		}

		if (!wcscmp(pInfo[i].szName, L"Type"))
		{
			if (*(PDWORD)pInfo->ucDate == 0x10)
			{
				Type = true;
			}
		}

		if (!wcscmp(pInfo[i].szName, L"ImagePath"))
		{
			szImagePath.Format(L"%s", pInfo[i].ucDate);
		}

		if (!wcscmp(pInfo[i].szName, L"Start"))
		{
			switch (*(PDWORD)pInfo[i].ucDate)
			{
			case 1:
				szStartType = L"延迟启动";
				break;
			case 2:
				szStartType = L"自动";
				break;
			case 3:
				szStartType = L"手动";
				break;
			case 4:
				szStartType = L"禁用";
				break;
			}
		}

		if (!wcscmp(pInfo[i].szName, L"Description"))
		{
			szDescription.Format(L"%s", pInfo[i].ucDate);
		}

	}
	free(pInfo);

	if (Type)
	{
		m_ServerList.InsertItem(0, szName);
		m_ServerList.SetItemText(0, 1, szDescription);
		m_ServerList.SetItemText(0, 2, szStartType);
		m_ServerList.SetItemText(0, 3, szImagePath);
	}

}

BEGIN_MESSAGE_MAP(CStartupViewDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CStartupViewDlg 消息处理程序


void CStartupViewDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码
	DWORD nEntry = theDriver.RegisterSearch(L"\\Registry\\Machine\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
	PCREGISTERINFO pInfo = theDriver.GetRegisterInfo(nEntry);

	m_AppList.DeleteAllItems();
	m_ServerList.DeleteAllItems();

	DWORD nRow = 0;
	for (size_t i = 0; i < nEntry; i++)
	{
		if (pInfo[i].bType == REG_SZ)
		{
			m_AppList.InsertItem(nRow, pInfo[i].szName);
			m_AppList.SetItemText(nRow, 1, (PWCHAR)pInfo[i].ucDate);
			nRow++;
		}
	}
	free(pInfo);

	nEntry = theDriver.RegisterSearch(L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services\\");
	pInfo = theDriver.GetRegisterInfo(nEntry);

	for (size_t i = 0; i < nEntry; i++)
	{
		if (pInfo[i].bType == 0xFFFF)
		{
			addServerList(pInfo[i].szName);
		}
	}
	free(pInfo);

}


BOOL CStartupViewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	DWORD dwStyle = m_AppList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;							//选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;								//网格线（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_SUBITEMIMAGES;
	m_AppList.SetExtendedStyle(dwStyle);						//设置扩展风格

	CRect ListCtrlRect;
	m_AppList.GetClientRect(ListCtrlRect);
	DWORD dwWidth = ListCtrlRect.Width() / 10;

	m_AppList.InsertColumn(0, L"名称", LVCFMT_LEFT, dwWidth * 4);
	m_AppList.InsertColumn(1, L"路径", LVCFMT_LEFT, ListCtrlRect.Width() - dwWidth * 4);


	dwStyle = m_ServerList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;							//选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;								//网格线（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_SUBITEMIMAGES;
	m_ServerList.SetExtendedStyle(dwStyle);						//设置扩展风格

	m_ServerList.GetClientRect(ListCtrlRect);
	dwWidth = ListCtrlRect.Width() / 10;

	m_ServerList.InsertColumn(0, L"名称", LVCFMT_LEFT, dwWidth * 3);
	m_ServerList.InsertColumn(1, L"描述", LVCFMT_LEFT, dwWidth * 6);
	m_ServerList.InsertColumn(2, L"启动类型", LVCFMT_LEFT, dwWidth * 1);
	m_ServerList.InsertColumn(3, L"路径", LVCFMT_LEFT, dwWidth * 6);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
