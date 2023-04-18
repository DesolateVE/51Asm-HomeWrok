
// CADView.cpp: CCADView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "CAD.h"
#endif

#include "CADDoc.h"
#include "CADView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCADView

IMPLEMENT_DYNCREATE(CCADView, CView)

BEGIN_MESSAGE_MAP(CCADView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_32771, &CCADView::SelectLineDraw)
	ON_COMMAND(ID_32772, &CCADView::SelectRectDraw)
	ON_COMMAND(ID_32773, &CCADView::SelectEllipseDraw)
	ON_COMMAND(ID_32774, &CCADView::SelectPentagramDraw)
	ON_COMMAND(ID_32775, &CCADView::PenSetting)
	ON_COMMAND(ID_32776, &CCADView::BrushSetting)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_32779, &CCADView::ShapeRotate)
	ON_COMMAND(ID_32780, &CCADView::ShapeEnable)
	ON_COMMAND(ID_32777, &CCADView::SelectShapePen)
	ON_COMMAND(ID_32778, &CCADView::SelectShapeBrush)
	ON_COMMAND(ACCEL_REDO, &CCADView::OnRedo)
	ON_COMMAND(ACCEL_UNDO, &CCADView::OnUndo)
END_MESSAGE_MAP()

// CCADView 构造/析构

CCADView::CCADView() noexcept
{

}

CCADView::~CCADView()
{
}

BOOL CCADView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CCADView 绘图

void CCADView::OnDraw(CDC* pDC)
{
	// 从Doc中拉取变量
	CCADDoc* pDoc = GetDocument();
	auto& m_pCurrentShape = pDoc->m_pCurrentShape;
	auto& m_listShape = pDoc->m_listShape;
	auto& m_eState = pDoc->m_eState;

	// TODO: 在此处为本机数据添加绘制代码

	//创建内存DC
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);

	CRect rcClient;
	GetClientRect(&rcClient);

	//创建内存位图
	CBitmap bmpMem;
	bmpMem.CreateCompatibleBitmap(pDC, rcClient.Width(), rcClient.Height());

	//选入位图
	dcMem.SelectObject(&bmpMem);
	dcMem.FillSolidRect(&rcClient, RGB(255, 255, 255));

	//绘制保存的所有的图形
	POSITION pos = m_listShape.GetHeadPosition();
	while (pos)
	{
		auto pShape = m_listShape.GetNext(pos);
		if (pShape->getEable())
		{
			pShape->DrawShape(&dcMem);
		}
	}

	//空指针直接跳过当前绘制
	if (m_pCurrentShape != nullptr)
	{
		//状态判断
		switch (m_eState)
		{
		case CCADDoc::DRAW:
			//绘制当前图形
			m_pCurrentShape->DrawShape(&dcMem);
			m_pCurrentShape->DrawSelectLine(&dcMem);
			break;
		case CCADDoc::SELECT:
			//绘制选中虚线
			m_pCurrentShape->DrawSelectLine(&dcMem);
			break;
		case CCADDoc::ROTATE:
			//显示当前角度
		{
			CString szRotateInfo;
			szRotateInfo.Format("当前旋转角度为：%.2lF 度", m_pCurrentShape->getRotateAngle());
			dcMem.TextOut(200, 50, szRotateInfo);
		}
		break;
		default:
			break;
		}
	}

	//拷贝到客户区 Bitblt
	pDC->BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &dcMem, 0, 0, SRCCOPY);

	//绘图完成后的清理
	dcMem.DeleteDC();		//删除DC
	bmpMem.DeleteObject();  //删除位图
}


// CCADView 打印

BOOL CCADView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CCADView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CCADView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CCADView 诊断

#ifdef _DEBUG
void CCADView::AssertValid() const
{
	CView::AssertValid();
}

void CCADView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCADDoc* CCADView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCADDoc)));
	return (CCADDoc*)m_pDocument;
}
#endif //_DEBUG

// CCADView 消息处理程序

