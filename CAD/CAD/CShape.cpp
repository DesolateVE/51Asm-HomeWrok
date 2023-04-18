#include "pch.h"
#include "CShape.h"

IMPLEMENT_SERIAL(CShape, CObject, 1)

CShape::CShape() :m_bEable(true), m_rotateAngle(0)
{

}

void CShape::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);			//�ø��ຯ���������κ��£�������ʶ
	if (ar.IsStoring())
	{
		ar << m_rotateAngle << m_ptMid << m_logPen << m_logBrush << m_bEable;
	}
	else
	{
		ar >> m_rotateAngle >> m_ptMid >> m_logPen >> m_logBrush >> m_bEable;
	}
}


void CShape::MoveShape(CPoint ptDest)
{
	m_ptMid = ptDest;
}

void CShape::RotateShape(CPoint ptDest)
{
	static double RadtoAngle = 45 / atan(1);
	m_rotateAngle = atan2(ptDest.y - m_ptMid.y, ptDest.x - m_ptMid.x) * RadtoAngle;
}

bool CShape::PtInPolygon(POINT p, POINT* ptPolygon, int nCount)
{
	int nCross = 0;
	for (int i = 0; i < nCount; i++)
	{
		POINT p1 = ptPolygon[i];
		POINT p2 = ptPolygon[(i + 1) % nCount];		// ��P1��P2�γ�����  
		if (p1.y == p2.y)
			continue;
		if (p.y < min(p1.y, p2.y))
			continue;
		if (p.y >= max(p1.y, p2.y))
			continue;
		// �󽻵��x���꣨��ֱ������ʽ����ת��������   
		double x = (double)(p.y - p1.y) * (double)(p2.x - p1.x) / (double)(p2.y - p1.y) + p1.x;
		// ֻͳ��p1p2��p�������ߵĽ���  
		if (x > p.x)
		{
			nCross++;
		}
	}
	// ����Ϊż�������ڶ����֮��  
	// ����Ϊ���������ڶ����֮��
	if ((nCross % 2) == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}


POINT CShape::RotatePoint(POINT pt, double rad)
{
	POINT rtPt;
	rtPt.x = (long)(pt.x * cos(rad) - pt.y * sin(rad));
	rtPt.y = (long)(pt.x * sin(rad) + pt.y * cos(rad));
	return rtPt;
}

