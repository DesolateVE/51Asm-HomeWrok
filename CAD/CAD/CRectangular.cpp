#include "pch.h"
#include "CRectangular.h"

IMPLEMENT_SERIAL(CRectangular, CShape, 1)

void CRectangular::DrawShape(CDC* pDC)
{
	POINT RVertex[5];
	CalculateRectPos(RVertex);

	CPen cp;
	cp.CreatePenIndirect(&m_logPen);
	pDC->SelectObject(&cp);

	CBrush cb;
	cb.CreateBrushIndirect(&m_logBrush);
	pDC->SelectObject(&cb);

	pDC->Polygon(RVertex, 5);
}

void CRectangular::DrawSelectLine(CDC* pDC)
{
	POINT RVertex[5];
	CalculateRectPos(RVertex, 10);

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

	pDC->Polygon(RVertex, 5);
}

bool CRectangular::PtInShape(POINT pt)
{
	POINT RVertex[5];
	CalculateRectPos(RVertex);
	return PtInPolygon(pt, RVertex, 4);
}

void CRectangular::UpdataInfo(CPoint one, CPoint two)
{
	//计算中点
	m_ptMid = { (one.x + two.x) / 2, (one.y + two.y) / 2 };

	//计算宽高
	m_nWidth = abs(two.x - one.x);
	m_nHeight = abs(two.y - one.y);
}

void CRectangular::Serialize(CArchive& ar)
{
	CShape::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << m_nWidth << m_nHeight;
	}
	else
	{
		ar >> m_nWidth >> m_nHeight;
	}
}

void CRectangular::CalculateRectPos(POINT* pt, int Pading /*= 0*/)
{
	static double RadtoAngle = 45 / atan(1);
	double Rad = m_rotateAngle / RadtoAngle;

	POINT offset[5];
	offset[0] = RotatePoint(POINT{ (long)m_nWidth / 2 + Pading,(long)m_nHeight / 2 + Pading }, Rad);
	offset[1] = RotatePoint(POINT{ (long)m_nWidth / 2 + Pading ,-((long)m_nHeight / 2 + Pading) }, Rad);
	offset[2] = RotatePoint(POINT{ -((long)m_nWidth / 2 + Pading),-((long)m_nHeight / 2 + Pading) }, Rad);
	offset[3] = RotatePoint(POINT{ -((long)m_nWidth / 2 + Pading),(long)m_nHeight / 2 + Pading }, Rad);
	offset[4] = offset[0];

	for (int i = 0; i < 5; i++)
	{
		pt[i].x = m_ptMid.x + offset[i].x;
		pt[i].y = m_ptMid.y + offset[i].y;
	}
}

CShape* CRectFactory::CreateShape()
{
	return new CRectangular;
}
