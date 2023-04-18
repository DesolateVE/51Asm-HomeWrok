#pragma once
#include "afxdialogex.h"
#include "PropertyNormal.h"
#include "PropertyStyle.h"
#include "PropertyWindow.h"
#include "PropertyClass.h"
#include "PropertyProcess.h"

// CPropertyDlg 对话框

class CPropertyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPropertyDlg)

public:
	CPropertyDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPropertyDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPERTY_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

protected:
	CPropertyNormal		m_pageNormal;
	CPropertyStyle		m_pageStyle;
	CPropertyWindow		m_pageWindow;
	CPropertyClass		m_pageClass;
	CPropertyProcess	m_pageProcess;
	CArray<CDialogEx*>	m_aryPage;

public:
	void setCurrentWnd(HWND hWnd);
	void DataRefresh(HWND hWnd);

protected:
	CTabCtrl m_TabCtrl;
	HWND m_hWndCur;

	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedRefresh();
	afx_msg void OnBnClickedSynch();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
};
