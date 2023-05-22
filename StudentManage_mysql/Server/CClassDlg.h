#pragma once
#include "afxdialogex.h"


// CClassDlg 对话框

class CClassDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CClassDlg)

public:
	CClassDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CClassDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLASS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	int m_nCurPage;
	int m_nMaxPage;
	int m_nCurLine;

	void InitButtonState();
	void GetPageCount();
	void InitListData(INT nPage);
	void SetPageFooter();

public:
	CListCtrl m_ClassList;
	CEdit m_editName;
	CString m_PageFooter;
	CButton m_btnPre;
	CButton m_btnNext;
	
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonPre();
	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnDblclkStudentList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickStudentList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRightpopRefresh();
	afx_msg void OnRightpopDelete();
};
