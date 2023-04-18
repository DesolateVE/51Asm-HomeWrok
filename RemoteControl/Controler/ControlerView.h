
// ControlerView.h: CControlerView 类的接口
//

#pragma once
#include "SocketTool.h"
#include "CPackage.h"

class CControlerView : public CView
{
protected: // 仅从序列化创建
	CControlerView() noexcept;
	DECLARE_DYNCREATE(CControlerView)

	// 特性
public:
	CControlerDoc* GetDocument() const;

	// 操作
public:
	static DWORD WINAPI MainTCPMessage(_In_ LPVOID lpParameter);
	static DWORD WINAPI GraphicMessage(_In_ LPVOID lpParameter);

	// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	// 实现
public:
	virtual ~CControlerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	CSocketTool			m_SocketTool;
	CSocketInfo			m_TCPInfo;
	CSocketInfo			m_UDPInfo;
	CRect				m_SerScreenRect;
	PBYTE				m_pGraphicBuf;

public:
	bool		m_bGraphicTrans;
	bool		m_bKeyBoard;
	bool		m_bMouse;

public:
	CPoint getSendCursorPoint();

	// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void ConnectServer();
	afx_msg void GraphicTrans();
	afx_msg void KeyBoardInput();
	afx_msg void MouseInput();
	afx_msg void FileControl();
	afx_msg void ConsoleContorl();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

#ifndef _DEBUG  // ControlerView.cpp 中的调试版本
inline CControlerDoc* CControlerView::GetDocument() const
{
	return reinterpret_cast<CControlerDoc*>(m_pDocument);
}
#endif

