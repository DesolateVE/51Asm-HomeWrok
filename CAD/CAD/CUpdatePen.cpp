#include "pch.h"
#include "CUpdatePen.h"

void CUpdatePen::Undo()
{
	LOGPEN logPen = m_pShapeOpt->getLogPen();
	m_pShapeOpt->setLogPen(getLogPen());
	setLogPen(logPen);
}

void CUpdatePen::Redo()
{
	Undo();
}
