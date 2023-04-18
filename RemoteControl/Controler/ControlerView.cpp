
// ControlerView.cpp: CControlerView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Controler.h"
#endif

#include "ControlerDoc.h"
#include "ControlerView.h"
#include "FileManageDlg.h"
#include "MFCConsoleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CControlerView

IMPLEMENT_DYNCREATE(CControlerView, CView)

BEGIN_MESSAGE_MAP(CControlerView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_32771, &CControlerView::ConnectServer)
	ON_COMMAND(ID_32772, &CControlerView::GraphicTrans)
	ON_COMMAND(ID_32773, &CControlerView::KeyBoardInput)
	ON_COMMAND(ID_32774, &CControlerView::MouseInput)
	ON_COMMAND(ID_32775, &CControlerView::FileControl)
	ON_COMMAND(ID_32776, &CControlerView::ConsoleContorl)
END_MESSAGE_MAP()

// CControlerView 构造/析构

CControlerView::CControlerView() noexcept
{
	// TODO: 在此处添加构造代码
	m_bGraphicTrans = false;
	m_bMouse = false;
	m_bKeyBoard = false;

	m_TCPInfo = m_SocketTool.InitTCPSock("192.168.124.25", "8848", false);
	m_UDPInfo = m_SocketTool.InitUDPSock("", "8849", true);
}

CControlerView::~CControlerView()
{
}

BOOL CControlerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CControlerView 绘图

void CControlerView::OnDraw(CDC* pDC)
{
	CControlerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if (m_bGraphicTrans)
	{
		CDC dcMem;
		CBitmap bmpMem;

		dcMem.CreateCompatibleDC(pDC);
		bmpMem.CreateCompatibleBitmap(pDC, m_SerScreenRect.Width(), m_SerScreenRect.Height());

		DWORD nBitsBufSize = m_SerScreenRect.Width() * m_SerScreenRect.Height() * 4;

		dcMem.SelectObject(&bmpMem);
		bmpMem.SetBitmapBits(nBitsBufSize, m_pGraphicBuf);

		CRect ClientRect;
		GetWindowRect(&ClientRect);
		pDC->StretchBlt(0, 0, ClientRect.Width(), ClientRect.Height(), &dcMem, 0, 0, m_SerScreenRect.Width(), m_SerScreenRect.Height(), SRCCOPY);	// 画面好特么糊
		//pDC->BitBlt(0, 0, m_SerScreenRect.Width(), m_SerScreenRect.Height(), &dcMem, 0, 0, SRCCOPY);												// 原画，但是窗口大小没适配

		dcMem.DeleteDC();
		bmpMem.DeleteObject();
	}

	// TODO: 在此处为本机数据添加绘制代码

}


// CControlerView 打印

BOOL CControlerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CControlerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CControlerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CControlerView 诊断

#ifdef _DEBUG
void CControlerView::AssertValid() const
{
	CView::AssertValid();
}

void CControlerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CControlerDoc* CControlerView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CControlerDoc)));
	return (CControlerDoc*)m_pDocument;
}
#endif //_DEBUG

DWORD WINAPI CControlerView::MainTCPMessage(_In_ LPVOID lpParameter)
{
	auto pView = reinterpret_cast<CControlerView*>(lpParameter);
	int iResult, nlength;
	char SockBuffer[0x400];
	eSockType SockType;
	while (true)
	{
		iResult = recv(pView->m_TCPInfo.m_Socket, SockBuffer, 0xFF, MSG_PEEK);
		nlength = reinterpret_cast<CPackageHeader*>(SockBuffer)->m_Length;
		SockType = reinterpret_cast<CPackageHeader*>(SockBuffer)->m_PackType;
		if ((iResult >= nlength) && (SockType == eSockType::S2C_SCREENDATA))		// 有些消息在自己那边处理
		{
			iResult = recv(pView->m_TCPInfo.m_Socket, SockBuffer, nlength, 0);
			switch (SockType)
			{
			case eSockType::S2C_SCREENDATA:
				RECT& temp = reinterpret_cast<CScreenInfo*>(SockBuffer)->m_Rect;
				pView->m_SerScreenRect.SetRect(0, 0, temp.right, temp.bottom);
				pView->m_bGraphicTrans = true;
				::CreateThread(NULL, 0, GraphicMessage, lpParameter, 0, NULL);
				break;
			}
		}
		if (iResult <= 0)
		{
			AfxMessageBox("对方已断开链接");
			shutdown(pView->m_TCPInfo.m_Socket, SD_SEND);
			closesocket(pView->m_TCPInfo.m_Socket);
			break;
		}
	}

	return 0;
}

