#pragma once

// CArchive����Խṹ��������������
CArchive& operator<<(CArchive& out, LOGPEN& pen);
CArchive& operator>>(CArchive& in, LOGPEN& pen);
CArchive& operator<<(CArchive& out, LOGBRUSH& brush);
CArchive& operator>>(CArchive& in, LOGBRUSH& brush);