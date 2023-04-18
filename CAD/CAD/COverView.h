#pragma once
#include "CADDoc.h"

// COverView 视图

class COverView : public CTreeView
{
	DECLARE_DYNCREATE(COverView)

protected:
	COverView();           // 动态创建所使用的受保护的构造函数
	virtual ~COverView();

protected:
	CImageList m_ImgList;

public:
	CCADDoc* GetDocument() const;

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNMClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void ShapeRotate();
	afx_msg void ShapeEnable();
	afx_msg void SelectShapePen();
	afx_msg void SelectShapeBrush();
};


