// CSSDTViewDlg.cpp: 实现文件
//

#include "pch.h"
#include "ARK_User.h"
#include "afxdialogex.h"
#include "CSSDTViewDlg.h"


// CSSDTViewDlg 对话框

IMPLEMENT_DYNAMIC(CSSDTViewDlg, CDialogEx)

CSSDTViewDlg::CSSDTViewDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NORMAL_LIST, pParent)
{

}

CSSDTViewDlg::~CSSDTViewDlg()
{
}

void CSSDTViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_ListCtrl);
}

int CALLBACK CompareFunc(LPARAM lParam1,
	LPARAM lParam2,
	LPARAM lParamSort)
{
	CListCtrl* pListCtrl = (CListCtrl*)lParamSort;
	CString    strItem1 = pListCtrl->GetItemText(static_cast<int>(lParam1), 1);
	CString    strItem2 = pListCtrl->GetItemText(static_cast<int>(lParam2), 1);
	int x1 = _tstoi(strItem1.GetBuffer());
	int x2 = _tstoi(strItem2.GetBuffer());
	int result = 0;
	if ((x1 - x2) < 0)
		result = -1;
	else if ((x1 - x2) == 0)
		result = 0;
	else
		result = 1;

	return result;
}

BOOL CSSDTViewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect ListCtrlRect;
	m_ListCtrl.GetClientRect(ListCtrlRect);
	DWORD dwWidth = ListCtrlRect.Width() / 10;

	ULONG nNum = 0;
	m_ListCtrl.InsertColumn(nNum++, L"函数名", LVCFMT_LEFT, dwWidth * 6);
	m_ListCtrl.InsertColumn(nNum++, L"编号", LVCFMT_LEFT, dwWidth * 1);
	m_ListCtrl.InsertColumn(nNum++, L"地址", LVCFMT_LEFT, ListCtrlRect.Width() - dwWidth * 7);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CSSDTViewDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CSSDTViewDlg 消息处理程序
void CSSDTViewDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码

	if (!bShow)
	{
		return;
	}

	m_ListCtrl.DeleteAllItems();
	DWORD EntryNum = theDriver.SSDTSearch();
	PCSSDTINFO pInfo = theDriver.GetSSDTInfo(EntryNum);
	CString szTemp;

	for (size_t i = 0; i < EntryNum; i++)
	{
		ULONG k = 1;
		m_ListCtrl.InsertItem(i, pInfo[i].szFunName);

		szTemp.Format(L"%i", pInfo[i].dwFunNum);
		m_ListCtrl.SetItemText(i, k++, szTemp);

		szTemp.Format(L"%p", pInfo[i].qwFunaddr);
		m_ListCtrl.SetItemText(i, k++, szTemp);
	}
	free(pInfo);

	m_ListCtrl.SortItemsEx(CompareFunc, (DWORD_PTR)&m_ListCtrl);
}
