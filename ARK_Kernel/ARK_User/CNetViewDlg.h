#pragma once
#include "afxdialogex.h"


// CNetViewDlg 对话框

class CNetViewDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNetViewDlg)

public:
	CNetViewDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CNetViewDlg();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ListCtrl;

	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
