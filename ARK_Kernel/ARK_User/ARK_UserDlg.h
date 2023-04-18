
// ARK_UserDlg.h: 头文件
//
#pragma once

#include "CFileViewDlg.h"
#include "CRegistryDlg.h"
#include "CDriverMoudleDlg.h"
#include "CSSDTViewDlg.h"
#include "CGDTViewDlg.h"
#include "CIDTViewDlg.h"
#include "CProcessViewDlg.h"
#include "CNetViewDlg.h"
#include "CStartupViewDlg.h"
#include "MyTABHelp.h"

// CARKUserDlg 对话框
class CARKUserDlg : public CDialogEx
{
// 构造
public:
	CARKUserDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ARK_USER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTcnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	CTabCtrl m_TabCtrl;

	DECLARE_DIALOG_INIT(m_TabCtrl)
		DECLARE_DIALOG_CLASS(CFileViewDlg)
		DECLARE_DIALOG_CLASS(CRegistryDlg)
		DECLARE_DIALOG_CLASS(CDriverMoudleDlg)
		DECLARE_DIALOG_CLASS(CSSDTViewDlg)
		DECLARE_DIALOG_CLASS(CGDTViewDlg)
		DECLARE_DIALOG_CLASS(CIDTViewDlg)
		DECLARE_DIALOG_CLASS(CProcessViewDlg)
		DECLARE_DIALOG_CLASS(CNetViewDlg)
		DECLARE_DIALOG_CLASS(CStartupViewDlg)
};
