#pragma once

#include "OperatorOverload.h"

class CShape
	:public CObject
{
protected:
	double m_rotateAngle;	// 旋转角
	CPoint m_ptMid;			// 中点

	LOGPEN m_logPen;		// 画笔
	LOGBRUSH m_logBrush;	// 画刷

	bool m_bEable;			// 是否显示

public:
	CShape();

public:
	// 序列化
	void Serialize(CArchive& ar) override;
	DECLARE_SERIAL(CShape)

public:
	// 绘制图形
	virtual void DrawShape(CDC* pDC) {}
	// 绘制选择框
	virtual void DrawSelectLine(CDC* pDC) {}
	// 落点关系判断
	virtual bool PtInShape(POINT pt) { return false; }
	// 生成图形所需数据
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

