// CElectiveDlg.cpp: 实现文件
//

#include "pch.h"
#include "Server.h"
#include "afxdialogex.h"
#include "CElectiveDlg.h"
#include "CEditUpdataDlg.h"

// CElectiveDlg 对话框

IMPLEMENT_DYNAMIC(CElectiveDlg, CDialogEx)

CElectiveDlg::CElectiveDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ELECTIVE, pParent)
{
	m_nCurPage = NULL;
	m_nMaxPage = NULL;
	m_nCurLine = NULL;
}

CElectiveDlg::~CElectiveDlg()
{
}

void CElectiveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST3, m_ElectiveList);
	DDX_Control(pDX, IDC_COMBO2, m_cmbStudent);
	DDX_Control(pDX, IDC_COMBO1, m_cmbCourse);
	DDX_Control(pDX, IDC_BUTTON2, m_btnPre);
	DDX_Control(pDX, IDC_BUTTON3, m_btnNext);
	DDX_Text(pDX, IDC_PAGE, m_PageFooter);
}


void CElectiveDlg::InitButtonState()
{
	bool EnablePre = m_nCurPage <= 1 ? FALSE : TRUE;
	bool EnableNext = m_nCurPage == m_nMaxPage ? FALSE : TRUE;
	m_btnPre.EnableWindow(EnablePre);
	m_btnNext.EnableWindow(EnableNext);
}

void CElectiveDlg::GetPageCount()
{
	CString szSql = EasyQueryTableCount("elective");
	CSQLData* pData = RequestSQLData(theApp.getSocket(), szSql);
	CSQLData::unPackSqlData(pData);
	m_nMaxPage = std::stoi(CSQLData::unPackSqlData(pData));
	free(pData);
}

void CElectiveDlg::InitListData(INT nPage)
{
	m_nCurPage = nPage;
	CString szSql = EasyQueryTable("elective", nPage);
	CSQLData* pData = RequestSQLData(theApp.getSocket(), szSql);
	EasyCreateList(pData, &m_ElectiveList);
	free(pData);

	SetPageFooter();	// 设定页脚
	InitButtonState();	// 按钮状态重置
}

void CElectiveDlg::InitCmbBoxData()
{
	CString szSql = EasyQueryTableFastInfo("course");
	CSQLData* pData = RequestSQLData(theApp.getSocket(), szSql);
	EasyCreateCmbBox(pData, &m_cmbCourse);
	free(pData);

	szSql = EasyQueryTableFastInfo("student");
	pData = RequestSQLData(theApp.getSocket(), szSql);
	EasyCreateCmbBox(pData, &m_cmbStudent);
	free(pData);
}

void CElectiveDlg::SetPageFooter()
{
	m_PageFooter.Format("[%d]/[%d]", m_nCurPage, m_nMaxPage);
	UpdateData(FALSE);
}

BEGIN_MESSAGE_MAP(CElectiveDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CElectiveDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON2, &CElectiveDlg::OnBnClickedButtonPre)
	ON_BN_CLICKED(IDC_BUTTON3, &CElectiveDlg::OnBnClickedButtonNext)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST3, &CElectiveDlg::OnDblclkStudentList)
	ON_NOTIFY(NM_RCLICK, IDC_LIST3, &CElectiveDlg::OnRclickStudentList)
	ON_COMMAND(ID_RIGHTPOP_REFRESH, &CElectiveDlg::OnRightpopRefresh)
	ON_COMMAND(ID_RIGHTPOP_DELETE, &CElectiveDlg::OnRightpopDelete)
END_MESSAGE_MAP()


// CElectiveDlg 消息处理程序


