#pragma once

#include "DriverBase.h"

typedef VOID(*SSDTENUM_CALLBACK)(PCSSDTINFO pInfo);

ULONG EnumSSDTFunInfo(SSDTENUM_CALLBACK pFun);