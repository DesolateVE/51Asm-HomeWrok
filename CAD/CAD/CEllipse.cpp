#include "pch.h"
#include "CEllipse.h"

IMPLEMENT_SERIAL(CEllipse, CShape, 1)

void CEllipse::DrawShape(CDC* pDC)
{
	//设定笔刷
	CPen cp;
	cp.CreatePenIndirect(&m_logPen);
	pDC->SelectObject(&cp);

	//设定画刷
	CBrush cb;
	cb.CreateBrushIndirect(&m_logBrush);
	pDC->SelectObject(&cb);

	pDC->Ellipse(m_ptMid.x - m_nRadius, m_ptMid.y - m_nRadius, m_ptMid.x + m_nRadius, m_ptMid.y + m_nRadius);
}

void CEllipse::DrawSelectLine(CDC* pDC)
{
	//设定笔刷
	CPen cp;
	LOGPEN logPen = { PS_DOT, NULL, 0x2E8B57 };
	cp.CreatePenIndirect(&logPen);
	pDC->SelectObject(&cp);

	//设定画刷
	CBrush cb;
	LOGBRUSH logBrush = { BS_HOLLOW ,NULL,NULL };
	cb.CreateBrushIndirect(&logBrush);
	pDC->SelectObject(&cb);

	//绘制虚线
	pDC->Ellipse(m_ptMid.x - m_nRadius - 10, m_ptMid.y - m_nRadius - 10, m_ptMid.x + m_nRadius + 10, m_ptMid.y + m_nRadius + 10);
}

bool CEllipse::PtInShape(POINT pt)
{
	long difx = pt.x - m_ptMid.x;
	long dify = pt.y - m_ptMid.y;
	double distance = sqrt(difx * difx + dify * dify);
	if (distance < m_nRadius + 10)
	{
		return true;
	}
	return false;
}

void CEllipse::UpdataInfo(CPoint one, CPoint two)
{
	//计算中点
	m_ptMid = { (one.x + two.x) / 2, (one.y + two.y) / 2 };

	//计算半径
	long difx = abs(two.x - one.x);
	long dify = abs(two.y - one.y);
	m_nRadius = max(difx, dify) / 2;
}

void CEllipse::Serialize(CArchive& ar)
{
	CShape::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << m_nRadius;
	}
	else
	{
		ar >> m_nRadius;
	}
}

CShape* CEllipseFactory::CreateShape()
{
	return new CEllipse;
}
