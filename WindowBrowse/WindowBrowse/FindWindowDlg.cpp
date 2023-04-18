// FindWindowDlg.cpp: 实现文件
//

#include "pch.h"
#include "WindowBrowse.h"
#include "afxdialogex.h"
#include "FindWindowDlg.h"


// CFindWindowDlg 对话框

IMPLEMENT_DYNAMIC(CFindWindowDlg, CDialogEx)

CFindWindowDlg::CFindWindowDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FINDWINDOW_DIALOG, pParent)
	, m_editHandle(_T(""))
	, m_editTitle(_T(""))
	, m_editClass(_T(""))
	, m_editStyle(_T(""))
	, m_editRect(_T(""))
	, m_checkHiden(TRUE)
	, m_radioSelect(0)
{
	// 资源初始化
	m_hCursor = AfxGetApp()->LoadCursor(IDC_CURSOR1);
	m_hScan = AfxGetApp()->LoadIcon(IDI_SCAN);
	m_hFrom = AfxGetApp()->LoadIcon(IDI_FROM);

	// 状态初始化
	m_bScaning = false;
	m_hWndCur = nullptr;
	m_hWndRtn = nullptr;
}

CFindWindowDlg::~CFindWindowDlg()
{
	DestroyCursor(m_hCursor);
	DestroyIcon(m_hScan);
	DestroyIcon(m_hFrom);
}

HWND CFindWindowDlg::getFindResult()
{
	return m_hWndRtn;
}

bool CFindWindowDlg::getRadioSelect()
{
	return m_radioSelect;
}

bool CFindWindowDlg::isClickIcon(CPoint& point)
{
	CRect rect;
	m_FindIcon.GetWindowRect(&rect);
	ScreenToClient(&rect);
	return rect.PtInRect(point);
}

void CFindWindowDlg::DrawBroder(HWND hWnd)
{
	// 若非窗口则返回
	if (!IsWindow(hWnd))
		return;

	CRect rect; // 窗口矩形

	// 得到窗口矩形
	::GetWindowRect(hWnd, &rect);

	CDC dc; // 窗口设备上下文
	dc.Attach(::GetWindowDC(hWnd));

	// 设置窗口当前前景色的混合模式为R2_NOT
	// R2_NOT - 当前的像素值为屏幕像素值的取反，这样可以覆盖掉上次的绘图
	dc.SetROP2(R2_NOT);

	// 创建画笔
	CPen pen;

	// PS_INSIDEFRAME - 产生封闭形状的框架内直线，指定一个限定矩形
	// 3 * GetSystemMetrics(SM_CXBORDER) - 三倍边界粗细
	// RGB(0,0,0) - 黑色
	pen.CreatePen(PS_INSIDEFRAME, 3 * GetSystemMetrics(SM_CXBORDER), RGB(0, 0, 0));

	// 选择画笔
	HGDIOBJ old_pen = dc.SelectObject(pen);

	// 设定画刷
	HGDIOBJ old_brush = dc.SelectObject(GetStockObject(NULL_BRUSH));

	// 画矩形
	dc.Rectangle(0, 0, rect.Width(), rect.Height());

	// 恢复原来的设备环境
	dc.SelectObject(old_pen);
	dc.SelectObject(old_brush);

	pen.DeleteObject();
}

void CFindWindowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_editHandle);
	DDX_Text(pDX, IDC_EDIT2, m_editTitle);
	DDX_Text(pDX, IDC_EDIT4, m_editClass);
	DDX_Text(pDX, IDC_EDIT3, m_editStyle);
	DDX_Text(pDX, IDC_EDIT5, m_editRect);
	DDX_Check(pDX, IDC_CHECK1, m_checkHiden);
	DDX_Radio(pDX, IDC_RADIO1, m_radioSelect);
	DDX_Control(pDX, IDC_FINDICON, m_FindIcon);
}


BEGIN_MESSAGE_MAP(CFindWindowDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CFindWindowDlg::OnBnClickedButtonOK)
	ON_BN_CLICKED(IDC_BUTTON2, &CFindWindowDlg::OnBnClickedButtonCANCEL)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CFindWindowDlg 消息处理程序


void CFindWindowDlg::OnBnClickedButtonOK()
{
	// TODO: 在此添加控件通知处理程序代码

	if (m_editHandle.IsEmpty())
	{
		AfxMessageBox("No Window is currently selected!");
	}
	else
	{
		UpdateData(TRUE);
		sscanf_s(m_editHandle.GetBuffer(), "%p", &m_hWndRtn);
		CDialogEx::OnOK();
	}
}


void CFindWindowDlg::OnBnClickedButtonCANCEL()
{
	// TODO: 在此添加控件通知处理程序代码

	CDialogEx::OnCancel();
}

void CFindWindowDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (isClickIcon(point))
	{
		m_bScaning = true;

		m_FindIcon.SetIcon(m_hScan);
		::SetCursor(m_hCursor);
		SetCapture();

		UpdateData(TRUE);
		if (m_checkHiden)
		{
			AfxGetMainWnd()->ShowWindow(SW_HIDE);
		}
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CFindWindowDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (m_bScaning)
	{
		// 状态标志复位
		m_bScaning = false;

		// 修改返回窗口句柄
		m_hWndRtn = m_hWndCur;

		// 图标恢复，释放鼠标捕获
		m_FindIcon.SetIcon(m_hFrom);
		ReleaseCapture();

		// 窗口描线恢复
		if (m_hWndCur != nullptr)
		{
			DrawBroder(m_hWndCur);
			m_hWndCur = nullptr;
		}

		// 主窗口显示
		if (m_checkHiden)
		{
			AfxGetMainWnd()->ShowWindow(SW_SHOW);
		}
	}

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CFindWindowDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (m_bScaning)
	{
		ClientToScreen(&point);
		HWND hWndCursor = ::WindowFromPoint(point);

		if (hWndCursor == GetSafeHwnd() || hWndCursor == AfxGetMainWnd()->GetSafeHwnd() || ::GetParent(hWndCursor) == GetSafeHwnd() || hWndCursor == nullptr)	// 窗口为空或窗口为自己也不做处理，子控件窗口也加了一层判断
		{
			return;
		}

		if (m_hWndCur != hWndCursor)
		{
			if (m_hWndCur != nullptr)
			{
				DrawBroder(m_hWndCur);
			}
			m_hWndCur = hWndCursor;
			DrawBroder(m_hWndCur);

			// 更新数据
			TCHAR szTitle[BYTE_MAX];
			TCHAR szClass[BYTE_MAX];
			CRect WndRect;
			LONG lStyle;

			::GetWindowText(m_hWndCur, szTitle, BYTE_MAX);
			::GetClassName(m_hWndCur, szClass, BYTE_MAX);
			::GetWindowRect(m_hWndCur, &WndRect);
			lStyle = ::GetWindowLong(m_hWndCur, GWL_STYLE);

			m_editHandle.Format("%p", m_hWndCur);
			m_editTitle.Format("%s", szTitle);
			m_editClass.Format("%s", szClass);
			m_editStyle.Format("%08X", lStyle);
			m_editRect.Format("(%d, %d)-(%d, %d) %dx%d", WndRect.left, WndRect.top, WndRect.right, WndRect.bottom, WndRect.Width(), WndRect.Height());

			UpdateData(FALSE);
		}
	}

	CDialogEx::OnMouseMove(nFlags, point);
}


BOOL CFindWindowDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	GetDlgItem(IDC_EDIT1)->SetFocus();

	return FALSE;
}
