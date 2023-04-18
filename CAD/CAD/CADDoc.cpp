
// CADDoc.cpp: CCADDoc 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "CAD.h"
#endif

#include "CADDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCADDoc

IMPLEMENT_DYNCREATE(CCADDoc, CDocument)

BEGIN_MESSAGE_MAP(CCADDoc, CDocument)
END_MESSAGE_MAP()


// CCADDoc 构造/析构

CCADDoc::CCADDoc() noexcept
{
	//当前绘制置空
	m_pCurrentShape = nullptr;

	//默认图形设置
	m_pShapeFactory = new CLineFactory;

	//默认画刷配置
	m_logBrush.lbColor = RGB(190, 190, 190);
	m_logBrush.lbStyle = BS_HOLLOW;
	m_logBrush.lbHatch = NULL;			//If lbStyle is BS_SOLID or BS_HOLLOW, lbHatch is ignored.

	//默认画笔配置
	m_logPen.lopnColor = RGB(0, 0, 0);
	m_logPen.lopnStyle = PS_SOLID;
	m_logPen.lopnWidth = POINT{ _SETPENWIDTH(5), NULL };	//The y member in the POINT structure for lopnWidth is not used.

	//当前状态为空闲
	m_eState = FREE;
}

CCADDoc::~CCADDoc()
{
}

BOOL CCADDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// 重置状态
	m_pCurrentShape = nullptr;
	m_pCurrentOperation = nullptr;
	m_eState = FREE;

	// 清空操作记录
	while (!m_stkToUndo.empty())
	{
		delete m_stkToUndo.top();
		m_stkToUndo.pop();
	}
	while (!m_stkToRedo.empty())
	{
		delete m_stkToRedo.top();
		m_stkToRedo.pop();
	}

	// 清空图形并释放内存
	POSITION pos = m_listShape.GetHeadPosition();
	while (pos)
	{
		auto pShape = m_listShape.GetNext(pos);
		delete pShape;
	}
	m_listShape.RemoveAll();

	return TRUE;
}




// CCADDoc 序列化

void CCADDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// 把标记的图形数量记录下来
		int nCnt = NULL;
		POSITION pos = m_listShape.GetHeadPosition();
		while (pos)
		{
			auto pShape = m_listShape.GetNext(pos);
			if (!pShape->getEable())
			{
				++nCnt;
			}
		}

		// 写入数据
		ar << m_listShape.GetCount() - nCnt;
		pos = m_listShape.GetHeadPosition();
		while (pos)
		{
			auto pShape = m_listShape.GetNext(pos);
			if (pShape->getEable())
			{
				ar << pShape;
			}
		}
	}
	else
	{
		// 重置状态
		m_pCurrentShape = nullptr;
		m_pCurrentOperation = nullptr;
		m_eState = FREE;

		// 清空操作记录
		while (!m_stkToUndo.empty())
		{
			delete m_stkToUndo.top();
			m_stkToUndo.pop();
		}
		while (!m_stkToRedo.empty())
		{
			delete m_stkToRedo.top();
			m_stkToRedo.pop();
		}

		// 清空图形并释放内存
		POSITION pos = m_listShape.GetHeadPosition();
		while (pos)
		{
			auto pShape = m_listShape.GetNext(pos);
			delete pShape;
		}
		m_listShape.RemoveAll();

		// 载入数据
		int nCnt = NULL;
		ar >> nCnt;

		CShape* pShape = nullptr;
		for (size_t i = 0; i < nCnt; i++)
		{
			ar >> pShape;
			m_listShape.AddTail(pShape);
		}
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CCADDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CCADDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CCADDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl* pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CCADDoc 诊断

#ifdef _DEBUG
void CCADDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCADDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CCADDoc 命令
