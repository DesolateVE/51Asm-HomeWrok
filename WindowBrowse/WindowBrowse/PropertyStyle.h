#pragma once
#include "afxdialogex.h"


// CPropertyStyle 对话框

class CPropertyStyle : public CDialogEx
{
	DECLARE_DYNAMIC(CPropertyStyle)

public:
	CPropertyStyle(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPropertyStyle();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPERTY_STYLE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
