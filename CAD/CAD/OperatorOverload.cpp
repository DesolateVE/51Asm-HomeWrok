#include "pch.h"
#include "OperatorOverload.h"

CArchive& operator<<(CArchive& out, LOGPEN& pen)
{
	out << pen.lopnColor << pen.lopnStyle << pen.lopnWidth;
	return out;
}

CArchive& operator<<(CArchive& out, LOGBRUSH& brush)
{
	out << brush.lbColor << brush.lbHatch << brush.lbStyle;
	return out;
}

CArchive& operator>>(CArchive& in, LOGPEN& pen)
{
	in >> pen.lopnColor >> pen.lopnStyle >> pen.lopnWidth;
	return in;
}

CArchive& operator>>(CArchive& in, LOGBRUSH& brush)
{
	in >> brush.lbColor >> brush.lbHatch >> brush.lbStyle;
	return in;
}