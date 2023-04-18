#pragma once

#include "CShape.h"

class COperation
{
public:
	COperation(CShape* pShape) :m_pShapeOpt(pShape) {}

public:
	virtual void Undo() = 0;	//³·Ïú
	virtual void Redo() = 0;	//ÖØ×ö

public:
	LOGBRUSH getLogBrush() const { return m_logBrush; }
	void setLogBrush(LOGBRUSH val) { m_logBrush = val; }
	LOGPEN getLogPen() const { return m_logPen; }
	void setLogPen(LOGPEN val) { m_logPen = val; }
	CPoint getMoveBegin() const { return m_ptBegin; }
	void setMoveBegin(CPoint val) { m_ptBegin = val; }
	CPoint getMoveEnd() const { return m_ptEnd; }
	void setMoveEnd(CPoint val) { m_ptEnd = val; }
	CShape* getShapeOpt() const { return m_pShapeOpt; }
	void setShapeOpt(CShape* val) { m_pShapeOpt = val; }
	double getRotateAngle() const { return m_rotateAngle; }
	void setRotateAngle(double val) { m_rotateAngle = val; }

protected:
	CShape* m_pShapeOpt;
	CPoint m_ptBegin;
	CPoint m_ptEnd;
	LOGBRUSH m_logBrush;
	LOGPEN m_logPen;
	double m_rotateAngle;
};

