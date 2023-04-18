#pragma once
#include "afxdialogex.h"


// CSSDTViewDlg 对话框

class CSSDTViewDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSSDTViewDlg)

public:
	CSSDTViewDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSSDTViewDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	CListCtrl m_ListCtrl;
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
