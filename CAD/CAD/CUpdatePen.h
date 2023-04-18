#pragma once
#include "COperation.h"
class CUpdatePen :
	public COperation
{
public:
	CUpdatePen(CShape* pShape) :COperation(pShape) {}

public:
	void Undo() override;
	void Redo() override;
};

