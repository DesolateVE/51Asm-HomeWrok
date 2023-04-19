
// ScreenShotDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ScreenShot.h"
#include "ScreenShotDlg.h"
#include "afxdialogex.h"

#include <memory>
using namespace std;

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


// CScreenShotDlg 对话框



CScreenShotDlg::CScreenShotDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SCREENSHOT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CScreenShotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CScreenShotDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_HOTKEY()
END_MESSAGE_MAP()


// CScreenShotDlg 消息处理程序

BOOL CScreenShotDlg::OnInitDialog()
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
	if (!::RegisterHotKey(GetSafeHwnd(), 0x1234, MOD_ALT | MOD_NOREPEAT, 'A'))
	{
		AfxMessageBox("注册热键失败");
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CScreenShotDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CScreenShotDlg::OnPaint()
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
HCURSOR CScreenShotDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//struct lParam
//{
//	union
//	{
//		LPARAM data;
//		struct
//		{
//			DWORD nRCount : 16;
//			DWORD nSCode : 8;
//			DWORD bExkey : 1;
//			DWORD : 4;
//			DWORD bContextcode : 1;
//			DWORD bPreviousState : 1;
//			DWORD bTransitionState : 1;
//		};
//	};
//};


// 优化方案：只比较客户端最右侧滚动条图像变化，将循环事件压缩到最小
void CScreenShotDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	if (nHotKeyId == 0x1234)
	{
		// 设定一个响应延迟
		Sleep(500);

		// 翻页指令
		INPUT inputs[2];
		inputs[0].type = INPUT_KEYBOARD;
		inputs[0].ki.wVk = VK_NEXT;
		inputs[0].ki.dwFlags = NULL;

		inputs[1].type = INPUT_KEYBOARD;
		inputs[1].ki.wVk = VK_NEXT;
		inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

		// 截图前准备
		CPoint pt;
		::GetCursorPos(&pt);					//获取鼠标位置
		HWND hWnd = ::WindowFromPoint(pt);		//获取鼠标所指向窗口句柄

		HDC hDC = ::GetDC(hWnd);				//获取窗口句柄的上下文
		CRect rc;
		::GetClientRect(hWnd, &rc);				//获取客户端大小

		CList<shared_ptr<CImage>> lstImgs;

		// 循环拷贝
		bool bEnd = true;
		while (bEnd)
		{
			// 拷贝客户区图像
			shared_ptr<CImage> pImg(new CImage);
			pImg->Create(rc.Width(), rc.Height(), 32);
			HDC hDCImg = pImg->GetDC();
			::BitBlt(hDCImg, 0, 0, rc.Width(), rc.Height(), hDC, 0, 0, SRCCOPY);

			if (!lstImgs.IsEmpty())
			{
				LPDWORD pBitsOld = (LPDWORD)lstImgs.GetTail()->GetBits();
				LPDWORD pBitsNew = (LPDWORD)pImg->GetBits();

				bEnd = false;
				DWORD nDif = 0;						//偏差值
				for (size_t i = 0; i < rc.Width() * rc.Height() / 2; i++)
				{
					if (*(pBitsOld - i) != *(pBitsNew - i))
					{
						nDif++;
					}
					if (nDif > rc.Width() * 10)		//排除选定框干扰
					{
						bEnd = true;
						break;
					}
				}
			}

			if (bEnd)
			{
				// 加入列表
				lstImgs.AddTail(pImg);
				// 向下翻页
				SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
				Sleep(500);
			}
			pImg->ReleaseDC();
		}

		//拼出一张大图
		CImage imgFinal;
		BOOL bRet = imgFinal.Create(rc.Width(), rc.Height() * lstImgs.GetSize(), 32);
		HDC hDcDst = imgFinal.GetDC();
		int nIdx = 0;
		auto pos = lstImgs.GetHeadPosition();
		while (pos)
		{
			auto pImg = lstImgs.GetNext(pos);
			HDC hDcSrc = pImg->GetDC();

			//拷贝数据
			::BitBlt(hDcDst, 0, nIdx * rc.Height(), rc.Width(), rc.Height(),
				hDcSrc, 0, 0,
				SRCCOPY);
			nIdx++;

			LPVOID pBits = pImg->GetBits();
			pImg->ReleaseDC();

		}
		imgFinal.ReleaseDC();
		imgFinal.Save("2.bmp");
	}


	//// 无法设置焦点
	//::SetFocus(hWnd);

	//// 参数不对
	//lParam lP = { 0 };
	//lP.nRCount = 1;
	//lP.nSCode = 0;
	//lP.bExkey = 1;
	//lP.bContextcode = 0;
	//lP.bPreviousState = 0;
	//lP.bTransitionState = 0;
	//::PostMessageA(hWnd, WM_KEYDOWN, VK_NEXT, lP.data);
	//lP.bPreviousState = 1;
	//lP.bTransitionState = 1;
	//::PostMessageA(hWnd, WM_KEYUP, VK_NEXT, lP.data);

	//keybd_event(VK_NEXT, 0, 0, 0);
	//keybd_event(VK_NEXT, 0, KEYEVENTF_KEYUP, 0);

	//// 以下方法无效
	//SCROLLINFO rolInfo;
	//ZeroMemory(&rolInfo, sizeof(rolInfo));
	//rolInfo.cbSize = sizeof(rolInfo);
	//rolInfo.fMask = SIF_TRACKPOS | SIF_RANGE | SIF_PAGE | SIF_POS;
	//bool bflag = ::GetScrollInfo(hWnd, SB_VERT, &rolInfo);
	//int nReturn = ::GetScrollPos(hWnd, SB_VERT);
	//::SetScrollPos(hWnd, SB_VERT, 5, false);


	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}
