#pragma once
#include "afxdialogex.h"


// CQueryDlg 对话框

class CQueryDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CQueryDlg)

public:
	CQueryDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CQueryDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_QUERY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	int m_nCurPage;
	int m_nMaxPage;
	int m_nCurLine;
	CString m_CurSql;

	void InitButtonState();
	void GetPageCount();
	void InitListData(INT nPage);
	void SetPageFooter();

public:
	CListCtrl m_QueryList;
	BOOL m_bStudent;
	BOOL m_bElective;
	BOOL m_bCourse;
	BOOL m_bClass;
	CString m_PageFooter;
	CButton m_btnPre;
	CButton m_btnNext;
	CEdit m_editInput;
	CComboBox m_cmbOption;

	afx_msg void OnBnClickedQuery();
	afx_msg void OnBnClickedButtonPre();
	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnSelchangeCombo();
};
