#pragma once

#include "OperatorOverload.h"

class CShape
	:public CObject
{
protected:
	double m_rotateAngle;	// ��ת��
	CPoint m_ptMid;			// �е�

	LOGPEN m_logPen;		// ����
	LOGBRUSH m_logBrush;	// ��ˢ

	bool m_bEable;			// �Ƿ���ʾ

public:
	CShape();

public:
	// ���л�
	void Serialize(CArchive& ar) override;
	DECLARE_SERIAL(CShape)

public:
	// ����ͼ��
	virtual void DrawShape(CDC* pDC) {}
	// ����ѡ���
	virtual void DrawSelectLine(CDC* pDC) {}
	// ����ϵ�ж�
	virtual bool PtInShape(POINT pt) { return false; }
	// ����ͼ����������
	virtual void UpdataInfo(CPoint one, CPoint two) {}

public:
	void MoveShape(CPoint ptDest);
	void RotateShape(CPoint ptDest);

public:
	LOGPEN getLogPen() const { return m_logPen; }
	void setLogPen(LOGPEN val) { m_logPen = val; }
	LOGBRUSH getLogBrush() const { return m_logBrush; }
	void setLogBrush(LOGBRUSH val) { m_logBrush = val; }
	bool getEable() const { return m_bEable; }
	void setEable(bool val) { m_bEable = val; }
	double getRotateAngle() const { return m_rotateAngle; }
	void setRotateAngle(double val) { m_rotateAngle = val; }
	CPoint getMidpt() const { return m_ptMid; }
	void setMidpt(CPoint val) { m_ptMid = val; }

public:
	static bool PtInPolygon(POINT p, POINT* ptPolygon, int nCount);
	static POINT RotatePoint(POINT pt, double rad);
};


class CShapeFactory
{
public:
	virtual CShape* CreateShape() = 0;
};

