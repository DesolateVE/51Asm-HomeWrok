#pragma once
#include "afxdialogex.h"


// CBrushSelectDialog 对话框

class CBrushSelectDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CBrushSelectDialog)

public:
	CBrushSelectDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CBrushSelectDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CComboBox m_cmbBrushStyle;
	CMFCColorButton m_btnBrushColor;
	afx_msg void OnBnClickedButton1();

private:
	LOGBRUSH m_logBrush;

public:
	LOGBRUSH getLogBrush() const { return m_logBrush; }
	void setLogBrush(LOGBRUSH val) { m_logBrush = val; }
};
