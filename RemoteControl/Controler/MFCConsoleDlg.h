#pragma once
#include "afxdialogex.h"
#include "CPackage.h"
#include "SocketTool.h"

// CMFCConsoleDlg 对话框

class CMFCConsoleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMFCConsoleDlg)

public:
	CMFCConsoleDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMFCConsoleDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCCONSOLE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_EditInput;
	CEdit m_EditOutput;
	CSocketInfo m_TCPInfo;

	static DWORD WINAPI ConsoleMessage(_In_ LPVOID lpParameter);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK();
};
