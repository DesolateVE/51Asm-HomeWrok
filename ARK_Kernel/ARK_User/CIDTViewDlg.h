#pragma once
#include <afxdialogex.h>

class CIDTViewDlg :
	public CDialogEx
{
	DECLARE_DYNAMIC(CIDTViewDlg)

public:
	CIDTViewDlg(CWnd* pParent = nullptr);   // ��׼���캯��
	virtual ~CIDTViewDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	CListCtrl m_ListCtrl;
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

