// COverView.cpp: 实现文件
//

#include "pch.h"
#include "CAD.h"
#include "COverView.h"
#include "resource.h"

// COverView

IMPLEMENT_DYNCREATE(COverView, CTreeView)

COverView::COverView()
{

}

COverView::~COverView()
{
}

BEGIN_MESSAGE_MAP(COverView, CTreeView)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_CLICK, &COverView::OnNMClick)
	ON_NOTIFY_REFLECT(NM_RCLICK, &COverView::OnNMRClick)
	ON_COMMAND(ID_32779, &COverView::ShapeRotate)
	ON_COMMAND(ID_32780, &COverView::ShapeEnable)
	ON_COMMAND(ID_32777, &COverView::SelectShapePen)
	ON_COMMAND(ID_32778, &COverView::SelectShapeBrush)
END_MESSAGE_MAP()


#ifndef _DEBUG
CCADDoc* COverView::GetDocument() const
{
	return (CCADDoc*)m_pDocument;
}
#endif // _DEBUG

// COverView 诊断

#ifdef _DEBUG

CCADDoc* COverView::GetDocument() const
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCADDoc)));
	return (CCADDoc*)m_pDocument;
}

void COverView::AssertValid() const
{
	CTreeView::AssertValid();
}

#ifndef _WIN32_WCE
void COverView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif
#endif //_DEBUG


// COverView 消息处理程序


void COverView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	CCADDoc* pDoc = GetDocument();
	CTreeCtrl& tree = GetTreeCtrl();
	auto& m_listShape = pDoc->m_listShape;

	tree.DeleteAllItems();
	POSITION pos = m_listShape.GetHeadPosition();
	while (pos)
	{
		auto pShape = m_listShape.GetNext(pos);
		if (pShape->getEable())
		{
			if (pShape->IsKindOf(RUNTIME_CLASS(CLine)))
			{
				tree.InsertItem("直线", 0, 0);
			}
			else if (pShape->IsKindOf(RUNTIME_CLASS(CEllipse)))
			{
				tree.InsertItem("圆", 2, 2);
			}
			else if (pShape->IsKindOf(RUNTIME_CLASS(CRectangular)))
			{
				tree.InsertItem("矩形", 1, 1);
			}
			else
			{
				tree.InsertItem("五角星", 3, 3);
			}
		}
	}
}


int COverView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CTreeCtrl& tree = GetTreeCtrl();
	CCADDoc* pDoc = GetDocument();
	auto& m_ImgList = pDoc->m_ImgList;

	m_ImgList.Create(50, 50, ILC_COLOR32, 4, 1);
	m_ImgList.Add(AfxGetApp()->LoadIcon(ICN_LINE));
	m_ImgList.Add(AfxGetApp()->LoadIcon(ICN_RECT));
	m_ImgList.Add(AfxGetApp()->LoadIcon(ICN_CIRCLE));
	m_ImgList.Add(AfxGetApp()->LoadIcon(ICN_PENTAGRAM));

	tree.SetImageList(&m_ImgList, TVSIL_NORMAL);

	return 0;
}


void COverView::OnNMClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	auto& tree = GetTreeCtrl();
	auto& m_listShape = GetDocument()->m_listShape;

	CPoint ptClient;
	CShape* pShape = nullptr;
	GetCursorPos(&ptClient);
	ScreenToClient(&ptClient);
	HTREEITEM  hSel = tree.HitTest(ptClient);
	if (hSel != nullptr)
	{
		auto hRoot = tree.GetRootItem();
		POSITION pos = m_listShape.GetHeadPosition();
		int nTimes = 1;
		while (hRoot != hSel)
		{
			++nTimes;
			hRoot = tree.GetNextSiblingItem(hRoot);
		}
		for (int i = 0; i < nTimes; i++)
		{
			pShape = m_listShape.GetNext(pos);
			if (!pShape->getEable())
			{
				--i;
			}
		}

		GetDocument()->m_pCurrentShape = pShape;
		GetDocument()->m_eState = CCADDoc::SELECT;

		GetDocument()->UpdateAllViews(this);
	}

	*pResult = 0;
}


