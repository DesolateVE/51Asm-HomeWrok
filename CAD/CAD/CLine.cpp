#include "pch.h"
#include "CLine.h"

IMPLEMENT_SERIAL(CLine, CShape, 1)

void CLine::DrawShape(CDC* pDC)
{
	//�趨��ˢ
	CPen cp;
	cp.CreatePenIndirect(&m_logPen);
	pDC->SelectObject(&cp);

	//�趨��ˢ
	CBrush cb;
	cb.CreateBrushIndirect(&m_logBrush);
	pDC->SelectObject(&cb);

	//����ת�����¼�������
	static double RadtoAngle = 45 / atan(1);
	double Rad = m_rotateAngle / RadtoAngle;
	POINT ptStart = m_ptMid + RotatePoint(POINT{ (long)m_fLength / 2,0 }, Rad);
	POINT ptEnd = m_ptMid + RotatePoint(POINT{ -(long)m_fLength / 2,0 }, Rad);

	//��������
	pDC->MoveTo(ptStart);
	pDC->LineTo(ptEnd);
}


bool CLine::PtInShape(POINT pt)
{
	POINT RVertex[5];
	CalculateRectPos(RVertex);
	return PtInPolygon(pt, RVertex, 4);
}


void CLine::DrawSelectLine(CDC* pDC)
{
	POINT RVertex[5];
	CalculateRectPos(RVertex);

	//�趨��ˢ
	CPen cp;
	LOGPEN logPen = { PS_DOT, NULL, 0x2E8B57 };
	cp.CreatePenIndirect(&logPen);
	pDC->SelectObject(&cp);

	//�趨��ˢ
	CBrush cb;
	LOGBRUSH logBrush = { BS_HOLLOW ,NULL,NULL };
	cb.CreateBrushIndirect(&logBrush);
	pDC->SelectObject(&cb);

	pDC->Polygon(RVertex, 5);
}


void CLine::UpdataInfo(CPoint one, CPoint two)
{
	//�����е�
	m_ptMid = { (one.x + two.x) / 2, (one.y + two.y) / 2 };

	//����Ƕ�
	static double RadtoAngle = 45 / atan(1);
	m_rotateAngle = atan2(two.y - one.y, two.x - one.x) * RadtoAngle;

	//���㳤��
	long difx = two.x - one.x;
	long dify = two.y - one.y;
	m_fLength = sqrt(difx * difx + dify * dify);
}

void CLine::Serialize(CArchive& ar)
{
	CShape::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << m_fLength;
	}
	else
	{
		ar >> m_fLength;
	}
}

void CLine::CalculateRectPos(POINT* pt)
{
	//�ڱ߾��趨
	const int Pading_x = 10;
	const int Pading_y = 10;

	static double RadtoAngle = 45 / atan(1);
	double Rad = m_rotateAngle / RadtoAngle;

	POINT offset[5];
	offset[0] = RotatePoint(POINT{ (long)m_fLength / 2 + Pading_x,Pading_y }, Rad);
	offset[1] = RotatePoint(POINT{ (long)m_fLength / 2 + Pading_x ,-Pading_y }, Rad);
	offset[2] = RotatePoint(POINT{ -((long)m_fLength / 2 + Pading_x),-Pading_y }, Rad);
	offset[3] = RotatePoint(POINT{ -((long)m_fLength / 2 + Pading_x),Pading_y }, Rad);
	offset[4] = offset[0];

	for (int i = 0; i < 5; i++)
	{
		pt[i].x = m_ptMid.x + offset[i].x;
		pt[i].y = m_ptMid.y + offset[i].y;
	}
}

CShape* CLineFactory::CreateShape()
{
	return new CLine;
}
