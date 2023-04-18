#pragma once
#include "afxdialogex.h"


// CPropertyProcess 对话框

class CPropertyProcess : public CDialogEx
{
	DECLARE_DYNAMIC(CPropertyProcess)

public:
	CPropertyProcess(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPropertyProcess();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPERTY_PROCESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
