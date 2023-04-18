#pragma once
#include "afxdialogex.h"

// CFindWindowDlg 对话框

class CFindWindowDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFindWindowDlg)

public:
	CFindWindowDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CFindWindowDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FINDWINDOW_DIALOG };
#endif

	// 资源句柄
protected:
	HICON m_hScan;
	HICON m_hFrom;
	HCURSOR m_hCursor;

	// 状态
protected:
	bool m_bScaning;
	HWND m_hWndCur;

public:
	HWND getFindResult();
	bool getRadioSelect();

	// 成员函数
protected:
	bool isClickIcon(CPoint& point);
	void DrawBroder(HWND hWnd);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	// 控件变量
protected:
	CString m_editHandle;
	CString m_editTitle;
	CString m_editClass;
	CString m_editStyle;
	CString m_editRect;
	BOOL m_checkHiden;
	BOOL m_radioSelect;
	CStatic m_FindIcon;
	HWND m_hWndRtn;

	afx_msg void OnBnClickedButtonOK();
	afx_msg void OnBnClickedButtonCANCEL();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
};
