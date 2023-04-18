#pragma once
#include "afxdialogex.h"


// CPropertyWindow 对话框

class CPropertyWindow : public CDialogEx
{
	DECLARE_DYNAMIC(CPropertyWindow)

public:
	CPropertyWindow(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPropertyWindow();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPERTY_WINDOW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
