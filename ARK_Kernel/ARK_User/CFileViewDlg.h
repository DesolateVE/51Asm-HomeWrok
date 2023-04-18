#pragma once
#include "afxdialogex.h"


// CFileViewDlg 对话框

class CFileViewDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFileViewDlg)

public:
	CFileViewDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CFileViewDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NORMAL_TAP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void OnClickDelete();

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_TreeCtrl;
	CListCtrl m_ListCtrl;
	CEdit m_EditCtrl;
	CButton m_ButtonCtrl;
	CImageList m_ImageList;

private:
	CString m_szFileDelete;

public:
	void GetDiskSymbol(HTREEITEM root);
	void UpdateList(PWCHAR szPath);
	CString GetFullPath(HTREEITEM hCurrent);

	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpandedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickFileList(NMHDR* pNMHDR, LRESULT* pResult);
};
