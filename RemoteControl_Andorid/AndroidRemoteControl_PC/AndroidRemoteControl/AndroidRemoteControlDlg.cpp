
// AndroidRemoteControlDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "AndroidRemoteControl.h"
#include "AndroidRemoteControlDlg.h"
#include "afxdialogex.h"
#include "VETool.h"
#include "SocketPackage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace hv;

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


// CAndroidRemoteControlDlg 对话框



CAndroidRemoteControlDlg::CAndroidRemoteControlDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ANDROIDREMOTECONTROL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_TextColor = rgbRed;
	m_bInit = FALSE;
	m_bClose = FALSE;
}

CAndroidRemoteControlDlg::~CAndroidRemoteControlDlg()
{
	m_TcpSrv.closesocket();
}

void CAndroidRemoteControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT3, m_EditIP);
	DDX_Control(pDX, IDC_EDIT2, m_EditHost);
	DDX_Control(pDX, IDC_EDIT1, m_EditPort);
	DDX_Control(pDX, IDC_DEST_IP, m_StaticDest);
}

BEGIN_MESSAGE_MAP(CAndroidRemoteControlDlg, CDialogEx)
	ON_WM_QUERYDRAGICON()
	ON_WM_SYSCOMMAND()
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON2, &CAndroidRemoteControlDlg::OnBnClickedPortOn)
	ON_BN_CLICKED(IDC_BUTTON3, &CAndroidRemoteControlDlg::OnBnClickedRemoteControl)
	ON_BN_CLICKED(IDC_BUTTON4, &CAndroidRemoteControlDlg::OnBnClickedShowWindow)
	ON_BN_CLICKED(IDC_BUTTON1, &CAndroidRemoteControlDlg::OnBnClickedShellOn)
END_MESSAGE_MAP()


// CAndroidRemoteControlDlg 消息处理程序

BOOL CAndroidRemoteControlDlg::OnInitDialog()
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
	InitSystemInfo();
	InitSocketInfo();
	m_EditPort.SetWindowTextW(L"9527");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAndroidRemoteControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CAndroidRemoteControlDlg::OnPaint()
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
HCURSOR CAndroidRemoteControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAndroidRemoteControlDlg::InitSystemInfo()
{
	VETool::CIPInfo* pInfo = new VETool::CIPInfo();

	SetWindowTextA(m_EditIP.GetSafeHwnd(), pInfo->getIPInfo());
	SetWindowTextA(m_EditHost.GetSafeHwnd(), pInfo->getHostName());
	m_StaticDest.SetWindowTextW(L"unConnect");

	delete pInfo;
}

void CAndroidRemoteControlDlg::InitSocketInfo()
{
	// 响应链接和断开事件
	m_TcpSrv.onConnection = [&](const SocketChannelPtr& channel) {
		if (channel->isConnected()) {
			m_Channel = channel;
			if (!m_bClose)
			{
				std::string peeraddr = channel->peeraddr();
				SetWindowTextA(m_StaticDest.GetSafeHwnd(), peeraddr.c_str());
				m_TextColor = rgbGreen;
				m_StaticDest.Invalidate();
			}
		}
		else {
			if (!m_bClose)
			{
				m_StaticDest.SetWindowTextW(L"wait to connect");
				m_TextColor = rgbRed;
				m_StaticDest.Invalidate();
			}
		}
	};

	// 响应消息
	m_TcpSrv.onMessage = [&](const SocketChannelPtr& channel, Buffer* buf) {
		// echo

		CHeader* pHeader = (CHeader*)buf->data();
		switch (pHeader->m_Type)
		{
		case PackageType::REQUEST_WINDOW_SHOW_ON:	// 应返回手机屏幕相关信息
		{
			CScreenInfo* pInfo = (CScreenInfo*)((char*)buf->data() + sizeof(CHeader));
			m_AndroidWnd.ResetWindow(CRect(0, 0, pInfo->m_ScreenWidth, pInfo->m_ScreenHeight));
			break;
		}
		case PackageType::STREAM_WINDOW_SHOW:		// 应返回窗口视图字节流

			m_AndroidWnd.RefreshView((char*)buf->data() + sizeof(CHeader), pHeader->m_Length);

			break;
		case PackageType::SOCKET_DISCONNECT:		// 断开连接
			if (!m_bClose)
			{
				m_StaticDest.SetWindowTextW(L"wait to connect");
				m_TextColor = rgbRed;
				m_StaticDest.Invalidate();
				channel->close();
			}
			break;
		case PackageType::STREAM_SHELL_DATA:
			buf->base[buf->len] = NULL;
			m_AndroidControlDlg.OnConsoleData(CString((char*)buf->data() + sizeof(CHeader)));
			break;
		}
	};

	// 消息回显
	m_TcpSrv.onWriteComplete = [](const SocketChannelPtr& channel, Buffer* buf) {

	};

	// 拆包规则
	unpack_setting_t protorpc_unpack_setting;
	ZeroMemory(&protorpc_unpack_setting, sizeof(unpack_setting_t));
	protorpc_unpack_setting.mode = UNPACK_BY_LENGTH_FIELD;
	protorpc_unpack_setting.package_max_length = DEFAULT_PACKAGE_MAX_LENGTH;
	protorpc_unpack_setting.body_offset = sizeof(CHeader);
	protorpc_unpack_setting.length_field_offset = offsetof(CHeader, m_Length);
	protorpc_unpack_setting.length_field_bytes = sizeof(CHeader::m_Length);
	protorpc_unpack_setting.length_field_coding = ENCODE_BY_LITTEL_ENDIAN;
	m_TcpSrv.setUnpack(&protorpc_unpack_setting);

	// 配置线程
	m_TcpSrv.setThreadNum(5);
}

