#include "MySSDT_Enum.h"

ULONG EnumSSDTFunInfo(SSDTENUM_CALLBACK pFun)
{
	NTSTATUS Status;
	HANDLE FileHandle;
	IO_STATUS_BLOCK ioStatus;
	FILE_STANDARD_INFORMATION FileInformation;
	//设置NTDLL路径
	UNICODE_STRING uniFileName;
	RtlInitUnicodeString(&uniFileName, L"\\SystemRoot\\system32\\ntdll.dll");

	//初始化打开文件的属性
	OBJECT_ATTRIBUTES objectAttributes;
	InitializeObjectAttributes(&objectAttributes, &uniFileName,
		OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);
	//创建文件

	Status = IoCreateFile(&FileHandle, FILE_READ_ATTRIBUTES | SYNCHRONIZE, &objectAttributes,
		&ioStatus, 0, FILE_READ_ATTRIBUTES, FILE_SHARE_READ, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT,
		NULL, 0, CreateFileTypeNone, NULL, IO_NO_PARAMETER_CHECKING);
	if (!NT_SUCCESS(Status))
		return 0;
	//获取文件信息

	Status = ZwQueryInformationFile(FileHandle, &ioStatus, &FileInformation,
		sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation);
	if (!NT_SUCCESS(Status)) {
		ZwClose(FileHandle);
		return 0;
	}
	//判断文件大小是否过大
	if (FileInformation.EndOfFile.HighPart != 0) {
		ZwClose(FileHandle);
		return 0;
	}
	//取文件大小
	ULONG uFileSize = FileInformation.EndOfFile.LowPart;


	//分配内存
	PVOID pBuffer = kmalloc(uFileSize + 0x100);
	if (pBuffer == NULL) {
		ZwClose(FileHandle);
		return 0;
	}

	//从头开始读取文件
	LARGE_INTEGER byteOffset;
	byteOffset.LowPart = 0;
	byteOffset.HighPart = 0;
	Status = ZwReadFile(FileHandle, NULL, NULL, NULL, &ioStatus, pBuffer, uFileSize, &byteOffset, NULL);
	if (!NT_SUCCESS(Status)) {
		ZwClose(FileHandle);
		return 0;
	}
	//取出导出表
	PIMAGE_DOS_HEADER  pDosHeader;
	PIMAGE_NT_HEADERS  pNtHeaders;
	PIMAGE_SECTION_HEADER pSectionHeader;
	ULONGLONG     FileOffset;//这里是64位数的，所以这里不是32个字节
	PIMAGE_EXPORT_DIRECTORY pExportDirectory;
	//DLL内存数据转成DOS头结构
	pDosHeader = (PIMAGE_DOS_HEADER)pBuffer;
	//取出PE头结构
	pNtHeaders = (PIMAGE_NT_HEADERS)((ULONGLONG)pBuffer + pDosHeader->e_lfanew);
	//判断PE头导出表表是否为空

	if (pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress == 0)
		return 0;

	//取出导出表偏移
	FileOffset = pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

	//取出节头结构
	pSectionHeader = (PIMAGE_SECTION_HEADER)((ULONGLONG)pNtHeaders + sizeof(IMAGE_NT_HEADERS));
	PIMAGE_SECTION_HEADER pOldSectionHeader = pSectionHeader;

	//遍历节结构进行地址运算
	for (UINT16 Index = 0; Index < pNtHeaders->FileHeader.NumberOfSections; Index++, pSectionHeader++)
	{
		if (pSectionHeader->VirtualAddress <= FileOffset && FileOffset <= pSectionHeader->VirtualAddress + pSectionHeader->SizeOfRawData)
			FileOffset = FileOffset - pSectionHeader->VirtualAddress + pSectionHeader->PointerToRawData;
	}

	//导出表地址
	pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)((ULONGLONG)pBuffer + FileOffset);

	//取出导出表函数地址
	PLONG AddressOfFunctions;
	FileOffset = pExportDirectory->AddressOfFunctions;

	//遍历节结构进行地址运算
	pSectionHeader = pOldSectionHeader;
	for (UINT16 Index = 0; Index < pNtHeaders->FileHeader.NumberOfSections; Index++, pSectionHeader++)
	{
		if (pSectionHeader->VirtualAddress <= FileOffset && FileOffset <= pSectionHeader->VirtualAddress + pSectionHeader->SizeOfRawData)
			FileOffset = FileOffset - pSectionHeader->VirtualAddress + pSectionHeader->PointerToRawData;
	}
	AddressOfFunctions = (PLONG)((ULONGLONG)pBuffer + FileOffset);//这里注意一下foa和rva

	//取出导出表函数名字
	PUSHORT AddressOfNameOrdinals;
	FileOffset = pExportDirectory->AddressOfNameOrdinals;

	//遍历节结构进行地址运算
	pSectionHeader = pOldSectionHeader;
	for (UINT16 Index = 0; Index < pNtHeaders->FileHeader.NumberOfSections; Index++, pSectionHeader++)
	{
		if (pSectionHeader->VirtualAddress <= FileOffset && FileOffset <= pSectionHeader->VirtualAddress + pSectionHeader->SizeOfRawData)
			FileOffset = FileOffset - pSectionHeader->VirtualAddress + pSectionHeader->PointerToRawData;
	}
	AddressOfNameOrdinals = (PUSHORT)((ULONGLONG)pBuffer + FileOffset);//注意一下foa和rva

	//取出导出表函数序号
	PULONG AddressOfNames;
	FileOffset = pExportDirectory->AddressOfNames;

	//遍历节结构进行地址运算
	pSectionHeader = pOldSectionHeader;
	for (UINT16 Index = 0; Index < pNtHeaders->FileHeader.NumberOfSections; Index++, pSectionHeader++)
	{
		if (pSectionHeader->VirtualAddress <= FileOffset && FileOffset <= pSectionHeader->VirtualAddress + pSectionHeader->SizeOfRawData)
			FileOffset = FileOffset - pSectionHeader->VirtualAddress + pSectionHeader->PointerToRawData;
	}
	AddressOfNames = (PULONG)((ULONGLONG)pBuffer + FileOffset);//注意一下foa和rva

	//分析导出表
	ULONG uNameOffset = 0;
	ULONG uOffset = 0;
	LPSTR FunName;
	PVOID pFuncAddr;
	ULONG uServerIndex;
	ULONG uAddressOfNames;
	ULONG uFunCount = 0;

	for (ULONG uIndex = 0; uIndex < pExportDirectory->NumberOfNames; uIndex++, AddressOfNames++, AddressOfNameOrdinals++)
	{
		uAddressOfNames = *AddressOfNames;
		pSectionHeader = pOldSectionHeader;
		for (UINT32 Index = 0; Index < pNtHeaders->FileHeader.NumberOfSections; Index++, pSectionHeader++)
		{
			if (pSectionHeader->VirtualAddress <= uAddressOfNames && uAddressOfNames <= pSectionHeader->VirtualAddress + pSectionHeader->SizeOfRawData)
				uOffset = uAddressOfNames - pSectionHeader->VirtualAddress + pSectionHeader->PointerToRawData;
		}
		FunName = (LPSTR)((ULONGLONG)pBuffer + uOffset);
		if (FunName[0] == 'Z' && FunName[1] == 'w')
		{
			pSectionHeader = pOldSectionHeader;
			uOffset = (ULONG)AddressOfFunctions[*AddressOfNameOrdinals];
			for (UINT32 Index = 0; Index < pNtHeaders->FileHeader.NumberOfSections; Index++, pSectionHeader++)
			{
				if (pSectionHeader->VirtualAddress <= uOffset && uOffset <= pSectionHeader->VirtualAddress + pSectionHeader->SizeOfRawData)
					uNameOffset = uOffset - pSectionHeader->VirtualAddress + pSectionHeader->PointerToRawData;
			}

			/*
			*	| 4C:8BD1                  | mov r10,rcx
			*	| B8 1B000000              | mov eax,0x1B
			*	Zw系列函数全是这种方式进入 syscall 4字节之后便是服务标号
			*/

			pFuncAddr = (PVOID)((ULONGLONG)pBuffer + uNameOffset);
			uServerIndex = *(PULONG)((ULONGLONG)pFuncAddr + 4);
			FunName[0] = 'N';
			FunName[1] = 't';

			CSSDTINFO ssdtInfo;
			ANSI_STRING aName = { 0 };
			UNICODE_STRING wName = { 0 };

			RtlZeroMemory(&ssdtInfo, sizeof(CSSDTINFO));
			RtlInitAnsiString(&aName, FunName);
			RtlAnsiStringToUnicodeString(&wName, &aName, TRUE);

			ssdtInfo.dwFunNum = uServerIndex;
			ssdtInfo.qwFunaddr = GetSSDTFunctionAddress(uServerIndex);
			RtlCopyMemory(ssdtInfo.szFunName, wName.Buffer, wName.Length);

			if (!ssdtInfo.qwFunaddr)
			{
				goto End;
			}

			// 回调函数
			pFun(&ssdtInfo);
			uFunCount++;

		End:
			RtlFreeUnicodeString(&wName);
		}
	}
	kfree(pBuffer);
	ZwClose(FileHandle);
	return uFunCount;
}