#pragma once

#include "DriverBase.h"

typedef VOID(*REGISTERENUM_CALLBACK)(PCREGISTERINFO pInfo);

ULONG EnumRegister(
	PWCHAR pRegisterName, 
	REGISTERENUM_CALLBACK pFun
);