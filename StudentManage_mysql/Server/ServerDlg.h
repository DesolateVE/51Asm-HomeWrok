
// ServerDlg.h: 头文件
//

#pragma once

#define MFC_TAB_HELP
#include "..\MyTool.h"
#include "CClassDlg.h"
#include "CCourseDlg.h"
#include "CElectiveDlg.h"
#include "CQueryDlg.h"
#include "CStudentDlg.h"

// CServerDlg 对话框
class CServerDlg : public CDialogEx
{
// 构造
public:
	CServerDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_TabCtrl;

	DECLARE_DIALOG_INIT(m_TabCtrl)
	DECLARE_DIALOG_CLASS(CQueryDlg)
	DECLARE_DIALOG_CLASS(CStudentDlg)
	DECLARE_DIALOG_CLASS(CClassDlg)
	DECLARE_DIALOG_CLASS(CCourseDlg)
	DECLARE_DIALOG_CLASS(CElectiveDlg)
	afx_msg void OnTabSelchange(NMHDR* pNMHDR, LRESULT* pResult);
};
