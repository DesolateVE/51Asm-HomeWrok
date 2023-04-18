// PropertyStyle.cpp: 实现文件
//

#include "pch.h"
#include "WindowBrowse.h"
#include "afxdialogex.h"
#include "PropertyStyle.h"


// CPropertyStyle 对话框

IMPLEMENT_DYNAMIC(CPropertyStyle, CDialogEx)

CPropertyStyle::CPropertyStyle(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROPERTY_STYLE, pParent)
{

}

CPropertyStyle::~CPropertyStyle()
{
}

void CPropertyStyle::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPropertyStyle, CDialogEx)
END_MESSAGE_MAP()


// CPropertyStyle 消息处理程序
