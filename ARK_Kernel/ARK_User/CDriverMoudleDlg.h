#pragma once
#include "afxdialogex.h"


// CDriverMoudleDlg 对话框

class CDriverMoudleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDriverMoudleDlg)

public:
	CDriverMoudleDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDriverMoudleDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NORMAL_LIST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ListCtrl;
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
