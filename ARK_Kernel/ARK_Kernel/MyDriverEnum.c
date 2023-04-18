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
			pFun(pLDR);			// �ص�
			uDriverCount++;		// ��������
		}

		pLDR = (PLDR_DATA_TABLE_ENTRY)pLDR->InLoadOrderLinks.Blink;	// ����ָ����һ��
	} while (pBegin != pLDR);

	return uDriverCount;
}