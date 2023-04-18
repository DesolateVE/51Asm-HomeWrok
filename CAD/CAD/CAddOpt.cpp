#include "pch.h"
#include "CAddOpt.h"

void CAddOpt::Undo()
{
	getShapeOpt()->setEable(false);
}

void CAddOpt::Redo()
{
	getShapeOpt()->setEable(true);
}