void CCADView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// 从Doc中拉取变量
	CCADDoc* pDoc = GetDocument();
	auto& m_pCurrentShape = pDoc->m_pCurrentShape;
	auto& m_pCurrentOperation = pDoc->m_pCurrentOperation;
	auto& m_pShapeFactory = pDoc->m_pShapeFactory;
	auto& m_listShape = pDoc->m_listShape;
	auto& m_logBrush = pDoc->m_logBrush;
	auto& m_logPen = pDoc->m_logPen;
	auto& m_eState = pDoc->m_eState;
	auto& m_ptMoveBegin = pDoc->m_ptMoveBegin;

	// 记录初始坐标
	m_ptMoveBegin = point;

	// 按下 SHIFT 进入选择模式，否则正常绘图
	if (nFlags & MK_SHIFT)
	{
		POSITION pos = m_listShape.GetTailPosition();
		while (pos)
		{
			auto pShape = m_listShape.GetPrev(pos);
			if (pShape->PtInShape(point) && pShape->getEable())
			{
				m_pCurrentShape = pShape;				//选定对象
				m_eState = CCADDoc::SELECT;				//更改标志位
				break;
			}
		}
	}
	else
	{
		if (m_eState == CCADDoc::SELECT)				//进入移动状态
		{
			if (m_pCurrentShape->PtInShape(point))
			{
				//创建新的操作记录
				m_pCurrentOperation = new CMoveOpt(m_pCurrentShape);
				m_pCurrentOperation->setMoveBegin(m_pCurrentShape->getMidpt());

				//移动前的状态和数据初始化
				m_eState = CCADDoc::MOVE;				//更改标志位
				goto END;
			}
		}
		//设定图形种类
		m_pCurrentShape = m_pShapeFactory->CreateShape();

		//配置图形参数
		m_pCurrentShape->UpdataInfo(m_ptMoveBegin, point);
		m_pCurrentShape->setLogBrush(m_logBrush);
		m_pCurrentShape->setLogPen(m_logPen);

		//设定状态
		m_eState = CCADDoc::DRAW;

		//创建新的操作记录
		m_pCurrentOperation = new CAddOpt(m_pCurrentShape);
	}
END:
	SetCapture();								//超出窗口区域也能获取鼠标消息
	InvalidateRect(NULL, FALSE);
	CView::OnLButtonDown(nFlags, point);
}


void CCADView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// 从Doc中拉取变量
	CCADDoc* pDoc = GetDocument();
	auto& m_pCurrentShape = pDoc->m_pCurrentShape;
	auto& m_pCurrentOperation = pDoc->m_pCurrentOperation;
	auto& m_listShape = pDoc->m_listShape;
	auto& m_eState = pDoc->m_eState;
	auto& m_ptMoveBegin = pDoc->m_ptMoveBegin;
	auto& m_stkToUndo = pDoc->m_stkToUndo;

	// 状态判断
	switch (m_eState)
	{
	case CCADDoc::DRAW:
		//刷新参数
		m_pCurrentShape->UpdataInfo(m_ptMoveBegin, point);

		//如果光标相对落点未发生有效位移，判定为无效操作
		{
			CPoint ptOff = point - m_ptMoveBegin;
			if (sqrt(ptOff.x * ptOff.x + ptOff.y * ptOff.y) < 25)
			{
				//释放内存
				delete m_pCurrentOperation;
				delete m_pCurrentShape;

				//选定置空
				m_pCurrentShape = nullptr;
				m_pCurrentOperation = nullptr;

				//切换状态
				m_eState = CCADDoc::FREE;
			}
			else
			{
				//存储绘制操作
				m_stkToUndo.push(m_pCurrentOperation);

				//保存图形
				m_listShape.AddTail(m_pCurrentShape);

				//切换状态
				m_eState = CCADDoc::SELECT;

				GetDocument()->UpdateAllViews(this);
			}
		}
		break;
	case CCADDoc::MOVE:
		//移动操作完成，记录操作
		m_pCurrentOperation->setMoveEnd(m_pCurrentShape->getMidpt());
		m_stkToUndo.push(m_pCurrentOperation);

		//恢复为选定状态
		m_eState = CCADDoc::SELECT;
		break;
	default:
		break;
	}

	ReleaseCapture();				//释放鼠标控制权
	InvalidateRect(NULL, FALSE);
	CView::OnLButtonUp(nFlags, point);
}


void CCADView::OnMouseMove(UINT nFlags, CPoint point)
{
	// 从Doc中拉取变量
	CCADDoc* pDoc = GetDocument();
	auto& m_pCurrentShape = pDoc->m_pCurrentShape;
	auto& m_pCurrentOperation = pDoc->m_pCurrentOperation;
	auto& m_eState = pDoc->m_eState;
	auto& m_ptMoveBegin = pDoc->m_ptMoveBegin;

	switch (m_eState)
	{
	case CCADDoc::DRAW:
		if (nFlags & MK_LBUTTON)
		{
			m_pCurrentShape->UpdataInfo(m_ptMoveBegin, point);
		}
		break;
	case CCADDoc::MOVE:
		if (nFlags & MK_LBUTTON)
		{
			CPoint ptOff = point - m_ptMoveBegin;
			m_pCurrentShape->MoveShape(m_pCurrentOperation->getMoveBegin() + ptOff);
		}
		break;
	case CCADDoc::ROTATE:
		if (nFlags & MK_CONTROL)
		{
			m_pCurrentShape->RotateShape(point);
		}
		break;
	default:
		break;
	}

	InvalidateRect(NULL, FALSE);
	CView::OnMouseMove(nFlags, point);
}

