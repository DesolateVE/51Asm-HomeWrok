
// WindowBrowseDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "WindowBrowse.h"
#include "WindowBrowseDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CWindowBrowseDlg 对话框



CWindowBrowseDlg::CWindowBrowseDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_WINDOWBROWSE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hAccel = ::LoadAccelerators(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
}

void CWindowBrowseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_WinTree);
}

BEGIN_MESSAGE_MAP(CWindowBrowseDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_RCLICK, IDC_TREE1, &CWindowBrowseDlg::OnRclickTree)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &CWindowBrowseDlg::OnDblclkTree)
	ON_COMMAND(ID_ACCELERATOR_FINDWINDOW, &CWindowBrowseDlg::OnAcceleratorFindwindow)
END_MESSAGE_MAP()


// CWindowBrowseDlg 消息处理程序

BOOL CWindowBrowseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	m_ImageList.Create(16, 16, ILC_COLOR32, 5, 1);
	m_ImageList.SetBkColor(RGB(255, 255, 255));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_WNDICON));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_WNDICON_H));

	m_WinTree.SetImageList(&m_ImageList, LVSIL_NORMAL);

	HANDLE hToken;
	BOOL flag = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);
	if (flag)
	{
		SetPrivilege(hToken, SE_DEBUG_NAME, TRUE);
		CloseHandle(hToken);
	}
	else
	{
		ShowErrMsg("OpenProcessToken Failed");
	}

	InitWinList();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CWindowBrowseDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CWindowBrowseDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CWindowBrowseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CWindowBrowseDlg::InitWinList()
{
	TCHAR szWindowText[BYTE_MAX];
	TCHAR szClassName[BYTE_MAX];
	CString szShowText;
	TREEINFO Info;

	m_WinTree.DeleteAllItems();

	HWND hDesktop = GetDesktopWindow()->GetSafeHwnd();
	::GetWindowText(hDesktop, szWindowText, BYTE_MAX);
	::GetClassName(hDesktop, szClassName, BYTE_MAX);

	szShowText.Format("Desktop [桌面] [%p] [%s] [%s]", hDesktop, szWindowText, szClassName);

	TV_INSERTSTRUCT NodeInfo; // 根目录
	ZeroMemory(&NodeInfo, sizeof(NodeInfo));
	NodeInfo.hParent = TVI_ROOT;
	NodeInfo.hInsertAfter = TVI_LAST;
	NodeInfo.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM;
	NodeInfo.item.pszText = szShowText.GetBuffer();
	NodeInfo.item.iImage = 0;
	NodeInfo.item.lParam = (LPARAM)hDesktop;
	m_TreeRoot = m_WinTree.InsertItem(&NodeInfo);

	Info.hItem = m_TreeRoot;
	Info.lpTree = &m_WinTree;

	::EnumWindows(EnumWindowsProc, (LPARAM)&Info);

	m_WinTree.Expand(m_TreeRoot, TVE_EXPAND);
}

BOOL CWindowBrowseDlg::SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	CString buf;

	if (!LookupPrivilegeValue(
		NULL,            // lookup privilege on local system
		lpszPrivilege,   // privilege to lookup 
		&luid))        // receives LUID of privilege
	{
		ShowErrMsg("LookupPrivilegeValue failed");
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
	{
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	}
	else
	{
		tp.Privileges[0].Attributes = 0;
	}

	// Enable the privilege or disable all privileges.

	if (!AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		(PTOKEN_PRIVILEGES)NULL,
		(PDWORD)NULL))
	{
		ShowErrMsg("AdjustTokenPrivileges failed.");
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		ShowErrMsg("The token does not have the specified privilege");
		return FALSE;
	}

	return TRUE;
}

BOOL CALLBACK EnumWindowsProc(_In_ HWND hwnd, _In_ LPARAM lParam)
{
	TREEINFO* pInfo = (TREEINFO*)lParam;
	TREEINFO& Info = UpdateTree(hwnd, pInfo);
	EnumChildWindows(hwnd, EnumWindowsProc, (LPARAM)&Info);		// 直接递归查找

	return true;
}

TREEINFO UpdateTree(HWND hwnd, TREEINFO* pInfo)
{
	TCHAR szWindowText[BYTE_MAX];
	TCHAR szClassName[BYTE_MAX];
	CString szShowText;
	TREEINFO Info;

	::GetWindowText(hwnd, szWindowText, BYTE_MAX);
	::GetClassName(hwnd, szClassName, BYTE_MAX);

	szShowText.Format("[窗口] [%p] [%s] [%s]", hwnd, szWindowText, szClassName);

	TV_INSERTSTRUCT NodeInfo;
	ZeroMemory(&NodeInfo, sizeof(NodeInfo));
	NodeInfo.hParent = pInfo->hItem;
	NodeInfo.hInsertAfter = TVI_LAST;
	NodeInfo.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM;
	NodeInfo.item.pszText = szShowText.GetBuffer();
	NodeInfo.item.iImage = ::IsWindowVisible(hwnd) ? 0 : 1;	// 窗口是否可见
	NodeInfo.item.lParam = (LPARAM)hwnd;

	Info.hItem = pInfo->lpTree->InsertItem(&NodeInfo);
	Info.lpTree = pInfo->lpTree;

	return Info;
}


void CWindowBrowseDlg::OnRclickTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CWindowBrowseDlg::OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CWindowBrowseDlg::OnAcceleratorFindwindow()
{
	// TODO: 在此添加命令处理程序代码
	CFindWindowDlg dlgFind;
	if (dlgFind.DoModal() == IDOK)
	{
		HWND hWndFind = dlgFind.getFindResult();

		// 功能界面选择
		if (dlgFind.getRadioSelect() == 0)	
		{
			CRect ParentRect;
			CRect ChildRect;
			CPoint OffsetPt;

			if (!m_dlgProperty.m_hWnd)
			{
				m_dlgProperty.setCurrentWnd(hWndFind);
				m_dlgProperty.Create(IDD_PROPERTY_DIALOG);

				// 非模态窗口位置是基于父窗口客户区的，所以要重新计算子窗口在父窗口中点坐标
				GetWindowRect(&ParentRect);
				m_dlgProperty.GetWindowRect(&ChildRect);

				// 算法一
				//OffsetPt.x = (ParentRect.Width() - ChildRect.Width()) / 2;
				//OffsetPt.y = (ParentRect.Height() - ChildRect.Height()) / 2;
				//OffsetPt.x += ParentRect.left;
				//OffsetPt.y += ParentRect.top;
				//ChildRect.MoveToXY(OffsetPt);

				// 算法二
				OffsetPt = ParentRect.CenterPoint() - ChildRect.CenterPoint();
				ChildRect.OffsetRect(OffsetPt);

				m_dlgProperty.MoveWindow(ChildRect);
				m_dlgProperty.ShowWindow(SW_SHOW);
			}
			else
			{
				m_dlgProperty.DataRefresh(hWndFind);
			}
		} // 属性界面
		else
		{

		} // 消息界面
	}
}



BOOL CWindowBrowseDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
	{
		return true;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
