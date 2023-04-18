// CBrushSelectDialog.cpp: 实现文件
//

#include "pch.h"
#include "CAD.h"
#include "afxdialogex.h"
#include "CPenSelectDialog.h"


// CBrushSelectDialog 对话框

IMPLEMENT_DYNAMIC(CPenSelectDialog, CDialogEx)

CPenSelectDialog::CPenSelectDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{


}

CPenSelectDialog::~CPenSelectDialog()
{
}

void CPenSelectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cmbPenStyle);
	DDX_Control(pDX, IDC_COMBO2, m_cmbPenWidth);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON1, m_btnPenColor);
}


BEGIN_MESSAGE_MAP(CPenSelectDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CPenSelectDialog::OnBnClickedButton1)
END_MESSAGE_MAP()


// CBrushSelectDialog 消息处理程序


BOOL CPenSelectDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	int nIndex = m_cmbPenStyle.AddString("———");
	m_cmbPenStyle.SetItemData(nIndex, PS_SOLID);
	nIndex = m_cmbPenStyle.AddString("--------");
	m_cmbPenStyle.SetItemData(nIndex, PS_DASH);
	nIndex = m_cmbPenStyle.AddString("........");
	m_cmbPenStyle.SetItemData(nIndex, PS_DOT);
	nIndex = m_cmbPenStyle.AddString("_._._._.");
	m_cmbPenStyle.SetItemData(nIndex, PS_DASHDOT);
	nIndex = m_cmbPenStyle.AddString("_.._.._.");
	m_cmbPenStyle.SetItemData(nIndex, PS_DASHDOTDOT);
	nIndex = m_cmbPenStyle.SetCurSel(0);

	char szNum[20];
	for (int i = 1; i < 20; i++)
	{
		sprintf_s(szNum, 20, "== %d ==", i);
		m_cmbPenWidth.AddString(szNum);
	}

	m_btnPenColor.EnableOtherButton("其他", FALSE, FALSE);

	for (int i = 0; i < m_cmbPenStyle.GetCount(); i++)
	{
		if (m_cmbPenStyle.GetItemData(i) == m_logPen.lopnStyle)
		{
			m_cmbPenStyle.SetCurSel(i);
			break;
		}
	}
	m_btnPenColor.SetColor(m_logPen.lopnColor);
	m_cmbPenWidth.SetCurSel(_SETPENWIDTH(m_logPen.lopnWidth.x));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CPenSelectDialog::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	int nIndex = m_cmbPenStyle.GetCurSel();
	m_logPen.lopnColor = m_btnPenColor.GetColor();
	m_logPen.lopnStyle = m_cmbPenStyle.GetItemData(nIndex);
	m_logPen.lopnWidth = POINT{ _GETPENWIDTH(m_cmbPenWidth.GetCurSel()) ,NULL };

	CDialogEx::OnOK();
}