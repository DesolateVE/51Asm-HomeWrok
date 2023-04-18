// CFileViewDlg.cpp: 实现文件
//

#include "pch.h"
#include "ARK_User.h"
#include "afxdialogex.h"
#include "CFileViewDlg.h"



// CFileViewDlg 对话框

IMPLEMENT_DYNAMIC(CFileViewDlg, CDialogEx)

CFileViewDlg::CFileViewDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NORMAL_TAP, pParent)
{

}

CFileViewDlg::~CFileViewDlg()
{
}

void CFileViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_TreeCtrl);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
	DDX_Control(pDX, IDC_EDIT3, m_EditCtrl);
	DDX_Control(pDX, IDC_BUTTON2, m_ButtonCtrl);
}


void CFileViewDlg::OnClickDelete()
{
	theDriver.MyDeleteFile(m_szFileDelete.GetBuffer());
	UpdateList(GetFullPath(m_TreeCtrl.GetSelectedItem()).GetBuffer());
}

BEGIN_MESSAGE_MAP(CFileViewDlg, CDialogEx)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE1, &CFileViewDlg::OnItemexpandedTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CFileViewDlg::OnSelchangedTree1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CFileViewDlg::OnRclickFileList)
	ON_COMMAND(ID_NOTHING_32771, &CFileViewDlg::OnClickDelete)
END_MESSAGE_MAP()


// CFileViewDlg 消息处理程序


BOOL CFileViewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	DWORD dwStyle = m_ListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;							//选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;								//网格线（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_SUBITEMIMAGES;
	m_ListCtrl.SetExtendedStyle(dwStyle);						//设置扩展风格

	CRect ListCtrlRect;
	m_ListCtrl.GetClientRect(ListCtrlRect);
	DWORD dwWidth = ListCtrlRect.Width() / 10;

	m_ListCtrl.InsertColumn(0, L"名称", LVCFMT_LEFT, dwWidth * 6);
	m_ListCtrl.InsertColumn(1, L"修改日期", LVCFMT_LEFT, dwWidth * 2);
	m_ListCtrl.InsertColumn(2, L"类型", LVCFMT_LEFT, dwWidth * 1);
	m_ListCtrl.InsertColumn(3, L"大小", LVCFMT_RIGHT, ListCtrlRect.Width() - dwWidth * 9);

	m_ImageList.Create(24, 24, ILC_COLOR32, 10, 30);
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDR_DISK));			  // 0
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDR_DIRECTORY));		  // 1
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDR_FILE));			  // 2
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDR_PC));				  // 3

	m_TreeCtrl.SetImageList(&m_ImageList, TVSIL_NORMAL);
	m_ListCtrl.SetImageList(&m_ImageList, LVSIL_SMALL);

	HTREEITEM NodeCur = m_TreeCtrl.InsertItem(L"我的电脑", 3, 3);

	GetDiskSymbol(NodeCur);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CFileViewDlg::GetDiskSymbol(HTREEITEM hParent)
{
	CString FilePath;
	GetLogicalDriveStrings(MAXBYTE, FilePath.GetBufferSetLength(MAXBYTE));
	LPTSTR lpszVariable = FilePath.GetBuffer();

	while (*lpszVariable)
	{
		m_TreeCtrl.InsertItem(lpszVariable, 0, 0, hParent);
		lpszVariable += wcslen(lpszVariable) + 1;
	}
}

