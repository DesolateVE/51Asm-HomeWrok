#pragma once

#include "CShape.h"
#include "COperation.h"

class CMoveOpt :
	public COperation
{
public:
	CMoveOpt(CShape* pShape) :COperation(pShape) {}

public:
	void Undo() override;
	void Redo() override;

private:

};

