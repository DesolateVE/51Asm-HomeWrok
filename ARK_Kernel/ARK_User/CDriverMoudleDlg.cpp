// CDriverMoudleDlg.cpp: 实现文件
//

#include "pch.h"
#include "ARK_User.h"
#include "afxdialogex.h"
#include "CDriverMoudleDlg.h"


// CDriverMoudleDlg 对话框

IMPLEMENT_DYNAMIC(CDriverMoudleDlg, CDialogEx)

CDriverMoudleDlg::CDriverMoudleDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NORMAL_LIST, pParent)
{

}

CDriverMoudleDlg::~CDriverMoudleDlg()
{
}

void CDriverMoudleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CDriverMoudleDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CDriverMoudleDlg 消息处理程序


BOOL CDriverMoudleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	CRect ListCtrlRect;
	m_ListCtrl.GetClientRect(ListCtrlRect);
	DWORD dwWidth = ListCtrlRect.Width() / 20;

	ULONG nNum = 0;
	m_ListCtrl.InsertColumn(nNum++, L"模块名", LVCFMT_LEFT, dwWidth * 3);
	m_ListCtrl.InsertColumn(nNum++, L"模块路径", LVCFMT_LEFT, dwWidth * 6);
	m_ListCtrl.InsertColumn(nNum++, L"模块基址", LVCFMT_LEFT, dwWidth * 4);
	m_ListCtrl.InsertColumn(nNum++, L"模块入口", LVCFMT_LEFT, dwWidth * 4);
	m_ListCtrl.InsertColumn(nNum++, L"大小", LVCFMT_LEFT, ListCtrlRect.Width() - dwWidth * 17);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDriverMoudleDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码
	if (!bShow)
	{
		return;
	}

	m_ListCtrl.DeleteAllItems();
	DWORD EntryNum = theDriver.DriverSearch();
	PCMODULE pInfo = theDriver.GetDriverInfo(EntryNum);
	CString szTemp;

	for (size_t i = 0; i < EntryNum; i++)
	{
		ULONG k = 1;
		m_ListCtrl.InsertItem(i, pInfo[i].szBaseDllName);
		m_ListCtrl.SetItemText(i, k++, pInfo[i].szFullDllName);

		szTemp.Format(L"%p", pInfo[i].pDllBase);
		m_ListCtrl.SetItemText(i, k++, szTemp);

		szTemp.Format(L"%p", pInfo[i].pEntryPoint);
		m_ListCtrl.SetItemText(i, k++, szTemp);

		szTemp.Format(L"%i", pInfo[i].dwSize);
		m_ListCtrl.SetItemText(i, k++, szTemp);
	}
	free(pInfo);
}
