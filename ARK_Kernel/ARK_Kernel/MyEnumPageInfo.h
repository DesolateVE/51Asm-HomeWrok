#pragma once

#include "DriverBase.h"

ULONG EnumPageInfo(PEPROCESS Process, PCPAGEINDEX pIndex, PAGE_TYPE PageType, PCPAGEINFO pBuffer);
ULONG MemoryRead(PEPROCESS eProcess,PVOID address,DWORD length,PVOID pBuffer);
ULONG MemoryWrite(PEPROCESS eProcess, PVOID address, DWORD length, PVOID pBuffer);