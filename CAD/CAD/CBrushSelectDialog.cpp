// CBrushSelectDialog.cpp: 实现文件
//

#include "pch.h"
#include "CAD.h"
#include "afxdialogex.h"
#include "CBrushSelectDialog.h"

// CBrushSelectDialog 对话框

IMPLEMENT_DYNAMIC(CBrushSelectDialog, CDialogEx)

CBrushSelectDialog::CBrushSelectDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
{

}

CBrushSelectDialog::~CBrushSelectDialog()
{
}

void CBrushSelectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cmbBrushStyle);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON1, m_btnBrushColor);
}


BEGIN_MESSAGE_MAP(CBrushSelectDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CBrushSelectDialog::OnBnClickedButton1)
END_MESSAGE_MAP()


// CBrushSelectDialog 消息处理程序


BOOL CBrushSelectDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	int nIndex = m_cmbBrushStyle.AddString(_T("实心笔刷"));
	m_cmbBrushStyle.SetItemData(nIndex, BS_SOLID);
	nIndex = m_cmbBrushStyle.AddString(_T("空心笔刷"));
	m_cmbBrushStyle.SetItemData(nIndex, BS_HOLLOW);

	m_btnBrushColor.EnableOtherButton("其他", FALSE, FALSE);

	//遍历选中对应配置
	for (int i = 0; i < m_cmbBrushStyle.GetCount(); i++)
	{
		if (m_cmbBrushStyle.GetItemData(i) == m_logBrush.lbStyle)
		{
			m_cmbBrushStyle.SetCurSel(i);
			break;
		}
	}

	//选中当前颜色
	m_btnBrushColor.SetColor(m_logBrush.lbColor);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CBrushSelectDialog::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	int nIndex = m_cmbBrushStyle.GetCurSel();
	m_logBrush.lbColor = m_btnBrushColor.GetColor();
	m_logBrush.lbStyle = m_cmbBrushStyle.GetItemData(nIndex);

	CDialogEx::OnOK();
}
