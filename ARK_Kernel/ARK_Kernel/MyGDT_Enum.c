#include "MyGDT_Enum.h"

ULONG EnumGDTEntry(PCGDTINFO pInfo)
{
	SEGMENT_REGISTER DesReg;
	PCODEDATA_SEGEMENT_DESCRIPTOR pDesEntry;
	PSYSTEM_SEGEMENT_DESCRIPTOR pSysEntry;

	RtlZeroMemory(&DesReg, sizeof(SEGMENT_REGISTER));
	_sgdt(&DesReg);
	pDesEntry = (PCODEDATA_SEGEMENT_DESCRIPTOR)DesReg.BaseAddress;
	ULONG nCount = 0;

	for (size_t i = 0; i < (DesReg.Limit + 1) / 8; i++)
	{
		if (!pDesEntry->P)
		{
			goto End;
		}


		if (pDesEntry->S == 0)		// 系统段
		{
			pSysEntry = (PSYSTEM_SEGEMENT_DESCRIPTOR)pDesEntry;
			pInfo->dwIndex = (ULONG)i;
			pInfo->bL = 1;
			pInfo->bS = 0;
			pInfo->qwBase = pSysEntry->BaseAddressLow | pSysEntry->BaseAddressLow1 << 16 | pSysEntry->BaseAddressLow2 << 24 | pSysEntry->BaseAddressHigh << 32;
			pInfo->wDPL = (USHORT)pSysEntry->DPL;
			pInfo->wType = (USHORT)pSysEntry->Type;
			pInfo->dwLimit = (ULONG)pSysEntry->SegmentLimitLow | (ULONG)pSysEntry->SegmentLimitHigh << 16;

			i++;
			pDesEntry++;
		}
		else						// 数据段或者代码段
		{
			pInfo->dwIndex = (ULONG)i;
			pInfo->bL = (BOOLEAN)pDesEntry->L;
			pInfo->bS = 1;
			pInfo->qwBase = (ULONGLONG)pDesEntry->BaseAddressLow | pDesEntry->BaseAddressHigh1 << 16 | pDesEntry->BaseAddressHigh2 << 24;
			pInfo->wDPL = (USHORT)pDesEntry->DPL;
			pInfo->wType = (USHORT)pDesEntry->Type;
			pInfo->dwLimit = pDesEntry->SegmentLimitLow | pDesEntry->SegmentLimitHigh << 16;
		}

		pInfo++;
		nCount++;

	End:
		pDesEntry++;
	}

	return nCount;
}