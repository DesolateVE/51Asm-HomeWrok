#pragma once
#include "COperation.h"
class CRotateOpt :
	public COperation
{
public:
	CRotateOpt(CShape* pShape) :COperation(pShape) {}

public:
	void Undo() override;
	void Redo() override;

};

