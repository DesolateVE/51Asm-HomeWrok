#include "pch.h"
#include "ARK_User.h"
#include "CRegistryDlg.h"

IMPLEMENT_DYNAMIC(CRegistryDlg, CDialogEx)

CRegistryDlg::CRegistryDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NORMAL_TAP, pParent)
{

}

CRegistryDlg::~CRegistryDlg()
{
}

void CRegistryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_TreeCtrl);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
	DDX_Control(pDX, IDC_EDIT3, m_EditCtrl);
	DDX_Control(pDX, IDC_BUTTON2, m_ButtonCtrl);
}

BEGIN_MESSAGE_MAP(CRegistryDlg, CDialogEx)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE1, &CRegistryDlg::OnItemexpandedTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CRegistryDlg::OnSelchangedTree1)
END_MESSAGE_MAP()


BOOL CRegistryDlg::OnInitDialog()
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
	ULONG nNum = 0;

	m_ListCtrl.InsertColumn(nNum++, L"名称", LVCFMT_LEFT, dwWidth * 2);
	m_ListCtrl.InsertColumn(nNum++, L"类型", LVCFMT_LEFT, dwWidth * 2);
	m_ListCtrl.InsertColumn(nNum++, L"数据", LVCFMT_LEFT, ListCtrlRect.Width() - dwWidth * 4);

	m_ImageList.Create(24, 24, ILC_COLOR32, 10, 30);
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDR_DIRECTORY));		  // 0
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDR_PC));				  // 1
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDR_KEYSTRING));		  // 2
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDR_KEYNUM));			  // 3

	m_TreeCtrl.SetImageList(&m_ImageList, TVSIL_NORMAL);
	m_ListCtrl.SetImageList(&m_ImageList, LVSIL_SMALL);

	HTREEITEM NodeCur = m_TreeCtrl.InsertItem(L"我的电脑", 1, 1);
	m_TreeCtrl.InsertItem(L"HKEY_LOCAL_MACHINE", 0, 0, NodeCur);
	m_TreeCtrl.InsertItem(L"HKEY_USERS", 0, 0, NodeCur);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

CString CRegistryDlg::GetFullPath(HTREEITEM hCurrent)
{
	CString strTemp;
	CString strReturn;
	HTREEITEM hRoot = m_TreeCtrl.GetRootItem();		    // 指向的是 [我的电脑]
	HTREEITEM hThis = hCurrent;

	while (hThis != hRoot)
	{
		strTemp = m_TreeCtrl.GetItemText(hThis);		//检索列表中项目文字

		if (strTemp == L"HKEY_LOCAL_MACHINE")
		{
			strTemp = L"\\Registry\\Machine";
		}
		if (strTemp == L"HKEY_USERS")
		{
			strTemp = L"\\Registry\\User";
		}

		if (strTemp.Right(1) != L"\\")
		{
			strTemp += L"\\";
		}
		strReturn = strTemp + strReturn;
		hThis = m_TreeCtrl.GetParentItem(hThis); //返回父项目句柄
	}

	return strReturn;
}

void CRegistryDlg::OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if (pNMTreeView->action == TVC_BYMOUSE)
	{
		m_ListCtrl.DeleteAllItems();					// 清空列表内容准备重新写入

		TVITEM item = pNMTreeView->itemNew;

		if (item.hItem == m_TreeCtrl.GetRootItem())		// [我的电脑] 不做处理
		{
			return;
		}

		DWORD RegNum = theDriver.RegisterSearch(GetFullPath(item.hItem).GetBuffer());

		if (RegNum == NULL)
		{
			return;
		}

		DWORD		ImageType;
		CString		szType;
		CString		szData;
		CString		szTemp;
		PCREGISTERINFO pInfo = theDriver.GetRegisterInfo(RegNum);

		ULONG Index = 0;
		for (size_t i = 0; i < RegNum; i++)
		{
			if (pInfo[i].bType == 0xFFFF)
			{
				continue;
			}

			ImageType = 3;
			szType = L"UNDEFINE";

			switch (pInfo[i].bType)
			{
			case REG_SZ:
				ImageType = 2;
				szType = L"REG_SZ";
				szData.Format(L"%s", pInfo[i].ucDate);
				break;
			case REG_EXPAND_SZ:
				ImageType = 2;
				szType = L"REG_EXPAND_SZ";
				szData.Format(L"%s", pInfo[i].ucDate);
				break;
			case REG_NONE:
				szType = L"REG_NONE";
				szData = L"0x00000000";
				break;
			case REG_DWORD:
				szType = L"REG_DWORD";
				szData.Format(L"0x%08X", *(PDWORD)pInfo[i].ucDate);
				break;
			case REG_BINARY:
				szType = L"REG_BINARY";
			default:
				szData.Empty();
				for (size_t k = 0; k < pInfo[i].DateLength; k++)
				{
					szTemp.Format(L"%02X ", pInfo[i].ucDate[k]);
					szData += szTemp;
				}
				break;
			}

			m_ListCtrl.InsertItem(Index, pInfo[i].szName, ImageType);
			m_ListCtrl.SetItemText(Index, 1, szType);
			m_ListCtrl.SetItemText(Index, 2, szData);

			Index++;
		}

		free(pInfo);
	}
}


void CRegistryDlg::OnItemexpandedTree1(NMHDR* pNMHDR, LRESULT* pResult)
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

			DWORD RegNum = theDriver.RegisterSearch(GetFullPath(hChild).GetBuffer());

			if (RegNum == NULL)
			{
				goto Next;
			}

			PCREGISTERINFO pInfo = theDriver.GetRegisterInfo(RegNum);
			for (size_t i = 0; i < RegNum; i++)
			{
				if (pInfo[i].bType == 0xFFFF)							// 只加载键
				{
					m_TreeCtrl.InsertItem(pInfo[i].szName, 0, 0, hChild);
				}
			}
			free(pInfo);

		Next:
			hChild = m_TreeCtrl.GetNextItem(hChild, TVGN_NEXT);			//获取树形控件TVGN_NEXT下兄弟项
		}
	}

	*pResult = 0;
}