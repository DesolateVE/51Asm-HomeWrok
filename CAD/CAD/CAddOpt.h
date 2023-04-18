#pragma once
#include "COperation.h"
class CAddOpt :
	public COperation
{
public:
	CAddOpt(CShape* pShape) :COperation(pShape) {}

public:
	void Undo() override;
	void Redo() override;
};
