#pragma once
#include "afxdialogex.h"

#define _SETPENWIDTH(_NUM) _NUM - 1
#define _GETPENWIDTH(_NUM) _NUM + 1

// CBrushSelectDialog 对话框

class CPenSelectDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CPenSelectDialog)

public:
	CPenSelectDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPenSelectDialog();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cmbPenStyle;
	CComboBox m_cmbPenWidth;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
	CMFCColorButton m_btnPenColor;

private:
	LOGPEN m_logPen;

public:
	LOGPEN getLogPen() const { return m_logPen; }
	void setLogPen(LOGPEN val) { m_logPen = val; }
};
