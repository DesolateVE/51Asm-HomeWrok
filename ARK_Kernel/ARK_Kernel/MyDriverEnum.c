#include "MyDriverEnum.h"

ULONG SearchDriver(PDRIVER_OBJECT DriverObject, DRIVERSEARCH_CALLBACK pFun)
{
	PLDR_DATA_TABLE_ENTRY pLDR = (PLDR_DATA_TABLE_ENTRY)DriverObject->DriverSection;
	PLDR_DATA_TABLE_ENTRY pBegin = pLDR;
	ULONG uDriverCount = 0;

	do
	{
		if (pLDR->DllBase)
		{
			pFun(pLDR);			// 回调
			uDriverCount++;		// 驱动数量
		}

		pLDR = (PLDR_DATA_TABLE_ENTRY)pLDR->InLoadOrderLinks.Blink;	// 链表指向下一项
	} while (pBegin != pLDR);

	return uDriverCount;
}