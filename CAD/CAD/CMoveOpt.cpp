#include "pch.h"
#include "CMoveOpt.h"

void CMoveOpt::Undo()
{
	m_pShapeOpt->MoveShape(m_ptBegin);
}

void CMoveOpt::Redo()
{
	m_pShapeOpt->MoveShape(m_ptEnd);
}
