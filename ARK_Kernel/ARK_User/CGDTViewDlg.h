#pragma once
#include "afxdialogex.h"


// CGDTViewDlg 对话框

class CGDTViewDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGDTViewDlg)

public:
	CGDTViewDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CGDTViewDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	CListCtrl m_ListCtrl;
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
