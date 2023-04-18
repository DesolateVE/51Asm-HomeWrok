#pragma once
#include "COperation.h"
class CUpdateBrush :
    public COperation
{
public:
    CUpdateBrush(CShape* pShape) :COperation(pShape) {}

public:
    void Undo() override;
    void Redo() override;

};

