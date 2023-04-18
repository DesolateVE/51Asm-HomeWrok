#pragma once
#include "afxdialogex.h"
#include "SocketTool.h"
#include "CPackage.h"

// CFileManageDlg 对话框

class CFileManageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFileManageDlg)

public:
	CFileManageDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CFileManageDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILEMANAGE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	CSocketInfo m_TCPInfo;
	CString		m_CurrentFile;

public:
	CTreeCtrl m_FileView;
	CListCtrl m_FileDetail;
	CString m_FilePath;
	HTREEITEM m_Root;
	CImageList m_ImageList;

public:
	void GetLogicalDrives(HTREEITEM hParent);
	void AddSubDir(HTREEITEM hParent);
	CString GetFullPath(HTREEITEM hCurrent);
	void ShowFileInfo(CString szFilePath);
	void FileMessagePeek(HTREEITEM hParent);
	void FileTransThread(bool isDownload);

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBack();
	afx_msg void OnSelchangedFileView(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpandedFileView(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickFileList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkFileList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void FileUpload();
	afx_msg void FileDownload();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
