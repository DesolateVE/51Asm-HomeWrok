#pragma once
#include "CShape.h"
class CEllipse :
	public CShape
{
public:
	void DrawShape(CDC* pDC) override;
	void DrawSelectLine(CDC* pDC) override;
	bool PtInShape(POINT pt) override;
	void UpdataInfo(CPoint one, CPoint two) override;

public:
	void Serialize(CArchive& ar) override;
	DECLARE_SERIAL(CEllipse)

private:
	int m_nRadius;
};


class CEllipseFactory :
	public CShapeFactory
{

public:
	CShape* CreateShape() override;

};