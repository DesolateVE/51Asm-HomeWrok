// CEditDataDlg.cpp: 实现文件
//

#include "pch.h"
#include "PE_Parsing.h"
#include "afxdialogex.h"
#include "CEditDataDlg.h"


// CEditDataDlg 对话框

IMPLEMENT_DYNAMIC(CEditDataDlg, CDialogEx)

CEditDataDlg::CEditDataDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INPUT_DIALOG, pParent)
{

}

CEditDataDlg::~CEditDataDlg()
{
}

void CEditDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT2, m_EditData);
}


void CEditDataDlg::setInput(CString szInput, RECT WndRect)
{
	m_szInput = szInput;
	m_WndRect = WndRect;
}

CString CEditDataDlg::getInput()
{
	return m_szInput;
}

BEGIN_MESSAGE_MAP(CEditDataDlg, CDialogEx)
END_MESSAGE_MAP()


// CEditDataDlg 消息处理程序


BOOL CEditDataDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	MoveWindow(&m_WndRect);
	m_EditData.SetWindowText(m_szInput);
	m_EditData.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
				   // 异常: OCX 属性页应返回 FALSE
}


void CEditDataDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	m_EditData.GetWindowText(m_szInput);
	CDialogEx::OnOK();
}
