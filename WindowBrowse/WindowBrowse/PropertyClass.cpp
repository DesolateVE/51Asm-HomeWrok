// PropertyClass.cpp: 实现文件
//

#include "pch.h"
#include "WindowBrowse.h"
#include "afxdialogex.h"
#include "PropertyClass.h"


// CPropertyClass 对话框

IMPLEMENT_DYNAMIC(CPropertyClass, CDialogEx)

CPropertyClass::CPropertyClass(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROPERTY_CLASS, pParent)
{

}

CPropertyClass::~CPropertyClass()
{
}

void CPropertyClass::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPropertyClass, CDialogEx)
END_MESSAGE_MAP()


// CPropertyClass 消息处理程序
