
// ARK_UserDlg.cpp: 实现文件
//

#include "pch.h"
#include "ARK_User.h"
#include "ARK_UserDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CARKUserDlg 对话框



CARKUserDlg::CARKUserDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ARK_USER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CARKUserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_TabCtrl);
}

BEGIN_MESSAGE_MAP(CARKUserDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CARKUserDlg::OnTcnSelchangeTab)
END_MESSAGE_MAP()


// CARKUserDlg 消息处理程序

BOOL CARKUserDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);			// 设置小图标

	// TODO: 在此添加额外的初始化代码

	BEGIN_DIALOG_CREATE()
		ON_DIALOG_CREATE(CFileViewDlg, IDD_NORMAL_TAP, L"文件")
		ON_DIALOG_CREATE(CRegistryDlg, IDD_NORMAL_TAP, L"注册表")
		ON_DIALOG_CREATE(CNetViewDlg, IDD_NORMAL_LIST, L"网络")
		ON_DIALOG_CREATE(CDriverMoudleDlg, IDD_NORMAL_LIST, L"驱动")
		ON_DIALOG_CREATE(CSSDTViewDlg, IDD_NORMAL_LIST, L"SSDT")
		ON_DIALOG_CREATE(CGDTViewDlg, IDD_NORMAL_LIST, L"GDT")
		ON_DIALOG_CREATE(CIDTViewDlg, IDD_NORMAL_LIST, L"IDT")
		ON_DIALOG_CREATE(CProcessViewDlg, IDD_PROCESS_TAP, L"进程")
		ON_DIALOG_CREATE(CStartupViewDlg, IDD_STARTUP_LIST, L"启动项")
	END_DIALOG_CREATE()

		return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CARKUserDlg::OnPaint()
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
HCURSOR CARKUserDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



afx_msg void CARKUserDlg::OnTcnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	ON_DIALOG_CHANGE()
}

