#pragma once
#include "afxdialogex.h"


// CEditDataDlg 对话框

class CEditDataDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEditDataDlg)

public:
	CEditDataDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CEditDataDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INPUT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EditData;
	CString m_szInput;
	RECT m_WndRect;

	void setInput(CString szInput,RECT WndRect);
	CString getInput();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