// 重写控件颜色
HBRUSH CAndroidRemoteControlDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (pWnd->GetDlgCtrlID() == m_StaticDest.GetDlgCtrlID())
	{
		pDC->SetTextColor(m_TextColor);
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

void CAndroidRemoteControlDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	
	m_bClose = TRUE;

	if (m_Channel && m_Channel->isOpened())
	{
		// 关闭前告诉手机端断开连接
		std::shared_ptr<CPackage> pPackage(new CPackage(PackageType::SOCKET_DISCONNECT, nullptr, 0));
		m_Channel->write(pPackage->m_Buffer, pPackage->m_Length);
	}

	CDialogEx::OnClose();
}

void CAndroidRemoteControlDlg::OnBnClickedPortOn()
{
	// TODO: 在此添加控件通知处理程序代码

	if (!m_bInit)
	{
		CString szPort;
		m_EditPort.GetWindowTextW(szPort);
		if (szPort.IsEmpty())
		{
			MessageBox(L"端口号为空！");
			return;
		}

		int port = _wtoi(szPort);
		int listenfd = m_TcpSrv.createsocket(port);
		if (listenfd < 0) {
			MessageBox(L"Socket创建失败");
			return;
		}

		m_TcpSrv.start();
		m_StaticDest.SetWindowTextW(L"wait to connect");
		m_bInit = TRUE;
	}
}


void CAndroidRemoteControlDlg::OnBnClickedRemoteControl()
{
	// TODO: 在此添加控件通知处理程序代码

	if (!m_Channel || !m_Channel->isOpened())
	{
		return;
	}

	std::shared_ptr<CPackage> pPackage(new CPackage(PackageType::REQUEST_ROOT_PERMISSION, nullptr, 0));
	m_Channel->write(pPackage->m_Buffer, pPackage->m_Length);
	m_AndroidWnd.m_bControl = TRUE;
}


void CAndroidRemoteControlDlg::OnBnClickedShowWindow()
{
	// TODO: 在此添加控件通知处理程序代码

	if (!m_Channel || !m_Channel->isOpened())
	{
		return;
	}

	m_AndroidWnd.m_Channel = m_Channel;
	m_AndroidWnd.CreateEx(NULL, AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)), L"Android Window", WS_CAPTION | WS_POPUPWINDOW, CRect(0, 0, 0, 0), theApp.GetMainWnd(), NULL);

	std::shared_ptr<CPackage> pPackage(new CPackage(PackageType::REQUEST_WINDOW_SHOW_ON, nullptr, 0));
	m_Channel->write(pPackage->m_Buffer, pPackage->m_Length);
}

void CAndroidRemoteControlDlg::OnBnClickedShellOn()
{
	// TODO: 在此添加控件通知处理程序代码

	if (!m_Channel || !m_Channel->isOpened())
	{
		return;
	}

	m_AndroidControlDlg.m_Channel = m_Channel;
	m_AndroidControlDlg.Create(IDD_ANDROID_CONSOLE_DIALOG);
	m_AndroidControlDlg.ShowWindow(SW_SHOW);
}

