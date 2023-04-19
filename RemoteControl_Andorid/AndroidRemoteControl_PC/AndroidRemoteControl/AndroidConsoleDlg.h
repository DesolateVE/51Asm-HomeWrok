#pragma once
#include "afxdialogex.h"

// CMFCConsoleDlg 对话框

class CAndroidConsoleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAndroidConsoleDlg)

public:
	CAndroidConsoleDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CAndroidConsoleDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ANDROID_CONSOLE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit					m_InputEdit;
	CEdit					m_OutputEdit;
	hv::SocketChannelPtr	m_Channel;
	CFont					m_Font;
	CString					m_ResultData;

	void OnConsoleData(CString data);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK();
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
};
