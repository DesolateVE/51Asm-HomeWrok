#pragma once
#include "CShape.h"
class CRectangular :
	public CShape
{
public:
	void DrawShape(CDC* pDC) override;
	void DrawSelectLine(CDC* pDC) override;
	bool PtInShape(POINT pt) override;
	void UpdataInfo(CPoint one, CPoint two) override;

public:
	void Serialize(CArchive& ar) override;
	DECLARE_SERIAL(CRectangular)

private:
	void CalculateRectPos(POINT* pt, int Pading = 0);

private:
	int m_nWidth;
	int m_nHeight;
};



class CRectFactory :
	public CShapeFactory
{

public:
	CShape* CreateShape() override;

};