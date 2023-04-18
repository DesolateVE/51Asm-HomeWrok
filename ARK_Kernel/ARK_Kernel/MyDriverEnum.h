#pragma once

#include "DriverBase.h"

typedef VOID(*DRIVERSEARCH_CALLBACK)(PLDR_DATA_TABLE_ENTRY);

ULONG SearchDriver(
	PDRIVER_OBJECT DriverObject,
	DRIVERSEARCH_CALLBACK pFun
);