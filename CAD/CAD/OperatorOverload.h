#pragma once

// CArchive类针对结构体进行运算符重载
CArchive& operator<<(CArchive& out, LOGPEN& pen);
CArchive& operator>>(CArchive& in, LOGPEN& pen);
CArchive& operator<<(CArchive& out, LOGBRUSH& brush);
CArchive& operator>>(CArchive& in, LOGBRUSH& brush);