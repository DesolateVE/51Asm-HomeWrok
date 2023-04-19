
// FileManageDlg.h: 头文件
//

#pragma once


// CFileManageDlg 对话框
class CFileManageDlg : public CDialogEx
{
	// 构造
public:
	CFileManageDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILEMANAGE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_FileView;
	CListCtrl m_FileDetail;
	CString m_FilePath;
	HTREEITEM m_Root;
	CImageList m_ImageList;

	void GetLogicalDrives(HTREEITEM hParent);
	void AddSubDir(HTREEITEM hParent);
	CString GetFullPath(HTREEITEM hCurrent);
	void ShowFileInfo(CString szFilePath);

	afx_msg void OnBnClickedBack();
	afx_msg void OnItemexpandedFileView(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedFileView(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkFileDetail(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
