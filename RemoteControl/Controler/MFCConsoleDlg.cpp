// MFCConsoleDlg.cpp: 实现文件
//

#include "pch.h"
#include "Controler.h"
#include "afxdialogex.h"
#include "MFCConsoleDlg.h"


// CMFCConsoleDlg 对话框

IMPLEMENT_DYNAMIC(CMFCConsoleDlg, CDialogEx)

CMFCConsoleDlg::CMFCConsoleDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCCONSOLE_DIALOG, pParent)
	, m_EditInput(_T(""))
{
	::CreateThread(NULL, 0, ConsoleMessage, this, 0, NULL);
}

CMFCConsoleDlg::~CMFCConsoleDlg()
{
}

void CMFCConsoleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, m_EditInput);
	DDX_Control(pDX, IDC_EDIT3, m_EditOutput);
}


BEGIN_MESSAGE_MAP(CMFCConsoleDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCConsoleDlg 消息处理程序


DWORD WINAPI CMFCConsoleDlg::ConsoleMessage(_In_ LPVOID lpParameter)
{
	auto pDlg = reinterpret_cast<CMFCConsoleDlg*>(lpParameter);

	CString szCmdRnt = "";
	char SockBuffer[1500];
	while (true)
	{
		int iResult = recv(pDlg->m_TCPInfo.m_Socket, SockBuffer, 1500, MSG_PEEK);
		int nlength = reinterpret_cast<CPackageHeader*>(SockBuffer)->m_Length;
		eSockType SockType = reinterpret_cast<CPackageHeader*>(SockBuffer)->m_PackType;
		if ((iResult >= nlength) && (SockType == eSockType::S2C_CMD))
		{
			iResult = recv(pDlg->m_TCPInfo.m_Socket, SockBuffer, nlength, 0);
			auto pCmdPack = reinterpret_cast<CConsoleSend*>(SockBuffer);
			szCmdRnt += pCmdPack->m_szRtnText;
			::SetDlgItemText(pDlg->GetSafeHwnd(), IDC_EDIT3, szCmdRnt);
			pDlg->m_EditOutput.LineScroll(pDlg->m_EditOutput.GetLineCount());
		}
		/*	if (iResult <= 0)
			{
				AfxMessageBox("对方已断开链接");
				shutdown(pDlg->m_TCPInfo.m_Socket, SD_SEND);
				closesocket(pDlg->m_TCPInfo.m_Socket);
				break;
			}*/
	}
	return 0;
}

BOOL CMFCConsoleDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if ((pMsg->message == WM_CHAR) && (pMsg->wParam == VK_RETURN))
	{
		if (GetFocus() == GetDlgItem(IDC_EDIT2))
		{
			UpdateData(TRUE);
			CString szCmdLine = m_EditInput;
			szCmdLine += "\r\n";

			// 发包
			SendPackage<CConsoleRequest>(m_TCPInfo.m_Socket, szCmdLine);

			m_EditInput.Empty();
			UpdateData(FALSE);
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CMFCConsoleDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	return;		// 防止退出了
	CDialogEx::OnOK();
}
