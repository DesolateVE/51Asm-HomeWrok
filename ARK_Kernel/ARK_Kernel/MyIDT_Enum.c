#include "MyIDT_Enum.h"

ULONG EnumIDTEntry(PCIDTINFO pInfo)
{
	SEGMENT_REGISTER DesReg;
	PINTERRPUT_TRAP_DESCRIPTOR pDesEntry;

	RtlZeroMemory(&DesReg, sizeof(SEGMENT_REGISTER));
	__sidt(&DesReg);
	pDesEntry = (PINTERRPUT_TRAP_DESCRIPTOR)DesReg.BaseAddress;

	ULONG nCount = 0;
	for (size_t i = 0; i < (DesReg.Limit + 1) / sizeof(INTERRPUT_TRAP_DESCRIPTOR); i++)		// 0x100 一般来说表都是这个大小
	{
		if (!pDesEntry->P)
		{
			goto End;
		}

		pInfo->dwIndex = (ULONG)i;
		pInfo->qwFunaddr = (ULONGLONG)pDesEntry->TargetOffestLow1 | (ULONGLONG)pDesEntry->TargetOffestLow2 << 16 | (ULONGLONG)pDesEntry->TargetOffestHigh << 32;
		pInfo->wDPL = (USHORT)pDesEntry->DPL;
		pInfo->wTargetSelector = (USHORT)pDesEntry->TargetSelector;

		pInfo++;
		nCount++;

	End:
		pDesEntry++;
	}

	return nCount;
}