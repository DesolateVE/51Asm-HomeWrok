// PropertyDlg.cpp: 实现文件
//

#include "pch.h"
#include "WindowBrowse.h"
#include "afxdialogex.h"
#include "PropertyDlg.h"


// CPropertyDlg 对话框

IMPLEMENT_DYNAMIC(CPropertyDlg, CDialogEx)

CPropertyDlg::CPropertyDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROPERTY_DIALOG, pParent)
{
	m_aryPage.Add(&m_pageNormal);
	m_aryPage.Add(&m_pageStyle);
	m_aryPage.Add(&m_pageWindow);
	m_aryPage.Add(&m_pageClass);
	m_aryPage.Add(&m_pageProcess);
}

CPropertyDlg::~CPropertyDlg()
{
}

void CPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_TabCtrl);
}


BEGIN_MESSAGE_MAP(CPropertyDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CPropertyDlg::OnBnClickedClose)
	ON_BN_CLICKED(IDC_BUTTON2, &CPropertyDlg::OnBnClickedRefresh)
	ON_BN_CLICKED(IDC_BUTTON3, &CPropertyDlg::OnBnClickedSynch)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CPropertyDlg::OnSelchangeTab)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CPropertyDlg 消息处理程序

void CPropertyDlg::setCurrentWnd(HWND hWnd)
{
	m_hWndCur = hWnd;
}

void CPropertyDlg::DataRefresh(HWND hWnd)
{
	TCHAR szTitle[BYTE_MAX];
	TCHAR szClassName[BYTE_MAX];
	WINDOWINFO cWndInfo;
	LONG lRtn;
	CRect WndRect;

	// PageNomal
	m_pageNormal.m_EditHandle.Format("%p", hWnd);

	::GetWindowText(m_hWndCur, szTitle, BYTE_MAX);
	m_pageNormal.m_EditTitle.Format("%s", szTitle);

	lRtn = ::GetWindowLong(hWnd, GWL_HINSTANCE);
	m_pageNormal.m_EditInstance.Format("%p", (PDWORD)lRtn);

	lRtn = ::GetWindowLong(hWnd, GWL_USERDATA);
	m_pageNormal.m_EditUserData.Format("%p", (PDWORD)lRtn);

	cWndInfo.cbSize = sizeof(WINDOWINFO);
	::GetWindowInfo(hWnd, &cWndInfo);

	WndRect = cWndInfo.rcWindow;
	m_pageNormal.m_EditRect.Format("(%d, %d)-(%d, %d) %dx%d", WndRect.left, WndRect.top, WndRect.right, WndRect.bottom, WndRect.Width(), WndRect.Height());

	WndRect = cWndInfo.rcClient;
	m_pageNormal.m_EditCilentRect.Format("(%d, %d)-(%d, %d) %dx%d", WndRect.left, WndRect.top, WndRect.right, WndRect.bottom, WndRect.Width(), WndRect.Height());

	for (int i = 0; i < m_aryPage.GetCount(); i++)
	{
		m_aryPage[i]->UpdateData(FALSE);
	}
}

void CPropertyDlg::OnBnClickedClose()
{
	// TODO: 在此添加控件通知处理程序代码
	CWnd::DestroyWindow();
}


void CPropertyDlg::OnBnClickedRefresh()
{
	// TODO: 在此添加控件通知处理程序代码
}


afx_msg void CPropertyDlg::OnBnClickedSynch()
{
	// TODO: 在此添加控件通知处理程序代码
}


BOOL CPropertyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	int nItem = 0;
	m_TabCtrl.InsertItem(nItem++, "常规");
	m_TabCtrl.InsertItem(nItem++, "样式");
	m_TabCtrl.InsertItem(nItem++, "窗口");
	m_TabCtrl.InsertItem(nItem++, "类");
	m_TabCtrl.InsertItem(nItem++, "进程");

	m_pageNormal.Create(IDD_PROPERTY_NORMAL, &m_TabCtrl);
	m_pageStyle.Create(IDD_PROPERTY_STYLE, &m_TabCtrl);
	m_pageWindow.Create(IDD_PROPERTY_WINDOW, &m_TabCtrl);
	m_pageClass.Create(IDD_PROPERTY_CLASS, &m_TabCtrl);
	m_pageProcess.Create(IDD_PROPERTY_PROCESS, &m_TabCtrl);

	CRect rcTab;
	m_TabCtrl.GetClientRect(rcTab);
	const int nDate = 15;
	rcTab.left = rcTab.left + 5;
	rcTab.top = rcTab.top + nDate * 2;
	rcTab.right = rcTab.right - 5;
	rcTab.bottom = rcTab.bottom - 5;

	for (int i = 0; i < m_aryPage.GetCount(); i++)
	{
		m_aryPage[i]->MoveWindow(rcTab);
		m_aryPage[i]->SetBackgroundColor(0xF9F9F9);		// 和 TabCtrl 界面颜色保持一致
	}

	DataRefresh(m_hWndCur);

	m_aryPage[0]->ShowWindow(SW_SHOW);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CPropertyDlg::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码

	for (int i = 0; i < m_aryPage.GetCount(); i++)
	{
		m_aryPage[i]->ShowWindow(SW_HIDE);
	}

	int n = m_TabCtrl.GetCurSel();
	m_aryPage[n]->ShowWindow(SW_SHOW);

	*pResult = 0;
}


void CPropertyDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CWnd::DestroyWindow();
}