DWORD WINAPI CControlerView::GraphicMessage(_In_ LPVOID lpParameter)
{
	auto pView = reinterpret_cast<CControlerView*>(lpParameter);

	BYTE pheader[8];
	pView->m_pGraphicBuf = (PBYTE)malloc(pView->m_SerScreenRect.Width() * pView->m_SerScreenRect.Height() * 4);

	while (pView->m_bGraphicTrans)
	{
		recvfrom(pView->m_UDPInfo.m_Socket, (char*)pheader, 8, MSG_PEEK, NULL, NULL);
		int nLine = pheader[3] * 100 + pheader[7];
		recvfrom(pView->m_UDPInfo.m_Socket, (char*)pView->m_pGraphicBuf + nLine * 1200, 1200, 0, NULL, NULL);

		pView->InvalidateRect(NULL, FALSE);
	}

	free(pView->m_pGraphicBuf);

	return 0;
}

CPoint CControlerView::getSendCursorPoint()
{
	CPoint	CurPos;
	CRect	ClientRect;
	::GetCursorPos(&CurPos);
	ScreenToClient(&CurPos);
	GetClientRect(&ClientRect);
	double RatioX = (double)m_SerScreenRect.Width() / ClientRect.Width();
	double RatioY = (double)m_SerScreenRect.Height() / ClientRect.Height();
	int mouse_x = CurPos.x * RatioX;
	int mouse_y = CurPos.y * RatioY;
	return CPoint(mouse_x, mouse_y);
}

// CControlerView 消息处理程序


void CControlerView::ConnectServer()
{
	// TODO: 在此添加命令处理程序代码
	connect(m_TCPInfo.m_Socket, m_TCPInfo.m_AddrInfo->ai_addr, m_TCPInfo.m_AddrInfo->ai_addrlen);
	::CreateThread(NULL, 0, MainTCPMessage, this, 0, NULL);
}


void CControlerView::GraphicTrans()
{
	// TODO: 在此添加命令处理程序代码
	CScreenDataRequest RequestPack(true);
	send(m_TCPInfo.m_Socket, (char*)&RequestPack, sizeof(CScreenDataRequest), 0);
}

void CControlerView::KeyBoardInput()
{
	// TODO: 在此添加命令处理程序代码
	m_bKeyBoard = !m_bKeyBoard;
}


void CControlerView::MouseInput()
{
	// TODO: 在此添加命令处理程序代码
	m_bMouse = !m_bMouse;
}


void CControlerView::FileControl()
{
	// TODO: 在此添加命令处理程序代码
	CFileManageDlg FileDlg;
	FileDlg.m_TCPInfo = m_TCPInfo;
	FileDlg.DoModal();
}


void CControlerView::ConsoleContorl()
{
	// TODO: 在此添加命令处理程序代码

	CMFCConsoleDlg ConsoleDlg;
	ConsoleDlg.m_TCPInfo = m_TCPInfo;
	ConsoleDlg.DoModal();
}


BOOL CControlerView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		if (m_bKeyBoard)
		{
			SendPackage<CKeyBoardRequest>(m_TCPInfo.m_Socket, pMsg->wParam);
		}
		break;
	case WM_RBUTTONDOWN:
		if (m_bMouse)
		{
			SendPackage<CMouseRequest>(m_TCPInfo.m_Socket, getSendCursorPoint(), CMouseRequest::MouseEvent::RBUTTONDOWN);
		}
		break;
	case WM_LBUTTONDOWN:
		if (m_bMouse)
		{
			SendPackage<CMouseRequest>(m_TCPInfo.m_Socket, getSendCursorPoint(), CMouseRequest::MouseEvent::LBUTTONDOWN);
		}
		break;
	case WM_LBUTTONDBLCLK:
		if (m_bMouse)
		{
			SendPackage<CMouseRequest>(m_TCPInfo.m_Socket, getSendCursorPoint(), CMouseRequest::MouseEvent::LBUTTONDBLCLK);
		}
		break;
	}


	return CView::PreTranslateMessage(pMsg);
}
