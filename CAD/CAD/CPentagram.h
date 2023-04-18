#pragma once
#include "CShape.h"
class CPentagram :
	public CShape
{
public:
	void DrawShape(CDC* pDC) override;
	void DrawSelectLine(CDC* pDC) override;
	bool PtInShape(POINT pt) override;
	void UpdataInfo(CPoint one, CPoint two) override;

public:
	void Serialize(CArchive& ar) override;
	DECLARE_SERIAL(CPentagram)

private:
	void pentagram(CDC* pDC, int R, int x, int y, double yDegree = 0);

private:
	int m_nRadius;
};


class CPentagramFactory :
	public CShapeFactory
{

public:
	CShape* CreateShape() override;

};