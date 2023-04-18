#pragma once
#include "afxdialogex.h"


// CProcessViewDlg 对话框

class CProcessViewDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProcessViewDlg)

public:
	CProcessViewDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CProcessViewDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROCESS_TAP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void UpdatePageList(PCPAGEINFO pInfo, DWORD nEntryNum, CListCtrl* pList);
	void SetPageListStyle(CListCtrl* pList);
	void UpdateHexEdit(PBYTE pByte);
	static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	DECLARE_MESSAGE_MAP()



private:
	HANDLE	m_eProcess;
	HANDLE	m_pid;
	static const ULONG hexWidth = 16;
	static const ULONG hexHeight = 6;

public:
	CListCtrl m_ProcessList;
	CListCtrl m_ThreadList;
	CListCtrl m_ModuleList;
	CListCtrl m_PageList;
	CListCtrl m_PXEList;
	CListCtrl m_PPEList;
	CListCtrl m_PDEList;
	CListCtrl m_PTEList;
	CEdit m_HexValueEdit;
	CEdit m_AddressEdit;
	CEdit m_ValueEdit;

public:
	virtual BOOL OnInitDialog();
	void OnClickKill();
	afx_msg void OnBnClickedRead();
	afx_msg void OnBnClickedWrite();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	void UpdateProcessList();
	afx_msg void OnDblclkProcessList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickProcessList(NMHDR* pNMHDR, LRESULT* pResult);
};

class LinearAddress
{
public:
	LinearAddress(UINT64 LineAddress)
	{
		this->Value = LineAddress;
	}
	UINT32 getPML4() { return PML4; }
	UINT32 getDirectoryPtr() { return DirectoryPtr; }
	UINT32 getDirectory() { return Directory; }
	UINT32 getTable() { return Table; }
	UINT32 getOffset() { return Offset; }
	void setValue(UINT64 LineAddress) { this->Value = LineAddress; }

private:
	union
	{
		struct
		{
			UINT64 Offset : 12;
			UINT64 Table : 9;
			UINT64 Directory : 9;
			UINT64 DirectoryPtr : 9;
			UINT64 PML4 : 9;
			UINT64 : 16;
		};
		UINT64 Value;
	};
};