
// FileManageDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "FileManage.h"
#include "FileManageDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFileManageDlg 对话框



CFileManageDlg::CFileManageDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILEMANAGE_DIALOG, pParent)
	, m_FilePath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileManageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_FileView);
	DDX_Control(pDX, IDC_LIST1, m_FileDetail);
	DDX_Text(pDX, IDC_EDIT1, m_FilePath);
}

BEGIN_MESSAGE_MAP(CFileManageDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, &CFileManageDlg::OnBnClickedBack)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE1, &CFileManageDlg::OnItemexpandedFileView)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CFileManageDlg::OnSelchangedFileView)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CFileManageDlg::OnDblclkFileDetail)
END_MESSAGE_MAP()


enum { ICO_PC, ICO_DISK, ICO_FILE };

// CFileManageDlg 消息处理程序

BOOL CFileManageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	DWORD dwStyle = m_FileDetail.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;							//选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;								//网格线（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_SUBITEMIMAGES;
	m_FileDetail.SetExtendedStyle(dwStyle);						//设置扩展风格

	// TODO: 在此添加额外的初始化代码
	m_FileDetail.InsertColumn(0, "名称", LVCFMT_LEFT, 300, -1);
	m_FileDetail.InsertColumn(1, "修改日期", LVCFMT_LEFT, 300, -1);
	m_FileDetail.InsertColumn(2, "类型", LVCFMT_LEFT, 100, -1);
	m_FileDetail.InsertColumn(3, "大小", LVCFMT_LEFT, 100, -1);

	m_ImageList.Create(24, 24, ILC_COLOR32, 10, 30);
	m_FileView.SetImageList(&m_ImageList, TVSIL_NORMAL);
	m_FileDetail.SetImageList(&m_ImageList, LVSIL_SMALL);

	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON1));	//ICO_PC
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON2));	//ICO_DISK
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON3));	//ICO_FILE

	m_Root = m_FileView.InsertItem("我的电脑", ICO_PC, ICO_PC);
	GetLogicalDrives(m_Root);
	m_FileView.Expand(m_Root, TVE_EXPAND);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CFileManageDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFileManageDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFileManageDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFileManageDlg::GetLogicalDrives(HTREEITEM hParent)
{
	CString FilePath;
	GetLogicalDriveStrings(MAXBYTE, FilePath.GetBufferSetLength(MAXBYTE));
	LPTSTR lpszVariable = FilePath.GetBuffer();

	while (*lpszVariable)
	{
		m_FileView.InsertItem(lpszVariable, ICO_DISK, ICO_DISK, hParent);
		lpszVariable += strlen(lpszVariable) + 1;
	}
}

void CFileManageDlg::AddSubDir(HTREEITEM hParent)
{
	CString strPath = GetFullPath(hParent);     //获取全路径
	strPath += "*.*";

	CFileFind file;
	BOOL bContinue = file.FindFile(strPath);    //查找包含字符串的文件

	while (bContinue)
	{
		bContinue = file.FindNextFile();        //查找下一个文件
		if (file.IsDirectory() && !file.IsDots())
		{
			m_FileView.InsertItem(file.GetFileName(), ICO_FILE, ICO_FILE, hParent);
		}
	}
}

CString CFileManageDlg::GetFullPath(HTREEITEM hCurrent)
{
	CString strTemp;
	CString strReturn = "";
	while (hCurrent != m_Root)
	{
		strTemp = m_FileView.GetItemText(hCurrent);    //检索列表中项目文字
		if (strTemp.Right(1) != "\\")
		{
			strTemp += "\\";
		}
		strReturn = strTemp + strReturn;
		hCurrent = m_FileView.GetParentItem(hCurrent); //返回父项目句柄
	}
	return strReturn;
}


