#pragma once
#include "afxdialogex.h"


// CEditUpdataDlg 对话框

class CEditUpdataDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEditUpdataDlg)

public:
	CEditUpdataDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CEditUpdataDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDITUPDATA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	CString m_EditCtrl;
	CRect m_Rect;

public:
	virtual void OnOK();
	virtual BOOL OnInitDialog();
};
