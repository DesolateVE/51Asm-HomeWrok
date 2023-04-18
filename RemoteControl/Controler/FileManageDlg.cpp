// FileManageDlg.cpp: 实现文件
//

#include "pch.h"
#include "Controler.h"
#include "afxdialogex.h"
#include "FileManageDlg.h"
#include <thread>
#include <functional>


// CFileManageDlg 对话框

IMPLEMENT_DYNAMIC(CFileManageDlg, CDialogEx)

CFileManageDlg::CFileManageDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILEMANAGE_DIALOG, pParent)
{

}

CFileManageDlg::~CFileManageDlg()
{
}

void CFileManageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_FileView);
	DDX_Control(pDX, IDC_LIST1, m_FileDetail);
	DDX_Text(pDX, IDC_EDIT1, m_FilePath);
}


BEGIN_MESSAGE_MAP(CFileManageDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &CFileManageDlg::OnBnClickedBack)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CFileManageDlg::OnSelchangedFileView)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE1, &CFileManageDlg::OnItemexpandedFileView)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CFileManageDlg::OnRclickFileList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CFileManageDlg::OnDblclkFileList)
	ON_COMMAND(ID_32778, &CFileManageDlg::FileUpload)
	ON_COMMAND(ID_32779, &CFileManageDlg::FileDownload)
END_MESSAGE_MAP()


// CFileManageDlg 消息处理程序


void CFileManageDlg::GetLogicalDrives(HTREEITEM hParent)
{
	SendPackage<CFileDataRequest>(m_TCPInfo.m_Socket, true, "我的电脑");
	FileMessagePeek(m_Root);
}

void CFileManageDlg::AddSubDir(HTREEITEM hParent)
{
	CString strPath = GetFullPath(hParent);     //获取全路径
	strPath += "*.*";

	SendPackage<CFileDataRequest>(m_TCPInfo.m_Socket, true, strPath);
	FileMessagePeek(hParent);
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
	m_FileDetail.DeleteAllItems();
	SendPackage<CFileDataRequest>(m_TCPInfo.m_Socket, false, szFilePath);
	FileMessagePeek(NULL);
}

// 懒得再写一个函数了，如果传入的节点为空，则是生成 List 文件信息
void CFileManageDlg::FileMessagePeek(HTREEITEM hParent)
{
	char SockBuffer[0xFF];
	int iIndex = 0;
	while (true)
	{
		int iResult = recv(m_TCPInfo.m_Socket, SockBuffer, 0xFF, MSG_PEEK);
		int nlength = reinterpret_cast<CPackageHeader*>(SockBuffer)->m_Length;
		eSockType SockType = reinterpret_cast<CPackageHeader*>(SockBuffer)->m_PackType;
		if ((iResult >= nlength) && (SockType == eSockType::S2C_FILE))
		{
			iResult = recv(m_TCPInfo.m_Socket, SockBuffer, nlength, 0);
			auto pPack = reinterpret_cast<CFileDataSend*>(SockBuffer);
			if (pPack->m_eFileSign == CFileDataSend::FileSign::END)
			{
				break;
			}
			else
			{
				if (hParent == NULL)
				{
					m_FileDetail.InsertItem(iIndex++, pPack->m_szFileName);
					m_FileDetail.SetItemText(iIndex - 1, 1, "未知");
					m_FileDetail.SetItemText(iIndex - 1, 2, "未知");
					m_FileDetail.SetItemText(iIndex - 1, 3, "未知");
				}
				else
				{
					m_FileView.InsertItem(pPack->m_szFileName, hParent);
				}
			}
		}
		//if (iResult <= 0)
		//{
		//	AfxMessageBox("对方已断开链接");
		//	shutdown(m_TCPInfo.m_Socket, SD_SEND);
		//	closesocket(m_TCPInfo.m_Socket);
		//	break;
		//}
	}
}