void CElectiveDlg::OnBnClickedButtonAdd()
{
	// TODO: 在此添加控件通知处理程序代码
	CString szValues;
	CString szValue1;
	CString szValue2;

	INT nIndex = m_cmbCourse.GetCurSel();
	if (nIndex == -1)
	{
		m_cmbCourse.GetWindowText(szValue1);
	}
	else
	{
		DWORD_PTR nID = m_cmbCourse.GetItemData(nIndex);
		szValue1.Format("%d", nID);
	}

	nIndex = m_cmbStudent.GetCurSel();
	if (nIndex == -1)
	{
		m_cmbStudent.GetWindowText(szValue2);
	}
	else
	{
		DWORD_PTR nID = m_cmbStudent.GetItemData(nIndex);
		szValue2.Format("%d", nID);
	}

	if (szValue1.IsEmpty() || szValue2.IsEmpty())
	{
		AfxMessageBox("参数不能为空");
		return;
	}
	szValues.Format("%s,%s", szValue2, szValue1);

	CString szSql = EasyAddInfo("elective", "student,course", szValues);
	OnlySendRequest(theApp.getSocket(), szSql);

	GetPageCount();				// 可能会新增加一页
	InitListData(m_nMaxPage);	// 跳转到最后一页查看最新添加的数据
}


void CElectiveDlg::OnBnClickedButtonPre()
{
	// TODO: 在此添加控件通知处理程序代码
	InitListData(--m_nCurPage);
}


void CElectiveDlg::OnBnClickedButtonNext()
{
	// TODO: 在此添加控件通知处理程序代码
	InitListData(++m_nCurPage);
}


BOOL CElectiveDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	DWORD dwStyle = m_ElectiveList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;                                //选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;                                    //网格线（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_FLATSB;                                       //扁平滚动条
	m_ElectiveList.SetExtendedStyle(dwStyle);                       //设置扩展风格

	GetPageCount();		// 获取总页数
	InitListData(1);	// 初始化列表
	InitCmbBoxData();	// 初始化下拉框

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CElectiveDlg::OnDblclkStudentList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	// ID不可修改，点击ID无响应
	if (pNMItemActivate->iSubItem == 0)
	{
		*pResult = 0;
		return;
	}

	// 可能是因为选项卡机制的原因，ClientToScreen() 转换的坐标不准确，这里利用鼠标坐标重新校准偏移
	CRect rect;
	m_ElectiveList.GetSubItemRect(pNMItemActivate->iItem, pNMItemActivate->iSubItem, LVIR_LABEL, rect);
	CPoint screenPoint;
	::GetCursorPos(&screenPoint);
	CPoint clientPoint = pNMItemActivate->ptAction;
	rect.OffsetRect(screenPoint - clientPoint);

	char buff[32];
	CEditUpdataDlg Dlg;
	Dlg.m_Rect = rect;
	m_ElectiveList.GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem, buff, 32);
	Dlg.m_EditCtrl = buff;
	if (Dlg.DoModal() == IDOK)
	{
		CString szExpres;
		CString szSQL;
		switch (pNMItemActivate->iSubItem)
		{
		case 1:
			szExpres.Format("student = '%s'", Dlg.m_EditCtrl);
			break;
		case 2:
			szExpres.Format("course = %s", Dlg.m_EditCtrl);
			break;
		}

		m_ElectiveList.GetItemText(pNMItemActivate->iItem, 0, buff, 20);		// 取得主键描述
		szSQL = EasyUpdataInfo("elective", szExpres, std::stoi(buff));
		OnlySendRequest(theApp.getSocket(), szSQL);
		InitListData(m_nCurPage);
	}

	*pResult = 0;
}


void CElectiveDlg::OnRclickStudentList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	m_nCurLine = pNMItemActivate->iItem;

	//获取窗口资源并创建
	CPoint point;
	::GetCursorPos(&point);
	CMenu mn;
	mn.LoadMenu(IDR_MENU1);      //加载的资源ID
	CMenu* pSubMn = mn.GetSubMenu(0);
	if (m_nCurLine == -1) { pSubMn->EnableMenuItem(ID_RIGHTPOP_DELETE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED); }
	pSubMn->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	*pResult = 0;
}


void CElectiveDlg::OnRightpopRefresh()
{
	// TODO: 在此添加命令处理程序代码
	GetPageCount();		// 获取总页数
	InitListData(1);	// 初始化列表
	InitCmbBoxData();	// 初始化下拉框
}


void CElectiveDlg::OnRightpopDelete()
{
	// TODO: 在此添加命令处理程序代码
	char buff[20];
	m_ElectiveList.GetItemText(m_nCurLine, 0, buff, 20);		// 取得主键描述
	CString szSQL = EasyDeleteInfo("elective", std::stoi(buff));
	OnlySendRequest(theApp.getSocket(), szSQL);
	InitListData(m_nCurPage);
}