void CCADView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// 从Doc中拉取变量
	CCADDoc* pDoc = GetDocument();
	auto& m_pCurrentShape = pDoc->m_pCurrentShape;
	auto& m_eState = pDoc->m_eState;
	auto& m_listShape = pDoc->m_listShape;

	POSITION pos = m_listShape.GetTailPosition();
	while (pos)
	{
		auto pShape = m_listShape.GetPrev(pos);
		if (pShape->PtInShape(point) && pShape->getEable())
		{
			m_pCurrentShape = pShape;		//选定对象
			m_eState = CCADDoc::SELECT;		//更改操作状态
			InvalidateRect(NULL, FALSE);

			//获取窗口资源并创建
			CPoint ptScreen = point;
			ClientToScreen(&ptScreen);			//窗口相对坐标转桌面坐标
			CMenu mn;
			mn.LoadMenu(MN_RIGHT_KEY);
			CMenu* pSubMn = mn.GetSubMenu(0);
			pSubMn->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, ptScreen.x, ptScreen.y, this);

			break;
		}
	}

	CView::OnRButtonDown(nFlags, point);
}

void CCADView::ShapeRotate()
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

void CCADView::ShapeEnable()
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

	GetDocument()->UpdateAllViews(this);
}

void CCADView::SelectLineDraw()
{
	// 从Doc中拉取变量
	CCADDoc* pDoc = GetDocument();
	auto& m_pShapeFactory = pDoc->m_pShapeFactory;

	// TODO: 在此添加命令处理程序代码
	m_pShapeFactory = new CLineFactory;
}


void CCADView::SelectRectDraw()
{
	// 从Doc中拉取变量
	CCADDoc* pDoc = GetDocument();
	auto& m_pShapeFactory = pDoc->m_pShapeFactory;

	m_pShapeFactory = new CRectFactory;
}


void CCADView::SelectEllipseDraw()
{
	// 从Doc中拉取变量
	CCADDoc* pDoc = GetDocument();
	auto& m_pShapeFactory = pDoc->m_pShapeFactory;

	m_pShapeFactory = new CEllipseFactory;
}


void CCADView::SelectPentagramDraw()
{
	// 从Doc中拉取变量
	CCADDoc* pDoc = GetDocument();
	auto& m_pShapeFactory = pDoc->m_pShapeFactory;

	m_pShapeFactory = new CPentagramFactory;
}


void CCADView::PenSetting()
{
	// 从Doc中拉取变量
	CCADDoc* pDoc = GetDocument();
	auto& m_logPen = pDoc->m_logPen;

	CPenSelectDialog CDlg;
	CDlg.setLogPen(m_logPen);
	if (CDlg.DoModal() == IDOK)
	{
		m_logPen = CDlg.getLogPen();
	}
}


void CCADView::BrushSetting()
{
	// 从Doc中拉取变量
	CCADDoc* pDoc = GetDocument();
	auto& m_logBrush = pDoc->m_logBrush;

	CBrushSelectDialog CDlg;
	CDlg.setLogBrush(m_logBrush);
	if (CDlg.DoModal() == IDOK)
	{
		m_logBrush = CDlg.getLogBrush();
	}
}


void CCADView::SelectShapePen()
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
}


void CCADView::SelectShapeBrush()
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
}


//复原操作
void CCADView::OnRedo()
{
	// 从Doc中拉取变量
	CCADDoc* pDoc = GetDocument();
	auto& m_pCurrentShape = pDoc->m_pCurrentShape;
	auto& m_eState = pDoc->m_eState;
	auto& m_stkToUndo = pDoc->m_stkToUndo;
	auto& m_stkToRedo = pDoc->m_stkToRedo;

	if (m_stkToRedo.empty())
	{
		return;
	}
	COperation* pOpt = m_stkToRedo.top();
	m_stkToRedo.pop();
	m_stkToUndo.push(pOpt);

	pOpt->Redo();
	m_eState = CCADDoc::FREE;
	m_pCurrentShape = nullptr;

	InvalidateRect(NULL, FALSE);
	GetDocument()->UpdateAllViews(this);
}

//撤销操作
void CCADView::OnUndo()
{
	// 从Doc中拉取变量
	CCADDoc* pDoc = GetDocument();
	auto& m_pCurrentShape = pDoc->m_pCurrentShape;
	auto& m_eState = pDoc->m_eState;
	auto& m_stkToUndo = pDoc->m_stkToUndo;
	auto& m_stkToRedo = pDoc->m_stkToRedo;

	if (m_stkToUndo.empty())
	{
		return;
	}
	COperation* pOpt = m_stkToUndo.top();
	m_stkToUndo.pop();
	m_stkToRedo.push(pOpt);

	pOpt->Undo();
	m_eState = CCADDoc::FREE;
	m_pCurrentShape = nullptr;

	InvalidateRect(NULL, FALSE);
	GetDocument()->UpdateAllViews(this);
}


// 重写更新，默认处理是页面整个刷新
void CCADView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: 在此添加专用代码和/或调用基类
	InvalidateRect(NULL, FALSE);
}
