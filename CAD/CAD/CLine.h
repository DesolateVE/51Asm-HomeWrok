#pragma once
#include "CShape.h"
class CLine :
	public CShape
{
public:
	void DrawShape(CDC* pDC) override;
	bool PtInShape(POINT pt) override;
	void DrawSelectLine(CDC* pDC) override;
	void UpdataInfo(CPoint one, CPoint two) override;

public:
	void Serialize(CArchive& ar) override;
	DECLARE_SERIAL(CLine)

private:
	void CalculateRectPos(POINT* pt);

private:
	double m_fLength;
};

class CLineFactory :
	public CShapeFactory
{

public:
	CShape* CreateShape() override;

};