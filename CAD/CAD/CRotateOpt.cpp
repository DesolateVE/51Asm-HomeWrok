#include "pch.h"
#include "CRotateOpt.h"

void CRotateOpt::Undo()
{
	double dAngle = m_pShapeOpt->getRotateAngle();
	m_pShapeOpt->setRotateAngle(getRotateAngle());
	setRotateAngle(dAngle);
}

void CRotateOpt::Redo()
{
	Undo();
}
