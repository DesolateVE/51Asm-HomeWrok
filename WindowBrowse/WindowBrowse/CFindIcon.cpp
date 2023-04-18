// CFindIcon.cpp: 实现文件
//

#include "pch.h"
#include "WindowBrowse.h"
#include "CFindIcon.h"


// CFindIcon

IMPLEMENT_DYNAMIC(CFindIcon, CStatic)

CFindIcon::CFindIcon()
{

}

CFindIcon::~CFindIcon()
{
}


BEGIN_MESSAGE_MAP(CFindIcon, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()



// CFindIcon 消息处理程序




void CFindIcon::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	SetIcon(LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_SCAN)));
	::SetCursor(LoadCursor(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDC_CURSOR1)));
	SetCapture();

	CStatic::OnLButtonDown(nFlags, point);
}


void CFindIcon::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	SetIcon(LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_FROM)));
	ReleaseCapture();

	CStatic::OnLButtonUp(nFlags, point);
}


void CFindIcon::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值



	CStatic::OnMouseMove(nFlags, point);
}
