// MFCConsoleDlg.cpp: 实现文件
//

#include "pch.h"
#include "AndroidRemoteControl.h"
#include "afxdialogex.h"
#include "AndroidConsoleDlg.h"
#include "SocketPackage.h"


// CMFCConsoleDlg 对话框

IMPLEMENT_DYNAMIC(CAndroidConsoleDlg, CDialogEx)

CAndroidConsoleDlg::CAndroidConsoleDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ANDROID_CONSOLE_DIALOG, pParent)
{
}

CAndroidConsoleDlg::~CAndroidConsoleDlg()
{
}

void CAndroidConsoleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_InputEdit);
	DDX_Control(pDX, IDC_EDIT2, m_OutputEdit);
}


BEGIN_MESSAGE_MAP(CAndroidConsoleDlg, CDialogEx)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CMFCConsoleDlg 消息处理程序




void CAndroidConsoleDlg::OnConsoleData(CString data)
{

	data.TrimRight(L"\n");
	data += "\r\n";
	m_ResultData += data;
	m_OutputEdit.SetWindowText(m_ResultData);
	m_OutputEdit.LineScroll(m_OutputEdit.GetLineCount());
}

BOOL CAndroidConsoleDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if ((pMsg->message == WM_CHAR) && (pMsg->wParam == VK_RETURN))
	{
		if (GetFocus() == GetDlgItem(IDC_EDIT1))
		{
			CString szCmdLine;
			CStringA aText;

			m_InputEdit.GetWindowText(szCmdLine);
			if (szCmdLine == "clear")
			{
				m_ResultData.Empty();
				m_InputEdit.SetWindowText(NULL);
				m_OutputEdit.SetWindowText(NULL);
				return CDialogEx::PreTranslateMessage(pMsg);
			}

			// 回显
			aText.Format("%S\n", szCmdLine);
			szCmdLine.Format(L"su: %s", szCmdLine);
			OnConsoleData(szCmdLine);

			std::shared_ptr<CPackage> pPackage(new CPackage(PackageType::STREAM_SHELL_DATA, aText.GetBuffer(), aText.GetLength()));
			m_Channel->write(pPackage->m_Buffer, pPackage->m_Length);
			m_InputEdit.SetWindowText(NULL);
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CAndroidConsoleDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	return;		// 防止退出
	CDialogEx::OnOK();
}


void CAndroidConsoleDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	m_Channel = NULL;
	m_ResultData.Empty();
	// 非模态框应该被销毁，而不是调用父类的关闭
	CWnd::DestroyWindow();
}


BOOL CAndroidConsoleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_Font.CreatePointFont(120, L"Courier New");
	m_OutputEdit.SetFont(&m_Font);
	m_InputEdit.SetFont(&m_Font);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
