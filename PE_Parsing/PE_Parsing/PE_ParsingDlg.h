
// PE_ParsingDlg.h: 头文件
//

#pragma once

struct MY_IMAGE_DOS_HEADER
{
	IMAGE_DOS_HEADER header;
	DWORD offset;
};

struct MY_IMAGE_NT_HEADERS
{
	IMAGE_NT_HEADERS header;
	DWORD offset;
};

struct MY_IMAGE_SECTION_HEADER
{
	IMAGE_SECTION_HEADER* headers;
	DWORD offset;
};

struct MY_PE
{
	MY_IMAGE_DOS_HEADER			DOS_HEADER;
	MY_IMAGE_NT_HEADERS			NT_HEADERS;
	MY_IMAGE_SECTION_HEADER		SECTION_HEADERS; 
};

// CPEParsingDlg 对话框
class CPEParsingDlg : public CDialogEx
{
// 构造
public:
	CPEParsingDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PE_PARSING_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	CString m_szFilePath;
	CTreeCtrl m_TreeCtrl;
	CListCtrl m_ListCtrl;
	MY_PE m_peStruct;

	void UpdateListView(int nSelect);
	void UpdatePEInfo();
	DWORD RVAtoFOA(DWORD rva);

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();

public:
	afx_msg void OnBnClickedMfcbutton3();
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedMfcbutton4();
};
