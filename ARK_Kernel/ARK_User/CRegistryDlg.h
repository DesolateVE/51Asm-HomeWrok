#pragma once
#include <afxdialogex.h>

class CRegistryDlg :
    public CDialogEx
{
	DECLARE_DYNAMIC(CRegistryDlg)

public:
	CRegistryDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CRegistryDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	CString GetFullPath(HTREEITEM hCurrent);

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_TreeCtrl;
	CListCtrl m_ListCtrl;
	CEdit m_EditCtrl;
	CButton m_ButtonCtrl;
	CImageList m_ImageList;

	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpandedTree1(NMHDR* pNMHDR, LRESULT* pResult);
};

