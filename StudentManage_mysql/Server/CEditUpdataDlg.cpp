// CEditUpdataDlg.cpp: 实现文件
//

#include "pch.h"
#include "Server.h"
#include "afxdialogex.h"
#include "CEditUpdataDlg.h"


// CEditUpdataDlg 对话框

IMPLEMENT_DYNAMIC(CEditUpdataDlg, CDialogEx)

CEditUpdataDlg::CEditUpdataDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDITUPDATA, pParent)
	, m_EditCtrl(_T(""))
{

}

CEditUpdataDlg::~CEditUpdataDlg()
{
}

void CEditUpdataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, m_EditCtrl);
}


BEGIN_MESSAGE_MAP(CEditUpdataDlg, CDialogEx)
END_MESSAGE_MAP()


// CEditUpdataDlg 消息处理程序


void CEditUpdataDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	UpdateData(TRUE);

	CDialogEx::OnOK();
}


BOOL CEditUpdataDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	MoveWindow(m_Rect);
	UpdateData(FALSE);
	GetDlgItem(IDC_EDIT2)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
