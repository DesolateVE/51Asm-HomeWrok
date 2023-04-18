
// CADView.h: CCADView 类的接口
//

#pragma once

#include "CShape.h"
#include "CLine.h"
#include "CRectangular.h"
#include "CEllipse.h"
#include "CPentagram.h"

#include "COperation.h"
#include "CMoveOpt.h"
#include "CAddOpt.h"
#include "CDelOpt.h"
#include "CRotateOpt.h"
#include "CUpdateBrush.h"
#include "CUpdatePen.h"

#include "CPenSelectDialog.h"
#include "CBrushSelectDialog.h"

class CCADDoc;

class CCADView : public CView
{
protected: // 仅从序列化创建
	CCADView() noexcept;
	DECLARE_DYNCREATE(CCADView)

	// 特性
public:
	CCADDoc* GetDocument() const;

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
	virtual ~CCADView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void SelectLineDraw();
	afx_msg void SelectRectDraw();
	afx_msg void SelectEllipseDraw();
	afx_msg void SelectPentagramDraw();
	afx_msg void PenSetting();
	afx_msg void BrushSetting();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void ShapeRotate();
	afx_msg void ShapeEnable();
	afx_msg void SelectShapePen();
	afx_msg void SelectShapeBrush();
	afx_msg void OnRedo();
	afx_msg void OnUndo();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
};

#ifndef _DEBUG  // CADView.cpp 中的调试版本
inline CCADDoc* CCADView::GetDocument() const
{
	return reinterpret_cast<CCADDoc*>(m_pDocument);
}
#endif