void CFileManageDlg::FileTransThread(bool isDownload)
{
	HANDLE hFile;
	char SockBuffer[1500];

	if (isDownload)
	{
		CString FileName = "C:\\Users\\64239\\Desktop\\";
		FileName += m_CurrentFile;
		hFile = CreateFile(FileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		SendPackage<CFileTransRequest>(m_TCPInfo.m_Socket, true, m_FilePath + m_CurrentFile);
	}
	else
	{
		CFileDialog FileDlg(true);
		if (FileDlg.DoModal() == IDCANCEL)
		{
			return;
		}

		hFile = CreateFile(FileDlg.GetPathName(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		SendPackage<CFileTransRequest>(m_TCPInfo.m_Socket, false, FileDlg.GetFileName());
	}

	if (isDownload)
	{
		while (true)
		{
			int iResult = recv(m_TCPInfo.m_Socket, SockBuffer, 1500, MSG_PEEK);
			int nlength = reinterpret_cast<CPackageHeader*>(SockBuffer)->m_Length;
			eSockType SockType = reinterpret_cast<CPackageHeader*>(SockBuffer)->m_PackType;
			if ((iResult >= nlength) && (SockType == eSockType::COM_FILEBYTE))
			{
				iResult = recv(m_TCPInfo.m_Socket, SockBuffer, nlength, 0);
				auto pPack = reinterpret_cast<CFileByte*>(SockBuffer);
				if (!pPack->m_bSign)
				{
					break;
				}
				else
				{
					DWORD dwWrite;
					WriteFile(hFile, pPack->m_Data, pPack->m_Length - sizeof(CFileByte), &dwWrite, NULL);
				}
			}
		}
	}
	else
	{
		while (true)
		{
			DWORD dwRead = 1;
			if (ReadFile(hFile, SockBuffer, 1200, &dwRead, NULL) && dwRead)
			{
				SendPackage<CFileByte>(m_TCPInfo.m_Socket, true, SockBuffer, dwRead);
			}
			else
			{
				SendPackage<CFileByte>(m_TCPInfo.m_Socket, false, nullptr, NULL);
				break;
			}
		}
	}

	CloseHandle(hFile);
}

BOOL CFileManageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

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

	m_Root = m_FileView.InsertItem("我的电脑");

	GetLogicalDrives(m_Root);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
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

	m_FilePath = szFilePath;
	UpdateData(FALSE);

	szFilePath += "*.*";
	ShowFileInfo(szFilePath);
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

	// 刷一下路径显示
	m_FilePath = strPath;
	UpdateData(FALSE);

	strPath += "*.*";
	ShowFileInfo(strPath);

	*pResult = 0;
}


void CFileManageDlg::OnItemexpandedFileView(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

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



void CFileManageDlg::OnRclickFileList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	CPoint point;
	GetCursorPos(&point);
	CMenu mn;
	mn.LoadMenu(IDR_MENU1);      //加载的资源ID
	CMenu* pSubMn = mn.GetSubMenu(0);
	pSubMn->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);

	m_CurrentFile = m_FileDetail.GetItemText(pNMItemActivate->iItem, 0);

	*pResult = 0;
}


void CFileManageDlg::OnDblclkFileList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	CString szFile = m_FilePath;
	szFile += m_FileDetail.GetItemText(pNMItemActivate->iItem, 0);
	szFile += "\\";

	m_FilePath = szFile;
	UpdateData(FALSE);

	szFile += "*.*";
	ShowFileInfo(szFile);

	*pResult = 0;
}


void CFileManageDlg::FileUpload()
{
	// TODO: 在此添加命令处理程序代码
	auto func = std::bind(&CFileManageDlg::FileTransThread, this, false);
	std::thread tUpload(func);
	tUpload.detach();
}


void CFileManageDlg::FileDownload()
{
	// TODO: 在此添加命令处理程序代码

	if (m_CurrentFile.IsEmpty())
	{
		AfxMessageBox("未选择文件");
		return;
	}

	auto func = std::bind(&CFileManageDlg::FileTransThread, this, true);
	std::thread tDownLoad(func);
	tDownLoad.detach();
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
			UpdateData(FALSE);
			ShowFileInfo(m_FilePath + "*.*");
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
