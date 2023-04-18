#include "pch.h"
#include "CDelOpt.h"

void CDelOpt::Undo()
{
	m_pShapeOpt->setEable(true);
}

void CDelOpt::Redo()
{
	m_pShapeOpt->setEable(false);
}
