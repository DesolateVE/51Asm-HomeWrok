// CQueryDlg.cpp: 实现文件
//

#include "pch.h"
#include "Server.h"
#include "afxdialogex.h"
#include "CQueryDlg.h"


// CQueryDlg 对话框

IMPLEMENT_DYNAMIC(CQueryDlg, CDialogEx)

CQueryDlg::CQueryDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_QUERY, pParent)
	, m_bStudent(FALSE)
	, m_bElective(FALSE)
	, m_bCourse(FALSE)
	, m_bClass(FALSE)
{
	m_nCurPage = NULL;
	m_nMaxPage = NULL;
	m_nCurLine = NULL;
}

CQueryDlg::~CQueryDlg()
{
}

void CQueryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST3, m_QueryList);
	DDX_Check(pDX, IDC_CHECK2, m_bStudent);
	DDX_Check(pDX, IDC_CHECK3, m_bElective);
	DDX_Check(pDX, IDC_CHECK4, m_bCourse);
	DDX_Check(pDX, IDC_CHECK5, m_bClass);
	DDX_Control(pDX, IDC_EDIT1, m_editInput);
	DDX_Control(pDX, IDC_COMBO1, m_cmbOption);
	DDX_Control(pDX, IDC_BUTTON2, m_btnPre);
	DDX_Control(pDX, IDC_BUTTON3, m_btnNext);
	DDX_Text(pDX, IDC_PAGE, m_PageFooter);
}


BEGIN_MESSAGE_MAP(CQueryDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CQueryDlg::OnBnClickedQuery)
	ON_BN_CLICKED(IDC_BUTTON2, &CQueryDlg::OnBnClickedButtonPre)
	ON_BN_CLICKED(IDC_BUTTON3, &CQueryDlg::OnBnClickedButtonNext)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CQueryDlg::OnSelchangeCombo)
END_MESSAGE_MAP()


// CQueryDlg 消息处理程序


void CQueryDlg::OnBnClickedQuery()
{
	// TODO: 在此添加控件通知处理程序代码

	UpdateData(TRUE);

	CString szInput;
	CString szOption;
	CString szLimit;
	CString szTable;
	CString szLink;

	m_editInput.GetWindowText(szInput);
	switch (m_cmbOption.GetCurSel())
	{
	case 0:
		szOption = "sname";
		szTable = "student";
		if (m_bElective)
		{
			szLink += "INNER JOIN elective ON student.sid = elective.student\n";
		}
		if (m_bClass)
		{
			szLink += "INNER JOIN class ON student.class = class.cid\n";
		}
		if (m_bCourse && m_bElective)
		{
			szLink += "INNER JOIN course ON elective.course = course.cid\n";
		}
		break;
	case 1:
		szOption = "cname";
		szTable = "class";
		if (m_bStudent)
		{
			szLink += "INNER JOIN student ON class.cid = student.class\n";
		}
		if (m_bElective && m_bStudent)
		{
			szLink += "INNER JOIN elective ON student.sid = elective.student\n";
		}
		if (m_bCourse && m_bElective && m_bStudent)
		{
			szLink += "INNER JOIN course ON elective.course = course.cid\n";
		}
		break;
	case 2:
		szOption = "cname";
		szTable = "course";
		if (m_bElective)
		{
			szLink += "INNER JOIN elective ON course.cid = elective.course\n";
		}
		if (m_bStudent && m_bElective)
		{
			szLink += "INNER JOIN student ON elective.student = student.sid\n";
		}
		if (m_bClass && m_bStudent && m_bElective)
		{
			szLink += "INNER JOIN class ON student.class = class.cid\n";
		}
		break;
	}

	if (!szInput.IsEmpty())
	{
		szInput = "%" + szInput + "%";
		szLimit.Format(" WHERE %s LIKE '%s'", szOption, szInput);
	}

	m_CurSql = szTable + "\n" + szLink + szLimit;

	GetPageCount();
	InitListData(1);
}


BOOL CQueryDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	DWORD dwStyle = m_QueryList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;                                //选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;                                    //网格线（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_FLATSB;                                       //扁平滚动条
	m_QueryList.SetExtendedStyle(dwStyle);							//设置扩展风格

	m_cmbOption.AddString("学生");
	m_cmbOption.AddString("班级");
	m_cmbOption.AddString("课程");

	SetPageFooter();
	InitButtonState();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CQueryDlg::SetPageFooter()
{
	m_PageFooter.Format("[%d]/[%d]", m_nCurPage, m_nMaxPage);
	UpdateData(FALSE);
}

void CQueryDlg::OnBnClickedButtonPre()
{
	// TODO: 在此添加控件通知处理程序代码
	InitListData(--m_nCurPage);
}

void CQueryDlg::OnBnClickedButtonNext()
{
	// TODO: 在此添加控件通知处理程序代码
	InitListData(++m_nCurPage);
}

void CQueryDlg::InitButtonState()
{
	bool EnablePre = m_nCurPage <= 1 ? FALSE : TRUE;
	bool EnableNext = m_nCurPage == m_nMaxPage ? FALSE : TRUE;
	m_btnPre.EnableWindow(EnablePre);
	m_btnNext.EnableWindow(EnableNext);
}

void CQueryDlg::GetPageCount()
{
	CString szSQL = EasyQueryTableCount(m_CurSql);
	CSQLData* pData = RequestSQLData(theApp.getSocket(), szSQL);
	if (pData->nRowCnt == NULL)
	{
		return;
	}
	CSQLData::unPackSqlData(pData);
	m_nMaxPage = std::stoi(CSQLData::unPackSqlData(pData));
	free(pData);
}

void CQueryDlg::InitListData(INT nPage)
{
	m_nCurPage = nPage;
	CString szSql = EasyQueryTable(m_CurSql, nPage);
	CSQLData* pData = RequestSQLData(theApp.getSocket(), szSql);
	EasyCreateList(pData, &m_QueryList);
	free(pData);

	SetPageFooter();	// 设定页脚
	InitButtonState();	// 按钮状态重置
}

void CQueryDlg::OnSelchangeCombo()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bStudent = FALSE;
	m_bClass = FALSE;
	m_bCourse = FALSE;
	switch (m_cmbOption.GetCurSel())
	{
	case 0:
		m_bStudent = TRUE;
		break;
	case 1:
		m_bClass = TRUE;
		break;
	case 2:
		m_bCourse = TRUE;
		break;
	}
	UpdateData(FALSE);
}
