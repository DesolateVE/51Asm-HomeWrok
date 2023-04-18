// PropertyProcess.cpp: 实现文件
//

#include "pch.h"
#include "WindowBrowse.h"
#include "afxdialogex.h"
#include "PropertyProcess.h"


// CPropertyProcess 对话框

IMPLEMENT_DYNAMIC(CPropertyProcess, CDialogEx)

CPropertyProcess::CPropertyProcess(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROPERTY_PROCESS, pParent)
{

}

CPropertyProcess::~CPropertyProcess()
{
}

void CPropertyProcess::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPropertyProcess, CDialogEx)
END_MESSAGE_MAP()


// CPropertyProcess 消息处理程序
