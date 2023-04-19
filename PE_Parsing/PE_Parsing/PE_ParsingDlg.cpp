
// PE_ParsingDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "PE_Parsing.h"
#include "PE_ParsingDlg.h"
#include "afxdialogex.h"
#include "CEditDataDlg.h"

#include <fstream>
#include <string>

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


// CPEParsingDlg 对话框



CPEParsingDlg::CPEParsingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PE_PARSING_DIALOG, pParent)
	, m_szFilePath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	ZeroMemory(&m_peStruct, sizeof(m_peStruct));
}

void CPEParsingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MFCEDITBROWSE2, m_szFilePath);
	DDX_Control(pDX, IDC_TREE1, m_TreeCtrl);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
}

BEGIN_MESSAGE_MAP(CPEParsingDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_MFCBUTTON3, &CPEParsingDlg::OnBnClickedMfcbutton3)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CPEParsingDlg::OnSelchangedTree1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CPEParsingDlg::OnDblclkList1)
	ON_BN_CLICKED(IDC_MFCBUTTON4, &CPEParsingDlg::OnBnClickedMfcbutton4)
END_MESSAGE_MAP()


// CPEParsingDlg 消息处理程序

BOOL CPEParsingDlg::OnInitDialog()
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

	// TODO: 在此添加额外的初始化代码

	LONG lStyle = GetWindowLong(m_ListCtrl.m_hWnd, GWL_STYLE);		//获取当前窗口style
	//lStyle &= ~LVS_TYPEMASK;										//清除显示方式位
	lStyle |= LVS_REPORT;											//设置style
	lStyle |= LVS_SINGLESEL;										//单选模式
	SetWindowLong(m_ListCtrl.m_hWnd, GWL_STYLE, lStyle);			//写入设置
	DWORD dwStyle = m_ListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;								//选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;									//网格线（只适用与report风格的listctrl）
	//dwStyle |= LVS_EX_CHECKBOXES;									//item前生成checkbox控件
	dwStyle |= LVS_EX_FLATSB;										//扁平滚动条
	m_ListCtrl.SetExtendedStyle(dwStyle);							//设置扩展风格

	int iIdx = 0;
	m_ListCtrl.InsertColumn(iIdx++, TEXT("pFile"), LVCFMT_LEFT, 100);
	m_ListCtrl.InsertColumn(iIdx++, TEXT("Description"), LVCFMT_LEFT, 150);
	m_ListCtrl.InsertColumn(iIdx++, TEXT("Value"), LVCFMT_LEFT, 200);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPEParsingDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CPEParsingDlg::OnPaint()
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
HCURSOR CPEParsingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#define ID_IMAGE_DOS_HEADER					1
#define ID_IMAGE_NT_HEADERS					2
#define ID_IMAGE_FILE_HEADER				3
#define ID_IMAGE_OPTIONAL_HEADER			4
#define ID_IMAGE_SECTION_HEADER				5
#define ID_IMAGE_DIRECTORY_ENTRY_EXPORT		6
#define ID_IMAGE_DIRECTORY_ENTRY_IMPORT		7


#define VALUE_TYPE_STR				1
#define VALUE_TYPE_NUM				2
#define VALUE_TYPE_UNUSEFUL			3


#define BEGIN_MAKE_LIST(OFFSET) \
CString szFormat; \
DWORD offset = OFFSET; \
int nRow = 0;

