#pragma once
#include "COperation.h"

class CDelOpt :
	public COperation
{
public:
	CDelOpt(CShape* pShape) :COperation(pShape) {}

public:
	void Undo() override;
	void Redo() override;

};

