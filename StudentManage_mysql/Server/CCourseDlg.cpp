// CCourseDlg.cpp: 实现文件
//

#include "pch.h"
#include "Server.h"
#include "afxdialogex.h"
#include "CCourseDlg.h"
#include "CEditUpdataDlg.h"

// CCourseDlg 对话框

IMPLEMENT_DYNAMIC(CCourseDlg, CDialogEx)

CCourseDlg::CCourseDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_COURSE, pParent)
{
	m_nCurPage = NULL;
	m_nMaxPage = NULL;
	m_nCurLine = NULL;
}

CCourseDlg::~CCourseDlg()
{
}

void CCourseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST3, m_CourseList);
	DDX_Control(pDX, IDC_EDIT1, m_editName);
	DDX_Control(pDX, IDC_BUTTON2, m_btnPre);
	DDX_Control(pDX, IDC_BUTTON3, m_btnNext);
	DDX_Text(pDX, IDC_PAGE, m_PageFooter);
}


BEGIN_MESSAGE_MAP(CCourseDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CCourseDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON2, &CCourseDlg::OnBnClickedButtonPre)
	ON_BN_CLICKED(IDC_BUTTON3, &CCourseDlg::OnBnClickedButtonNext)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST3, &CCourseDlg::OnDblclkStudentList)
	ON_NOTIFY(NM_RCLICK, IDC_LIST3, &CCourseDlg::OnRclickStudentList)
	ON_COMMAND(ID_RIGHTPOP_REFRESH, &CCourseDlg::OnRightpopRefresh)
	ON_COMMAND(ID_RIGHTPOP_DELETE, &CCourseDlg::OnRightpopDelete)
END_MESSAGE_MAP()


// CCourseDlg 消息处理程序

void CCourseDlg::InitButtonState()
{
	bool EnablePre = m_nCurPage <= 1 ? FALSE : TRUE;
	bool EnableNext = m_nCurPage == m_nMaxPage ? FALSE : TRUE;
	m_btnPre.EnableWindow(EnablePre);
	m_btnNext.EnableWindow(EnableNext);
}

void CCourseDlg::OnBnClickedButtonAdd()
{
	// TODO: 在此添加控件通知处理程序代码
	CString szName;
	CString szValues;

	m_editName.GetWindowText(szName);
	if (szName.IsEmpty())
	{
		AfxMessageBox("参数不能为空");
		return;
	}
	szValues.Format("'%s'", szName);

	CString szSql = EasyAddInfo("course", "cname", szValues);
	OnlySendRequest(theApp.getSocket(), szSql);

	GetPageCount();				// 可能会新增加一页
	InitListData(m_nMaxPage);	// 跳转到最后一页查看最新添加的数据
}

BOOL CCourseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	DWORD dwStyle = m_CourseList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;                                //选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;                                    //网格线（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_FLATSB;                                       //扁平滚动条
	m_CourseList.SetExtendedStyle(dwStyle);                        //设置扩展风格

	GetPageCount();		// 获取总页数
	InitListData(1);	// 初始化列表

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CCourseDlg::GetPageCount()
{
	CString szSql = EasyQueryTableCount("course");
	CSQLData* pData = RequestSQLData(theApp.getSocket(), szSql);
	CSQLData::unPackSqlData(pData);
	m_nMaxPage = std::stoi(CSQLData::unPackSqlData(pData));
	free(pData);
}

void CCourseDlg::InitListData(INT nPage)
{
	m_nCurPage = nPage;
	CString szSql = EasyQueryTable("course", nPage);
	CSQLData* pData = RequestSQLData(theApp.getSocket(), szSql);
	EasyCreateList(pData, &m_CourseList);
	free(pData);

	SetPageFooter();	// 设定页脚
	InitButtonState();	// 按钮状态重置
}

void CCourseDlg::SetPageFooter()
{
	m_PageFooter.Format("[%d]/[%d]", m_nCurPage, m_nMaxPage);
	UpdateData(FALSE);
}

void CCourseDlg::OnBnClickedButtonPre()
{
	// TODO: 在此添加控件通知处理程序代码
	InitListData(--m_nCurPage);
}

void CCourseDlg::OnBnClickedButtonNext()
{
	// TODO: 在此添加控件通知处理程序代码
	InitListData(++m_nCurPage);
}

void CCourseDlg::OnDblclkStudentList(NMHDR* pNMHDR, LRESULT* pResult)
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
	m_CourseList.GetSubItemRect(pNMItemActivate->iItem, pNMItemActivate->iSubItem, LVIR_LABEL, rect);
	CPoint screenPoint;
	::GetCursorPos(&screenPoint);
	CPoint clientPoint = pNMItemActivate->ptAction;
	rect.OffsetRect(screenPoint - clientPoint);

	char buff[32];
	CEditUpdataDlg Dlg;
	Dlg.m_Rect = rect;
	m_CourseList.GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem, buff, 32);
	Dlg.m_EditCtrl = buff;
	if (Dlg.DoModal() == IDOK)
	{
		CString szExpres;
		CString szSQL;
		switch (pNMItemActivate->iSubItem)
		{
		case 1:
			szExpres.Format("cname = '%s'", Dlg.m_EditCtrl);
			break;
		}

		m_CourseList.GetItemText(pNMItemActivate->iItem, 0, buff, 20);		// 取得主键描述
		szSQL = EasyUpdataInfo("course", szExpres, std::stoi(buff));
		OnlySendRequest(theApp.getSocket(), szSQL);
		InitListData(m_nCurPage);
	}

	*pResult = 0;
}


void CCourseDlg::OnRclickStudentList(NMHDR* pNMHDR, LRESULT* pResult)
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


void CCourseDlg::OnRightpopRefresh()
{
	// TODO: 在此添加命令处理程序代码
	GetPageCount();		// 获取总页数
	InitListData(1);	// 初始化列表
}


void CCourseDlg::OnRightpopDelete()
{
	// TODO: 在此添加命令处理程序代码
	char buff[20];
	m_CourseList.GetItemText(m_nCurLine, 0, buff, 20);		// 取得主键描述
	CString szSQL = EasyDeleteInfo("course", std::stoi(buff));
	OnlySendRequest(theApp.getSocket(), szSQL);
	InitListData(m_nCurPage);
}