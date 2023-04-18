#include "pch.h"
#include "CShape.h"

IMPLEMENT_SERIAL(CShape, CObject, 1)

CShape::CShape() :m_bEable(true), m_rotateAngle(0)
{

}

void CShape::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);			//该父类函数不会做任何事，仅作标识
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
		POINT p2 = ptPolygon[(i + 1) % nCount];		// 点P1与P2形成连线  
		if (p1.y == p2.y)
			continue;
		if (p.y < min(p1.y, p2.y))
			continue;
		if (p.y >= max(p1.y, p2.y))
			continue;
		// 求交点的x坐标（由直线两点式方程转化而来）   
		double x = (double)(p.y - p1.y) * (double)(p2.x - p1.x) / (double)(p2.y - p1.y) + p1.x;
		// 只统计p1p2与p向右射线的交点  
		if (x > p.x)
		{
			nCross++;
		}
	}
	// 交点为偶数，点在多边形之外  
	// 交点为奇数，点在多边形之内
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

