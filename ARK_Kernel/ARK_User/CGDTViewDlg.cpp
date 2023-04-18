// CGDTViewDlg.cpp: 实现文件
//

#include "pch.h"
#include "ARK_User.h"
#include "afxdialogex.h"
#include "CGDTViewDlg.h"


// CGDTViewDlg 对话框

IMPLEMENT_DYNAMIC(CGDTViewDlg, CDialogEx)

CGDTViewDlg::CGDTViewDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NORMAL_LIST, pParent)
{

}

CGDTViewDlg::~CGDTViewDlg()
{
}

void CGDTViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CGDTViewDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CGDTViewDlg 消息处理程序
BOOL CGDTViewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect ListCtrlRect;
	m_ListCtrl.GetClientRect(ListCtrlRect);
	DWORD dwWidth = ListCtrlRect.Width() / 9;

	ULONG nNum = 0;
	m_ListCtrl.InsertColumn(nNum++, L"Index", LVCFMT_LEFT, dwWidth * 1);
	m_ListCtrl.InsertColumn(nNum++, L"Base", LVCFMT_LEFT, dwWidth * 2);
	m_ListCtrl.InsertColumn(nNum++, L"Limit", LVCFMT_LEFT, dwWidth * 2);
	m_ListCtrl.InsertColumn(nNum++, L"Type", LVCFMT_LEFT, dwWidth * 1);
	m_ListCtrl.InsertColumn(nNum++, L"DPL", LVCFMT_LEFT, dwWidth * 1);
	m_ListCtrl.InsertColumn(nNum++, L"S", LVCFMT_LEFT, dwWidth * 1);
	m_ListCtrl.InsertColumn(nNum++, L"L", LVCFMT_LEFT, ListCtrlRect.Width() - dwWidth * 8);

	return TRUE;
}


// CSSDTViewDlg 消息处理程序
void CGDTViewDlg::OnShowWindow(BOOL bShow, UINT nStatus)
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
	PCGDTINFO pInfo = theDriver.GetGDTInfo(&nEntry);

	for (size_t i = 0; i < nEntry; i++)
	{
		ULONG k = 1;

		szTemp.Format(L"%X", pInfo[i].dwIndex);
		m_ListCtrl.InsertItem(i, szTemp);

		szTemp.Format(L"%p", pInfo[i].qwBase);
		m_ListCtrl.SetItemText(i, k++, szTemp);

		szTemp.Format(L"%p", pInfo[i].dwLimit);
		m_ListCtrl.SetItemText(i, k++, szTemp);

		szTemp.Format(L"0x%02X", pInfo[i].wType);
		m_ListCtrl.SetItemText(i, k++, szTemp);

		szTemp.Format(L"0x%02X", pInfo[i].wDPL);
		m_ListCtrl.SetItemText(i, k++, szTemp);

		szTemp.Format(L"0x%02X", pInfo[i].bS);
		m_ListCtrl.SetItemText(i, k++, szTemp);

		szTemp.Format(L"0x%02X", pInfo[i].bL);
		m_ListCtrl.SetItemText(i, k++, szTemp);
	}

	free(pInfo);

}