void CFileManageDlg::ShowFileInfo(CString szFilePath)
{
	int i, k = 0;
	CTime tempTime;
	CString tempStr;
	SHFILEINFO info;
	CFileFind file;

	szFilePath += "*.*";
	BOOL bContinue = file.FindFile(szFilePath);
	szFilePath.Delete(szFilePath.Find("*.*"), 3);


	m_FileDetail.DeleteAllItems();
	m_ImageList.SetImageCount(3);

	while (bContinue)
	{
		bContinue = file.FindNextFile();

		if (!file.IsDots())
		{
			SHGetFileInfo(szFilePath + file.GetFileName(), 0, &info, sizeof(&info), SHGFI_DISPLAYNAME | SHGFI_ICON | SHGFI_TYPENAME);

			if (file.IsDirectory())
			{
				i = ICO_FILE;
				tempStr = "";
			}
			else
			{
				i = m_ImageList.Add(info.hIcon);
				ULONGLONG ullFileSize = file.GetLength() / 1024;
				tempStr.Format("%I64d KB", ullFileSize ? ullFileSize : 1);	//防止文件过小
			}
			file.GetLastWriteTime(tempTime);
			m_FileDetail.InsertItem(k++, info.szDisplayName, i);
			m_FileDetail.SetItemText(k - 1, 1, tempTime.Format("%Y-%m-%d %H:%M:%S"));
			m_FileDetail.SetItemText(k - 1, 2, info.szTypeName);
			m_FileDetail.SetItemText(k - 1, 3, tempStr);
		}
	}

	m_FilePath = szFilePath;
	UpdateData(FALSE);
}

void CFileManageDlg::OnBnClickedBack()
{
	// TODO: 在此添加控件通知处理程序代码

	if (m_FilePath.GetLength() <= 3)
	{
		return;
	}

	CString szFilePath = m_FilePath;
	szFilePath.Delete(szFilePath.GetLength() - 1);
	int n = szFilePath.ReverseFind('\\');
	szFilePath.Delete(n + 1, szFilePath.GetLength() - n);

	ShowFileInfo(szFilePath);
}


void CFileManageDlg::OnItemexpandedFileView(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	// 应该判断一下是折叠还是打开，不然会重复查询
	if (pNMTreeView->action == TVE_EXPAND)
	{
		TVITEM item = pNMTreeView->itemNew;							//发送\接受关于树形视图项目信息
		HTREEITEM hChild = m_FileView.GetChildItem(item.hItem);		//获取指定位置中的子项
		while (hChild)
		{
			if (!m_FileView.ItemHasChildren(hChild))
			{
				AddSubDir(hChild);									//添加子目录
			}
			hChild = m_FileView.GetNextItem(hChild, TVGN_NEXT);		//获取树形控件TVGN_NEXT下兄弟项
		}
	}

	*pResult = 0;
}


void CFileManageDlg::OnSelchangedFileView(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	TVITEM item = pNMTreeView->itemNew;
	if (item.hItem == m_Root)
	{
		return;
	}

	CString strPath = GetFullPath(item.hItem);
	ShowFileInfo(strPath);

	// 自适应宽度 （效果不是很好）
	/*int nCount = m_FileDetail.GetItemCount();
	for (int i = 0; i < nCount; ++i)
	{
		m_FileDetail.SetColumnWidth(i, LVSCW_AUTOSIZE);
	}*/

	*pResult = 0;
}


void CFileManageDlg::OnDblclkFileDetail(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CString szFile = m_FilePath;
	szFile += m_FileDetail.GetItemText(pNMItemActivate->iItem, 0);

	if (m_FileDetail.GetItemText(pNMItemActivate->iItem, 2) == "文件夹")
	{
		szFile += "\\";
		ShowFileInfo(szFile);
	}
	else
	{
		ShellExecute(NULL, "open", szFile, NULL, NULL, SW_SHOWNORMAL);
	}

	*pResult = 0;
}


BOOL CFileManageDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if ((pMsg->message == WM_CHAR) && (pMsg->wParam == VK_RETURN))
	{
		if (GetFocus() == GetDlgItem(IDC_EDIT1))
		{
			UpdateData(TRUE);
			if (m_FilePath.Right(1) != "\\")
			{
				m_FilePath += "\\";
			}
			ShowFileInfo(m_FilePath);
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
