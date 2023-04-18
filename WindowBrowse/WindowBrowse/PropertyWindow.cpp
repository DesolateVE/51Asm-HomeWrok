// PropertyWindow.cpp: 实现文件
//

#include "pch.h"
#include "WindowBrowse.h"
#include "afxdialogex.h"
#include "PropertyWindow.h"


// CPropertyWindow 对话框

IMPLEMENT_DYNAMIC(CPropertyWindow, CDialogEx)

CPropertyWindow::CPropertyWindow(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROPERTY_WINDOW, pParent)
{

}

CPropertyWindow::~CPropertyWindow()
{
}

void CPropertyWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPropertyWindow, CDialogEx)
END_MESSAGE_MAP()


// CPropertyWindow 消息处理程序
