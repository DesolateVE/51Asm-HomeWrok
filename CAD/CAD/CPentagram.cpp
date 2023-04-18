#include "pch.h"
#include "CPentagram.h"

IMPLEMENT_SERIAL(CPentagram, CShape, 1)

void CPentagram::DrawShape(CDC* pDC)
{
	pentagram(pDC, m_nRadius, m_ptMid.x, m_ptMid.y, m_rotateAngle);
}

void CPentagram::DrawSelectLine(CDC* pDC)
{
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

	//��������
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
	//�����е�
	m_ptMid = { (one.x + two.x) / 2, (one.y + two.y) / 2 };

	//����뾶
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
// Parameter: CDC * pDC		��ͼ�豸
// Parameter: int R			���������Բ�뾶
// Parameter: int x			Բ������x
// Parameter: int y			Բ������y
// Parameter: int yDegree	ƫת��
//************************************
void CPentagram::pentagram(CDC* pDC, int R, int x, int y, double yDegree /*= 0*/)
{
	//�趨��ˢ
	CPen cp;
	cp.CreatePenIndirect(&m_logPen);
	pDC->SelectObject(&cp);

	//�趨��ˢ
	CBrush cb;
	cb.CreateBrushIndirect(&m_logBrush);
	pDC->SelectObject(&cb);
	
	double rad = atan(1.0) / 45.0;                   //ÿ�ȵĻ���ֵ
	double r = R * sin(18 * rad) / cos(36 * rad);    //����Ƕ���ĳ���

	POINT RVertex[5], rVertex[5];
	for (int k = 0; k < 5; k++)                      //��ȡ����
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
