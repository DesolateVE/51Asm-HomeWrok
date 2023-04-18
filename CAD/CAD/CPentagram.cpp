#include "pch.h"
#include "CPentagram.h"

IMPLEMENT_SERIAL(CPentagram, CShape, 1)

void CPentagram::DrawShape(CDC* pDC)
{
	pentagram(pDC, m_nRadius, m_ptMid.x, m_ptMid.y, m_rotateAngle);
}

void CPentagram::DrawSelectLine(CDC* pDC)
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
	pDC->Ellipse(m_ptMid.x - m_nRadius, m_ptMid.y - m_nRadius, m_ptMid.x + m_nRadius, m_ptMid.y + m_nRadius);
}

bool CPentagram::PtInShape(POINT pt)
{
	long difx = pt.x - m_ptMid.x;
	long dify = pt.y - m_ptMid.y;
	double distance = sqrt(difx * difx + dify * dify);
	if (distance < m_nRadius)
	{
		return true;
	}
	return false;
}


void CPentagram::UpdataInfo(CPoint one, CPoint two)
{
	//计算中点
	m_ptMid = { (one.x + two.x) / 2, (one.y + two.y) / 2 };

	//计算半径
	long difx = abs(two.x - one.x);
	long dify = abs(two.y - one.y);
	m_nRadius = max(difx, dify) / 2;
}

void CPentagram::Serialize(CArchive& ar)
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

//************************************
// Method:    pentagram
// FullName:  CPentagram::pentagram
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: CDC * pDC		绘图设备
// Parameter: int R			五角星外切圆半径
// Parameter: int x			圆心坐标x
// Parameter: int y			圆心坐标y
// Parameter: int yDegree	偏转角
//************************************
void CPentagram::pentagram(CDC* pDC, int R, int x, int y, double yDegree /*= 0*/)
{
	//设定笔刷
	CPen cp;
	cp.CreatePenIndirect(&m_logPen);
	pDC->SelectObject(&cp);

	//设定画刷
	CBrush cb;
	cb.CreateBrushIndirect(&m_logBrush);
	pDC->SelectObject(&cb);
	
	double rad = atan(1.0) / 45.0;                   //每度的弧度值
	double r = R * sin(18 * rad) / cos(36 * rad);    //五角星短轴的长度

	POINT RVertex[5], rVertex[5];
	for (int k = 0; k < 5; k++)                      //求取坐标
	{
		RVertex[k] = { int(x - (R * cos((90 + k * 72 + yDegree) * rad))), int(y - (R * sin((90 + k * 72 + yDegree) * rad))) };
		rVertex[k] = { int(x - (r * cos((90 + 36 + k * 72 + yDegree) * rad))), int(y - (r * sin((90 + 36 + k * 72 + yDegree) * rad))) };
	}

	for (int i = 0; i < 5; i++)
	{
		POINT polylinepoint[4] = { RVertex[i], rVertex[i], { x, y },RVertex[i] };
		POINT polylinepoint1[4] = { RVertex[(i + 1) % 5], rVertex[i], {x, y }, RVertex[(i + 1) % 5] };
		pDC->Polygon(polylinepoint, 4);
		pDC->Polygon(polylinepoint1, 4);
	}

}

CShape* CPentagramFactory::CreateShape()
{
	return new CPentagram;
}