void COverView::OnNMRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	auto& tree = GetTreeCtrl();
	auto& m_listShape = GetDocument()->m_listShape;

	CShape* pShape = nullptr;
	CPoint ptClient;
	CPoint ptScreen;
	GetCursorPos(&ptScreen);
	ptClient = ptScreen;
	ScreenToClient(&ptClient);
	HTREEITEM  hSel = tree.HitTest(ptClient);
	if (hSel != nullptr)
	{
		auto hRoot = tree.GetRootItem();
		POSITION pos = m_listShape.GetHeadPosition();
		int nTimes = 1;
		while (hRoot != hSel)
		{
			++nTimes;
			hRoot = tree.GetNextSiblingItem(hRoot);
		}
		for (int i = 0; i < nTimes; i++)
		{
			pShape = m_listShape.GetNext(pos);
			if (!pShape->getEable())
			{
				--i;
			}
		}
		GetDocument()->m_pCurrentShape = pShape;
		GetDocument()->m_eState = CCADDoc::SELECT;

		GetDocument()->UpdateAllViews(this);

		CMenu mn;
		mn.LoadMenu(MN_RIGHT_KEY);
		CMenu* pSubMn = mn.GetSubMenu(0);
		pSubMn->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, ptScreen.x, ptScreen.y, this);
	}

	*pResult = 0;
}


void COverView::ShapeRotate()
{
	// 从Doc中拉取变量
	CCADDoc* pDoc = GetDocument();
	auto& m_pCurrentShape = pDoc->m_pCurrentShape;
	auto& m_pCurrentOperation = pDoc->m_pCurrentOperation;
	auto& m_eState = pDoc->m_eState;
	auto& m_stkToUndo = pDoc->m_stkToUndo;

	//记录操作
	m_pCurrentOperation = new CRotateOpt(m_pCurrentShape);
	m_pCurrentOperation->setRotateAngle(m_pCurrentOperation->getRotateAngle());
	m_stkToUndo.push(m_pCurrentOperation);

	//切换状态
	m_eState = CCADDoc::ROTATE;
}

void COverView::ShapeEnable()
{
	// 从Doc中拉取变量
	CCADDoc* pDoc = GetDocument();
	auto& m_pCurrentShape = pDoc->m_pCurrentShape;
	auto& m_pCurrentOperation = pDoc->m_pCurrentOperation;
	auto& m_eState = pDoc->m_eState;
	auto& m_stkToUndo = pDoc->m_stkToUndo;

	//记录操作
	m_pCurrentOperation = new CDelOpt(m_pCurrentShape);
	m_stkToUndo.push(m_pCurrentOperation);

	m_pCurrentShape->setEable(false);
	m_pCurrentShape = nullptr;
	m_eState = CCADDoc::FREE;

	GetDocument()->UpdateAllViews(NULL);
}

void COverView::SelectShapePen()
{
	// 从Doc中拉取变量
	CCADDoc* pDoc = GetDocument();
	auto& m_pCurrentShape = pDoc->m_pCurrentShape;
	auto& m_pCurrentOperation = pDoc->m_pCurrentOperation;
	auto& m_stkToUndo = pDoc->m_stkToUndo;

	CPenSelectDialog CDlg;
	CDlg.setLogPen(m_pCurrentShape->getLogPen());
	if (CDlg.DoModal() == IDOK)
	{
		//记录操作
		m_pCurrentOperation = new CUpdatePen(m_pCurrentShape);
		m_pCurrentOperation->setLogPen(m_pCurrentShape->getLogPen());
		m_stkToUndo.push(m_pCurrentOperation);

		m_pCurrentShape->setLogPen(CDlg.getLogPen());
	}

	GetDocument()->UpdateAllViews(this);
}

void COverView::SelectShapeBrush()
{
	// 从Doc中拉取变量
	CCADDoc* pDoc = GetDocument();
	auto& m_pCurrentShape = pDoc->m_pCurrentShape;
	auto& m_pCurrentOperation = pDoc->m_pCurrentOperation;
	auto& m_stkToUndo = pDoc->m_stkToUndo;


	CBrushSelectDialog CDlg;
	CDlg.setLogBrush(m_pCurrentShape->getLogBrush());
	if (CDlg.DoModal() == IDOK)
	{
		//记录操作
		m_pCurrentOperation = new CUpdateBrush(m_pCurrentShape);
		m_pCurrentOperation->setLogBrush(m_pCurrentShape->getLogBrush());
		m_stkToUndo.push(m_pCurrentOperation);

		m_pCurrentShape->setLogBrush(CDlg.getLogBrush());
	}

	GetDocument()->UpdateAllViews(this);
}