#define MAKE_LIST_ITEAM(STRUCT,VARIABLE) \
szFormat.Format(TEXT("%04X"), offset); \
m_ListCtrl.InsertItem(nRow, szFormat); \
m_ListCtrl.SetItemText(nRow, 1, TEXT(#VARIABLE)); \
szFormat.Format(TEXT("%%0%dX"), sizeof(STRUCT.VARIABLE) * 2); \
szFormat.Format(szFormat, STRUCT.VARIABLE); \
m_ListCtrl.SetItemText(nRow, 2, szFormat); \
m_ListCtrl.SetItemData(nRow, VALUE_TYPE_NUM); \
++nRow; \
offset += sizeof(STRUCT.VARIABLE);

#define MAKE_LIST_ITEAMS(STRUCT,VARIABLE) \
for(size_t i = 0; i < ARRAYSIZE(STRUCT.VARIABLE); i++) \
{ \
szFormat.Format(TEXT("%04X"), offset); \
m_ListCtrl.InsertItem(nRow, szFormat); \
szFormat.Format(TEXT("%s[%d]"), TEXT(#VARIABLE), i); \
m_ListCtrl.SetItemText(nRow, 1, szFormat); \
szFormat.Format(TEXT("%%0%dX"), sizeof(STRUCT.VARIABLE[0]) * 2); \
szFormat.Format(szFormat, STRUCT.VARIABLE[i]); \
m_ListCtrl.SetItemText(nRow, 2, szFormat); \
m_ListCtrl.SetItemData(nRow, VALUE_TYPE_NUM); \
++nRow; \
offset += sizeof(STRUCT.VARIABLE[0]); \
}

#define MAKE_LIST_SZITEAM(STRUCT,VARIABLE) \
szFormat.Format(TEXT("%04X"), offset); \
m_ListCtrl.InsertItem(nRow, szFormat); \
m_ListCtrl.SetItemText(nRow, 1, TEXT(#VARIABLE)); \
szFormat.Format(TEXT("%S"), (char*)(STRUCT.VARIABLE)); \
m_ListCtrl.SetItemText(nRow, 2, szFormat); \
m_ListCtrl.SetItemData(nRow, VALUE_TYPE_STR); \
++nRow; \
offset += ARRAYSIZE(STRUCT.VARIABLE);

#define MAKE_LIST_NEWLINE() \
m_ListCtrl.InsertItem(nRow, TEXT("---")); \
m_ListCtrl.SetItemText(nRow, 1, TEXT("---")); \
m_ListCtrl.SetItemText(nRow, 2, TEXT("---")); \
m_ListCtrl.SetItemData(nRow, VALUE_TYPE_UNUSEFUL); \
++nRow;

#define END_MAKE_LIST()




void CPEParsingDlg::UpdateListView(int nSelect)
{
	m_ListCtrl.DeleteAllItems();

	using namespace std;
	switch (nSelect)
	{
	case ID_IMAGE_DOS_HEADER:
	{
		auto& DosHeader = m_peStruct.DOS_HEADER.header;
		BEGIN_MAKE_LIST(m_peStruct.DOS_HEADER.offset)
			MAKE_LIST_ITEAM(DosHeader, e_magic)
			MAKE_LIST_ITEAM(DosHeader, e_cblp)
			MAKE_LIST_ITEAM(DosHeader, e_cp)
			MAKE_LIST_ITEAM(DosHeader, e_crlc)
			MAKE_LIST_ITEAM(DosHeader, e_cparhdr)
			MAKE_LIST_ITEAM(DosHeader, e_minalloc)
			MAKE_LIST_ITEAM(DosHeader, e_maxalloc)
			MAKE_LIST_ITEAM(DosHeader, e_ss)
			MAKE_LIST_ITEAM(DosHeader, e_sp)
			MAKE_LIST_ITEAM(DosHeader, e_csum)
			MAKE_LIST_ITEAM(DosHeader, e_ip)
			MAKE_LIST_ITEAM(DosHeader, e_cs)
			MAKE_LIST_ITEAM(DosHeader, e_lfarlc)
			MAKE_LIST_ITEAM(DosHeader, e_ovno)
			MAKE_LIST_ITEAMS(DosHeader, e_res)
			MAKE_LIST_ITEAM(DosHeader, e_oemid)
			MAKE_LIST_ITEAM(DosHeader, e_oeminfo)
			MAKE_LIST_ITEAMS(DosHeader, e_res2)
			MAKE_LIST_ITEAM(DosHeader, e_lfanew)
		END_MAKE_LIST()
	}
	break;
	case ID_IMAGE_NT_HEADERS:
	{
		auto& NTHeader = m_peStruct.NT_HEADERS.header;
		BEGIN_MAKE_LIST(m_peStruct.NT_HEADERS.offset)
			MAKE_LIST_ITEAM(NTHeader, Signature)
			END_MAKE_LIST()
	}
	break;
	case ID_IMAGE_FILE_HEADER:
	{
		auto& FileHeader = m_peStruct.NT_HEADERS.header.FileHeader;
		BEGIN_MAKE_LIST(m_peStruct.NT_HEADERS.offset + 4)
			MAKE_LIST_ITEAM(FileHeader, Machine)
			MAKE_LIST_ITEAM(FileHeader, NumberOfSections)
			MAKE_LIST_ITEAM(FileHeader, TimeDateStamp)
			MAKE_LIST_ITEAM(FileHeader, PointerToSymbolTable)
			MAKE_LIST_ITEAM(FileHeader, NumberOfSymbols)
			MAKE_LIST_ITEAM(FileHeader, SizeOfOptionalHeader)
			MAKE_LIST_ITEAM(FileHeader, Characteristics)
		END_MAKE_LIST()
	}
	break;
	case ID_IMAGE_OPTIONAL_HEADER:
	{
		auto& OptionalHeader = m_peStruct.NT_HEADERS.header.OptionalHeader;
		BEGIN_MAKE_LIST(m_peStruct.NT_HEADERS.offset + 4 + sizeof(IMAGE_FILE_HEADER))
			MAKE_LIST_ITEAM(OptionalHeader, Magic)
			MAKE_LIST_ITEAM(OptionalHeader, MajorLinkerVersion)
			MAKE_LIST_ITEAM(OptionalHeader, MinorLinkerVersion)
			MAKE_LIST_ITEAM(OptionalHeader, SizeOfCode)
			MAKE_LIST_ITEAM(OptionalHeader, SizeOfInitializedData)
			MAKE_LIST_ITEAM(OptionalHeader, SizeOfUninitializedData)
			MAKE_LIST_ITEAM(OptionalHeader, AddressOfEntryPoint)
			MAKE_LIST_ITEAM(OptionalHeader, BaseOfCode)
			MAKE_LIST_ITEAM(OptionalHeader, BaseOfData)
			MAKE_LIST_ITEAM(OptionalHeader, ImageBase)
			MAKE_LIST_ITEAM(OptionalHeader, SectionAlignment)
			MAKE_LIST_ITEAM(OptionalHeader, FileAlignment)
			MAKE_LIST_ITEAM(OptionalHeader, MajorOperatingSystemVersion)
			MAKE_LIST_ITEAM(OptionalHeader, MinorOperatingSystemVersion)
			MAKE_LIST_ITEAM(OptionalHeader, MajorImageVersion)
			MAKE_LIST_ITEAM(OptionalHeader, MinorImageVersion)
			MAKE_LIST_ITEAM(OptionalHeader, MajorSubsystemVersion)
			MAKE_LIST_ITEAM(OptionalHeader, MinorSubsystemVersion)
			MAKE_LIST_ITEAM(OptionalHeader, Win32VersionValue)
			MAKE_LIST_ITEAM(OptionalHeader, SizeOfImage)
			MAKE_LIST_ITEAM(OptionalHeader, SizeOfHeaders)
			MAKE_LIST_ITEAM(OptionalHeader, CheckSum)
			MAKE_LIST_ITEAM(OptionalHeader, Subsystem)
			MAKE_LIST_ITEAM(OptionalHeader, DllCharacteristics)
			MAKE_LIST_ITEAM(OptionalHeader, SizeOfStackReserve)
			MAKE_LIST_ITEAM(OptionalHeader, SizeOfStackCommit)
			MAKE_LIST_ITEAM(OptionalHeader, SizeOfHeapReserve)
			MAKE_LIST_ITEAM(OptionalHeader, SizeOfHeapCommit)
			MAKE_LIST_ITEAM(OptionalHeader, LoaderFlags)
			MAKE_LIST_ITEAM(OptionalHeader, NumberOfRvaAndSizes)
			for (size_t i = 0; i < OptionalHeader.NumberOfRvaAndSizes; i++)
			{
				szFormat.Format(TEXT("DataDirectory[%d]"), i);
				m_ListCtrl.InsertItem(nRow++, szFormat);
				MAKE_LIST_ITEAM(OptionalHeader.DataDirectory[i], VirtualAddress)
				MAKE_LIST_ITEAM(OptionalHeader.DataDirectory[i], Size)
			}
		END_MAKE_LIST()
	}
	break;
	case ID_IMAGE_SECTION_HEADER:
	{
		auto SectionHeader = m_peStruct.SECTION_HEADERS.headers;
		BEGIN_MAKE_LIST(m_peStruct.SECTION_HEADERS.offset)
			for (size_t i = 0; i < m_peStruct.NT_HEADERS.header.FileHeader.NumberOfSections; i++)
			{
				MAKE_LIST_SZITEAM(SectionHeader[i], Name)
					MAKE_LIST_ITEAM(SectionHeader[i], Misc)
					MAKE_LIST_ITEAM(SectionHeader[i], VirtualAddress)
					MAKE_LIST_ITEAM(SectionHeader[i], SizeOfRawData)
					MAKE_LIST_ITEAM(SectionHeader[i], PointerToRawData)
					MAKE_LIST_ITEAM(SectionHeader[i], PointerToRelocations)
					MAKE_LIST_ITEAM(SectionHeader[i], PointerToLinenumbers)
					MAKE_LIST_ITEAM(SectionHeader[i], NumberOfRelocations)
					MAKE_LIST_ITEAM(SectionHeader[i], NumberOfLinenumbers)
					MAKE_LIST_ITEAM(SectionHeader[i], Characteristics)
				MAKE_LIST_NEWLINE()
			}
		END_MAKE_LIST()
	}
	break;
	case ID_IMAGE_DIRECTORY_ENTRY_IMPORT:
	{
		DWORD dwFOA = RVAtoFOA(m_peStruct.NT_HEADERS.header.OptionalHeader.DataDirectory[1].VirtualAddress);
		ASSERT(dwFOA);

		auto pIMPORT = (IMAGE_IMPORT_DESCRIPTOR*)malloc(sizeof(IMAGE_IMPORT_DESCRIPTOR));
		ASSERT(pIMPORT);
		fstream ifs;
		CString szFormat;
		DWORD nRow = 0;
		char ch;

		ifs.open(m_szFilePath, ios::out | ios::in | ios::binary);

		while (true)
		{
			ifs.seekg(dwFOA);
			ifs.read((char*)pIMPORT, sizeof(IMAGE_IMPORT_DESCRIPTOR));
			dwFOA += sizeof(IMAGE_IMPORT_DESCRIPTOR);
			if (!pIMPORT->Name)
			{
				break;
			}

			ifs.seekg(RVAtoFOA(pIMPORT->Name));
			szFormat.Empty();
			do
			{
				ifs >> ch;
				szFormat += ch;
			} while (ch != NULL);
			m_ListCtrl.InsertItem(nRow++, szFormat);

			DWORD funFOA = RVAtoFOA(pIMPORT->OriginalFirstThunk);
			while (true)
			{
				DWORD Thunkaddr;
				ifs.seekg(funFOA);
				ifs.read((char*)&Thunkaddr, sizeof(DWORD));
				funFOA += sizeof(DWORD);
				if (!Thunkaddr)
				{
					break;
				}

				if (Thunkaddr >> 31)
				{
					m_ListCtrl.InsertItem(nRow, TEXT("---"));
					m_ListCtrl.SetItemText(nRow, 1, TEXT("---"));
					szFormat.Format(TEXT("%04X"), Thunkaddr & 0x0000FFFF);
					m_ListCtrl.SetItemText(nRow++, 2, szFormat);
					continue;
				}

				WORD hint;
				ifs.seekg(RVAtoFOA(Thunkaddr));
				ifs.read((char*)&hint, sizeof(WORD));
				szFormat.Empty();
				do
				{
					ifs >> ch;
					szFormat += ch;
				} while (ch != NULL);
				m_ListCtrl.InsertItem(nRow, TEXT("---"));
				m_ListCtrl.SetItemText(nRow, 1, szFormat);
				szFormat.Format(TEXT("%04X"), hint);
				m_ListCtrl.SetItemText(nRow++, 2, szFormat);
			}
			MAKE_LIST_NEWLINE()
		}
		free(pIMPORT);
		ifs.close();
	}
	break;
	}

	//循环为每列自动调整列宽
	m_ListCtrl.SetRedraw(FALSE);
	CHeaderCtrl* pHeader = m_ListCtrl.GetHeaderCtrl();
	int nColumnCount = pHeader->GetItemCount();
	for (int i = 0; i < nColumnCount; i++)
	{
		m_ListCtrl.SetColumnWidth(i, LVSCW_AUTOSIZE);
		int nColumnWidth = m_ListCtrl.GetColumnWidth(i);
		m_ListCtrl.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
		int nHeaderWidth = m_ListCtrl.GetColumnWidth(i);
		m_ListCtrl.SetColumnWidth(i, max(nColumnWidth, nHeaderWidth) + 5);
	}
	m_ListCtrl.SetRedraw(TRUE);

}

void CPEParsingDlg::UpdatePEInfo()
{
	// 分析PE数据
	if (auto pBuff = m_peStruct.SECTION_HEADERS.headers)
	{
		free(pBuff);
	}

	using namespace std;
	fstream ifs;
	ifs.open(m_szFilePath, ios::out | ios::in | ios::binary);

	// 获取 IMAGE_DOS_HEADER 信息
	auto& pDOS_HEADER = m_peStruct.DOS_HEADER;
	pDOS_HEADER.offset = 0;
	ifs.read((char*)&pDOS_HEADER.header, sizeof(pDOS_HEADER.header));

	// 获取 IMAGE_NT_HEADERS 信息
	auto& pNT_HEADRS = m_peStruct.NT_HEADERS;
	pNT_HEADRS.offset = pDOS_HEADER.header.e_lfanew;
	ifs.seekg(pNT_HEADRS.offset);
	ifs.read((char*)&pNT_HEADRS.header, sizeof(pNT_HEADRS.header));

	// 获取 IMAGE_SECTION_HEADER 信息
	auto& pSECTION_HEADERS = m_peStruct.SECTION_HEADERS;
	pSECTION_HEADERS.offset = ifs.tellg();
	DWORD dwReadSize = sizeof(IMAGE_SECTION_HEADER) * pNT_HEADRS.header.FileHeader.NumberOfSections;
	pSECTION_HEADERS.headers = (IMAGE_SECTION_HEADER*)malloc(dwReadSize);
	ifs.read((char*)pSECTION_HEADERS.headers, dwReadSize);

	ifs.close();
}

DWORD CPEParsingDlg::RVAtoFOA(DWORD rva)
{
	for (int i = 0; i < m_peStruct.NT_HEADERS.header.FileHeader.NumberOfSections; i++)
	{
		auto pStruct = &m_peStruct.SECTION_HEADERS.headers[i];
		if (pStruct->VirtualAddress + pStruct->SizeOfRawData > rva)
		{
			return rva - pStruct->VirtualAddress + pStruct->PointerToRawData;
		}
	}
	return 0;
}

void CPEParsingDlg::OnBnClickedMfcbutton3()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (m_szFilePath.IsEmpty())
	{
		AfxMessageBox(TEXT("未选择执行文件！"));
		return;
	}

	// 配置树控件样式
	DWORD dwStyles = m_TreeCtrl.GetStyle();
	dwStyles |= TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS;
	m_TreeCtrl.ModifyStyle(0, dwStyles);

	// 添加树控件信息
	m_TreeCtrl.DeleteAllItems();
	m_ListCtrl.DeleteAllItems();

	HTREEITEM hTree;
	int offsetName = m_szFilePath.GetLength() - m_szFilePath.ReverseFind('\\') - 1;
	auto TreeRoot = m_TreeCtrl.InsertItem(m_szFilePath.Right(offsetName));

	hTree = m_TreeCtrl.InsertItem(TEXT("IMAGE_DOS_HEADER"), TreeRoot);
	m_TreeCtrl.SetItemData(hTree, ID_IMAGE_DOS_HEADER);

	hTree = m_TreeCtrl.InsertItem(TEXT("IMAGE_NT_HEADERS"), TreeRoot);
	m_TreeCtrl.SetItemData(hTree, ID_IMAGE_NT_HEADERS);

	auto NT_HEADERS = hTree;
	hTree = m_TreeCtrl.InsertItem(TEXT("IMAGE_FILE_HEADER"), NT_HEADERS);
	m_TreeCtrl.SetItemData(hTree, ID_IMAGE_FILE_HEADER);

	hTree = m_TreeCtrl.InsertItem(TEXT("IMAGE_OPTIONAL_HEADER"), NT_HEADERS);
	m_TreeCtrl.SetItemData(hTree, ID_IMAGE_OPTIONAL_HEADER);

	auto OPTIONAL_HEADER = hTree;

	hTree = m_TreeCtrl.InsertItem(TEXT("IMAGE_DIRECTORY_ENTRY_EXPORT"), OPTIONAL_HEADER);
	m_TreeCtrl.SetItemData(hTree, ID_IMAGE_DIRECTORY_ENTRY_EXPORT);

	hTree = m_TreeCtrl.InsertItem(TEXT("IMAGE_DIRECTORY_ENTRY_IMPORT"), OPTIONAL_HEADER);
	m_TreeCtrl.SetItemData(hTree, ID_IMAGE_DIRECTORY_ENTRY_IMPORT);

	hTree = m_TreeCtrl.InsertItem(TEXT("IMAGE_SECTION_HEADER"), TreeRoot);
	m_TreeCtrl.SetItemData(hTree, ID_IMAGE_SECTION_HEADER);

	UpdatePEInfo();
}

void CPEParsingDlg::OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	UpdateListView(pNMTreeView->itemNew.lParam);

	*pResult = 0;
}


void CPEParsingDlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (pNMItemActivate->iItem == -1 || pNMItemActivate->iSubItem != 2)
	{
		return;
	}

	if (pNMItemActivate->lParam == ID_IMAGE_DIRECTORY_ENTRY_IMPORT)
	{
		return;
	}

	CRect rect;
	m_ListCtrl.GetSubItemRect(pNMItemActivate->iItem, pNMItemActivate->iSubItem, LVIR_LABEL, rect);
	m_ListCtrl.ClientToScreen(rect);
	CString szShow = m_ListCtrl.GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem);
	CString szOffset = m_ListCtrl.GetItemText(pNMItemActivate->iItem, 0);

	CEditDataDlg dlg;
	dlg.setInput(szShow, rect);

	if (dlg.DoModal() == IDOK)
	{
		switch (m_ListCtrl.GetItemData(pNMItemActivate->iItem))
		{
		case VALUE_TYPE_NUM:
		{
			using namespace std;

			fstream ofs;
			TCHAR* end;
			ofs.open(m_szFilePath, ios::out | ios::in | ios::binary);
			DWORD dwOffset = _tcstol(szOffset.GetBuffer(), &end, 16);
			DWORD dwWrite = _tcstol(dlg.getInput().GetBuffer(), &end, 16);
			ofs.seekp(dwOffset);
			ofs.write((char*)&dwWrite, szShow.GetLength() / 2);
			ofs.close();
		}
		break;
		case NULL:
		case VALUE_TYPE_STR:
		case VALUE_TYPE_UNUSEFUL:
			AfxMessageBox(TEXT("能别乱改吗？你搁这测BUG呢？"));
			return;
		}
		UpdatePEInfo();
		UpdateListView(m_TreeCtrl.GetItemData(m_TreeCtrl.GetSelectedItem()));
	}

	*pResult = 0;
}


void CPEParsingDlg::OnBnClickedMfcbutton4()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_szFilePath.IsEmpty())
	{
		AfxMessageBox(TEXT("尚选择执行文件"));
		return;
	}

	CFileDialog dlg(TRUE);
	if (dlg.DoModal() != IDOK)
	{
		return;
	}

	CString szSouce = dlg.GetPathName();

	using namespace std;
	fstream souStream;
	fstream desStream;

	souStream.open(szSouce, ios::out | ios::in | ios::binary);
	desStream.open(m_szFilePath, ios::out | ios::in | ios::binary);

	auto pHeader = &m_peStruct.NT_HEADERS.header;
	DWORD FileAlignment = pHeader->OptionalHeader.FileAlignment;
	DWORD SectionAlignment = pHeader->OptionalHeader.SectionAlignment;

	desStream.seekp(0, ios::end);
	DWORD oldOffest = desStream.tellp();

	BYTE temp;
	while (!souStream.eof())
	{
		souStream >> temp;
		desStream << temp;
	}

	DWORD newOffest = desStream.tellp();
	DWORD index = m_peStruct.NT_HEADERS.header.FileHeader.NumberOfSections;
	DWORD newSize = newOffest - oldOffest + m_peStruct.SECTION_HEADERS.headers[index - 1].SizeOfRawData;
	DWORD RawDataSize;
	if (newSize % FileAlignment)
	{
		RawDataSize = (newSize / FileAlignment + 1) * FileAlignment;
	}
	else
	{
		RawDataSize = newSize;
	}

	DWORD offset = (DWORD)&m_peStruct.SECTION_HEADERS.headers[index - 1].PointerToRawData - (DWORD)m_peStruct.SECTION_HEADERS.headers + m_peStruct.SECTION_HEADERS.offset;
	souStream.seekp(offset);
	souStream << RawDataSize;

	offset = (DWORD)&m_peStruct.SECTION_HEADERS.headers[index - 1].Misc - (DWORD)m_peStruct.SECTION_HEADERS.headers + m_peStruct.SECTION_HEADERS.offset;
	souStream.seekp(offset);
	souStream << newSize;

	offset = (DWORD)&m_peStruct.NT_HEADERS.header.OptionalHeader.SizeOfImage - (DWORD)&m_peStruct.NT_HEADERS.header + m_peStruct.NT_HEADERS.offset;
	souStream.seekp(offset);
	souStream << (m_peStruct.SECTION_HEADERS.headers[index - 1].VirtualAddress + RawDataSize);

	souStream.close();
	desStream.close();

	AfxMessageBox(TEXT("写入完成"));
	UpdatePEInfo();
	if (auto pIteam = m_TreeCtrl.GetSelectedItem())
	{
		UpdateListView(m_TreeCtrl.GetItemData(pIteam));
	}
}