void CFileViewDlg::UpdateList(PWCHAR szPath)
{
	m_ListCtrl.DeleteAllItems();					// 清空列表内容准备重新写入

	DWORD FileNum = theDriver.FileSearch(szPath);

	if (FileNum == NULL)
	{
		return;
	}

	PCFILEINFO pInfo = theDriver.GetFileInfo(FileNum);

	CString		szFileSize;
	CString		szFileType;
	CString		szFileTime;
	ULONG		FileType;

	for (size_t i = 0; i < FileNum; i++)
	{
		if (pInfo[i].bFileType == FILE_ATTRIBUTE_DIRECTORY)
		{
			FileType = 1;
			szFileType = L"文件夹";
			szFileSize = L"";
		}
		else
		{
			ULONG ulFileSize = pInfo[i].qwFileSize.QuadPart / 1024;
			if (!ulFileSize) ulFileSize = 1;

			FileType = 2;
			szFileType = L"";
			szFileSize.Format(L"%i KB", ulFileSize);

		}

		FILETIME LocalTime;
		SYSTEMTIME SystemTime;

		FileTimeToLocalFileTime((PFILETIME)&pInfo[i].qwChangeTime, &LocalTime);
		FileTimeToSystemTime(&LocalTime, &SystemTime);
		szFileTime.Format(L"%d/%d/%d %d:%d", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute);

		m_ListCtrl.InsertItem(i, pInfo[i].szFileName, FileType);
		m_ListCtrl.SetItemText(i, 1, szFileTime);					// 修改日期
		m_ListCtrl.SetItemText(i, 2, szFileType);					// 类型
		m_ListCtrl.SetItemText(i, 3, szFileSize);					// 大小
	}

	free(pInfo);
}

CString CFileViewDlg::GetFullPath(HTREEITEM hCurrent)
{
	CString strTemp;
	CString strReturn;
	HTREEITEM hRoot = m_TreeCtrl.GetRootItem();		   // 指向的是 [我的电脑]
	HTREEITEM hThis = hCurrent;

	while (hThis != hRoot)
	{
		strTemp = m_TreeCtrl.GetItemText(hThis);    //检索列表中项目文字
		if (strTemp.Right(1) != L"\\")
		{
			strTemp += L"\\";
		}
		strReturn = strTemp + strReturn;
		hThis = m_TreeCtrl.GetParentItem(hThis); //返回父项目句柄
	}
	return strReturn;
}

void CFileViewDlg::OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if (pNMTreeView->action == TVC_BYMOUSE)
	{
		TVITEM item = pNMTreeView->itemNew;

		if (item.hItem == m_TreeCtrl.GetRootItem())		// [我的电脑] 不做处理
		{
			return;
		}

		UpdateList(GetFullPath(item.hItem).GetBuffer());
	}
}


void CFileViewDlg::OnItemexpandedTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	if (pNMTreeView->action == TVE_EXPAND)
	{
		TVITEM item = pNMTreeView->itemNew;								//发送\接受关于树形视图项目信息
		HTREEITEM hChild = m_TreeCtrl.GetChildItem(item.hItem);			//获取指定位置中的子项
		while (hChild)
		{

			HTREEITEM hGrandChild = m_TreeCtrl.GetChildItem(hChild);	// 清除子项的子项准备重新加载
			while (hGrandChild)
			{
				HTREEITEM hThis = hGrandChild;
				hGrandChild = m_TreeCtrl.GetNextItem(hGrandChild, TVGN_NEXT);
				m_TreeCtrl.DeleteItem(hThis);
			}

			DWORD FileNum = theDriver.FileSearch(GetFullPath(hChild).GetBuffer());

			if (FileNum == NULL)
			{
				goto Next;
			}

			PCFILEINFO pInfo = theDriver.GetFileInfo(FileNum);
			for (size_t i = 0; i < FileNum; i++)
			{
				if (pInfo[i].bFileType == FILE_ATTRIBUTE_DIRECTORY)		// 只加载文件夹
				{
					m_TreeCtrl.InsertItem(pInfo[i].szFileName, 1, 1, hChild);
				}
			}
			free(pInfo);

		Next:
			hChild = m_TreeCtrl.GetNextItem(hChild, TVGN_NEXT);			//获取树形控件TVGN_NEXT下兄弟项
		}
	}

	*pResult = 0;
}


void CFileViewDlg::OnRclickFileList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	m_szFileDelete = GetFullPath(m_TreeCtrl.GetSelectedItem());
	m_szFileDelete += m_ListCtrl.GetItemText(pNMItemActivate->iItem, 0);

	CPoint point(pNMItemActivate->ptAction);
	m_ListCtrl.ClientToScreen(&point);

	CMenu mn;
	mn.LoadMenu(IDR_MENU1);      //加载的资源ID
	CMenu* pSubMn = mn.GetSubMenu(0);
	pSubMn->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}
