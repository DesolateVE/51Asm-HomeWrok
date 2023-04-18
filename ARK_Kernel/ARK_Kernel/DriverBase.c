#include "DriverBase.h"

PSYSTEM_SERVICE_TABLE	g_pKeServiceDescriptorTable;
PPAGE_TABLE_ENTRY		g_PTE_BASE;
PPAGE_TABLE_ENTRY		g_PDE_BASE;
PPAGE_TABLE_ENTRY		g_PPE_BASE;
PPAGE_TABLE_ENTRY		g_PXE_BASE;

ULONGLONG GetKeServiceDescriptorTable64()
{
	PUCHAR StartSearchAddress = (PUCHAR)__readmsr(0xC0000082);
	PUCHAR EndSearchAddress = StartSearchAddress + 0x500;
	PUCHAR i = NULL;
	UCHAR b1 = 0, b2 = 0, b3 = 0;
	ULONG templong = 0;
	ULONGLONG addr = 0;
	for (i = StartSearchAddress; i < EndSearchAddress; i++)
	{
		if (MmIsAddressValid(i) && MmIsAddressValid(i + 1) && MmIsAddressValid(i + 2))
		{
			b1 = *i;
			b2 = *(i + 1);
			b3 = *(i + 2);
			if (b1 == 0x4c && b2 == 0x8d && b3 == 0x15) // KeServiceDescriptorTable µÄÌØÕ÷Âë 4c8d15 
			{
				memcpy(&templong, i + 3, 4);
				addr = (ULONGLONG)templong + (ULONGLONG)i + 7;
				return addr;
			}
		}
	}
	return 0;
}

VOID InitPageTableBase()
{
	g_PTE_BASE = (PPAGE_TABLE_ENTRY)(*(PULONGLONG)((ULONGLONG)MmGetVirtualForPhysical + 0x22));
	g_PDE_BASE = (PPAGE_TABLE_ENTRY)((ULONGLONG)g_PTE_BASE + (((ULONGLONG)g_PTE_BASE & 0xffffffffffff) >> 9));
	g_PPE_BASE = (PPAGE_TABLE_ENTRY)((ULONGLONG)g_PTE_BASE + (((ULONGLONG)g_PDE_BASE & 0xffffffffffff) >> 9));
	g_PXE_BASE = (PPAGE_TABLE_ENTRY)((ULONGLONG)g_PTE_BASE + (((ULONGLONG)g_PPE_BASE & 0xffffffffffff) >> 9));
}

ULONGLONG GetSSDTFunctionAddress(ULONGLONG Index)
{
	if (Index >= g_pKeServiceDescriptorTable->NumberOfServices)
	{
		return 0;
	}

	LONG dwTemp = 0;
	ULONGLONG qwTemp = 0, stb = 0, ret = 0;
	PSYSTEM_SERVICE_TABLE ssdt = g_pKeServiceDescriptorTable;
	stb = (ULONGLONG)(ssdt->ServiceTableBase);
	qwTemp = stb + 4 * Index;
	dwTemp = *(PLONG)qwTemp;
	dwTemp = dwTemp >> 4;
	ret = stb + (LONG64)dwTemp;
	return ret;
}