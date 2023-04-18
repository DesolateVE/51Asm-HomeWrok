#include "MyEnumPageInfo.h"

ULONG EnumPageInfo(PEPROCESS Process, PCPAGEINDEX pIndex, PAGE_TYPE PageType, PCPAGEINFO pBuffer)
{
	KAPC_STATE ks;
	PPAGE_TABLE_ENTRY pEntry = NULL;
	ULONG nCount = 0;

	// EPROCESS地址无效则退出
	if (!MmIsAddressValid(Process))
		return 0;

	// 依附进程
	KeStackAttachProcess(Process, &ks);

	switch (PageType)
	{
	case PAGE_PXE:
		pEntry = g_PXE_BASE;
		break;
	case PAGE_PPE:
		pEntry = g_PPE_BASE + (pIndex->wPXEIndex * 0x200);
		break;
	case PAGE_PDE:
		pEntry = g_PDE_BASE + (pIndex->wPXEIndex * 0x200 * 0x200 + pIndex->wPPEIndex * 0x200);
		break;
	case PAGE_PTE:
		pEntry = g_PTE_BASE + (pIndex->wPXEIndex * 0x200 * 0x200 * 0x200 + pIndex->wPPEIndex * 0x200 * 0x200 + pIndex->wPDEIndex * 0x200);
		break;
	}

	if (!MmIsAddressValid(pEntry))
	{
		goto End;
	}

	for (USHORT i = 0; i < 0x200; i++)
	{
		pBuffer[nCount].qwAddress = pEntry->BaseAddress << 12;
		pBuffer[nCount].bAccess = (BOOLEAN)pEntry->A;
		pBuffer[nCount].bNX = (BOOLEAN)pEntry->NX;
		pBuffer[nCount].bRW = (BOOLEAN)pEntry->RW;
		pBuffer[nCount].bUS = (BOOLEAN)pEntry->US;
		pBuffer[nCount].wIndex = i;
		nCount++;
		pEntry++;
	}

End:

	// 取消依附进程
	KeUnstackDetachProcess(&ks);

	return nCount;
}

ULONG MemoryRead(PEPROCESS eProcess, PVOID address, DWORD length, PVOID pBuffer)
{
	KAPC_STATE ks;

	// EPROCESS地址无效则退出
	if (!MmIsAddressValid(eProcess))
		return 0;

	// 依附进程
	KeStackAttachProcess(eProcess, &ks);

	try {

		ProbeForRead(address, length, 8);
		RtlCopyMemory(pBuffer, address, length);

	} except(EXCEPTION_EXECUTE_HANDLER) {
		/* Error handling code */
		goto End;
	}

End:

	// 取消依附进程
	KeUnstackDetachProcess(&ks);

	return length;
}

ULONG MemoryWrite(PEPROCESS eProcess, PVOID address, DWORD length, PVOID pBuffer)
{
	KAPC_STATE ks;

	// EPROCESS地址无效则退出
	if (!MmIsAddressValid(eProcess))
		return 0;

	// 依附进程
	KeStackAttachProcess(eProcess, &ks);

	try {

		ProbeForWrite(address, length, 8);
		RtlCopyMemory(address, pBuffer, length);

	} except(EXCEPTION_EXECUTE_HANDLER) {
		/* Error handling code */
		goto End;
	}

End:

	// 取消依附进程
	KeUnstackDetachProcess(&ks);

	return length;
}
