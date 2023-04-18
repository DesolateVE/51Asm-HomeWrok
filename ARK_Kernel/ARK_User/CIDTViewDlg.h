#pragma once
#include <afxdialogex.h>

class CIDTViewDlg :
	public CDialogEx
{
	DECLARE_DYNAMIC(CIDTViewDlg)

public:
	CIDTViewDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CIDTViewDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	CListCtrl m_ListCtrl;
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

