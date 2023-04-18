#include "pch.h"
#include "CUpdateBrush.h"

void CUpdateBrush::Undo()
{
	LOGBRUSH logBrush = m_pShapeOpt->getLogBrush();
	m_pShapeOpt->setLogBrush(getLogBrush());
	setLogBrush(logBrush);
}

void CUpdateBrush::Redo()
{
	Undo();
}
