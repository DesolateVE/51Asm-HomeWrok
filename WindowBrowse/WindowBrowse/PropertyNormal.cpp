// PropertyNormal.cpp: 实现文件
//

#include "pch.h"
#include "WindowBrowse.h"
#include "afxdialogex.h"
#include "PropertyNormal.h"


// CPropertyNormal 对话框

IMPLEMENT_DYNAMIC(CPropertyNormal, CDialogEx)

CPropertyNormal::CPropertyNormal(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROPERTY_NORMAL, pParent)
	, m_EditTitle(_T(""))
	, m_EditHandle(_T(""))
	, m_EditProcess(_T(""))
	, m_EditRect(_T(""))
	, m_EditCilentRect(_T(""))
	, m_EditInstance(_T(""))
	, m_EditUserData(_T(""))
	, m_EditRestoredRect(_T(""))
	, m_EditMenuHandle(_T(""))
{

}

CPropertyNormal::~CPropertyNormal()
{
}

void CPropertyNormal::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_EditTitle);
	DDX_Text(pDX, IDC_EDIT4, m_EditHandle);
	DDX_Text(pDX, IDC_EDIT2, m_EditProcess);
	DDX_Text(pDX, IDC_EDIT3, m_EditRect);
	DDX_Text(pDX, IDC_EDIT6, m_EditCilentRect);
	DDX_Text(pDX, IDC_EDIT7, m_EditInstance);
	DDX_Text(pDX, IDC_EDIT9, m_EditUserData);
	DDX_Text(pDX, IDC_EDIT5, m_EditRestoredRect);
	DDX_Text(pDX, IDC_EDIT8, m_EditMenuHandle);
	DDX_Control(pDX, IDC_COMBO1, m_EditWndByte);
}


BEGIN_MESSAGE_MAP(CPropertyNormal, CDialogEx)
END_MESSAGE_MAP()


// CPropertyNormal 消息处理程序
