
// WindowBrowseDlg.h: 头文件
//

#pragma once
#include "PropertyDlg.h"
#include "FindWindowDlg.h"

// CWindowBrowseDlg 对话框
class CWindowBrowseDlg : public CDialogEx
{
	// 构造
public:
	CWindowBrowseDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WINDOWBROWSE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	HACCEL m_hAccel;
	CPropertyDlg m_dlgProperty;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

protected:
	void InitWinList();
	BOOL SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);

public:
	CTreeCtrl m_WinTree;
	CImageList m_ImageList;
	HTREEITEM m_TreeRoot;
	afx_msg void OnRclickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAcceleratorFindwindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

struct TREEINFO
{
	HTREEITEM hItem;
	CTreeCtrl* lpTree;
};

BOOL CALLBACK EnumWindowsProc(_In_ HWND hwnd, _In_ LPARAM lParam);
TREEINFO UpdateTree(HWND hwnd, TREEINFO* pInfo);