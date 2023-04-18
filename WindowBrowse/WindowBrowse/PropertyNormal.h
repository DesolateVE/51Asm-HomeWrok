#pragma once
#include "afxdialogex.h"


// CPropertyNormal 对话框

class CPropertyNormal : public CDialogEx
{
	DECLARE_DYNAMIC(CPropertyNormal)

public:
	CPropertyNormal(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPropertyNormal();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPERTY_NORMAL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_EditTitle;
	CString m_EditHandle;
	CString m_EditProcess;
	CString m_EditRect;
	CString m_EditCilentRect;
	CString m_EditInstance;
	CString m_EditUserData;
	CString m_EditRestoredRect;
	CString m_EditMenuHandle;
	CComboBox m_EditWndByte;
};
