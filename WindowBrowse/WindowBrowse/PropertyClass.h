#pragma once
#include "afxdialogex.h"


// CPropertyClass 对话框

class CPropertyClass : public CDialogEx
{
	DECLARE_DYNAMIC(CPropertyClass)

public:
	CPropertyClass(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPropertyClass();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPERTY_CLASS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
