// CGDTViewDlg.cpp: 实现文件
//

#include "pch.h"
#include "ARK_User.h"
#include "afxdialogex.h"
#include "CIDTViewDlg.h"


// CGDTViewDlg 对话框

IMPLEMENT_DYNAMIC(CIDTViewDlg, CDialogEx)

CIDTViewDlg::CIDTViewDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NORMAL_LIST, pParent)
{

}

CIDTViewDlg::~CIDTViewDlg()
{
}

void CIDTViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CIDTViewDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CGDTViewDlg 消息处理程序
BOOL CIDTViewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect ListCtrlRect;
	m_ListCtrl.GetClientRect(ListCtrlRect);
	DWORD dwWidth = ListCtrlRect.Width() / 6;

	ULONG nNum = 0;
	m_ListCtrl.InsertColumn(nNum++, L"Index", LVCFMT_LEFT, dwWidth * 1);
	m_ListCtrl.InsertColumn(nNum++, L"Pointer", LVCFMT_LEFT, dwWidth * 3);
	m_ListCtrl.InsertColumn(nNum++, L"DPL", LVCFMT_LEFT, dwWidth * 1);
	m_ListCtrl.InsertColumn(nNum++, L"TargetSelector", LVCFMT_LEFT, ListCtrlRect.Width() - dwWidth * 5);

	return TRUE;
}


// CSSDTViewDlg 消息处理程序
void CIDTViewDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码

	if (!bShow)
	{
		return;
	}

	m_ListCtrl.DeleteAllItems();

	DWORD nEntry;
	CString szTemp;
	PCIDTINFO pInfo = theDriver.GetIDTInfo(&nEntry);

	for (size_t i = 0; i < nEntry; i++)
	{
		ULONG k = 1;

		szTemp.Format(L"%X", pInfo[i].dwIndex);
		m_ListCtrl.InsertItem(i, szTemp);

		szTemp.Format(L"%p", pInfo[i].qwFunaddr);
		m_ListCtrl.SetItemText(i, k++, szTemp);

		szTemp.Format(L"%02X", pInfo[i].wDPL);
		m_ListCtrl.SetItemText(i, k++, szTemp);

		szTemp.Format(L"%04X", pInfo[i].wTargetSelector);
		m_ListCtrl.SetItemText(i, k++, szTemp);
	}

	free(pInfo);

}
