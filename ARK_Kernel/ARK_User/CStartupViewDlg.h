#pragma once
#include "afxdialogex.h"


// CStartupViewDlg 对话框

class CStartupViewDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CStartupViewDlg)

public:
	CStartupViewDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CStartupViewDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum {
		IDD = IDD_STARTUP_LIST
	};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void addServerList(PWCHAR szName);


	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	CListCtrl m_AppList;
	CListCtrl m_ServerList;
};
