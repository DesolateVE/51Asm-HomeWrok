#include "MySSDT_Enum.h"

ULONG EnumSSDTFunInfo(SSDTENUM_CALLBACK pFun)
{
	NTSTATUS Status;
	HANDLE FileHandle;
	IO_STATUS_BLOCK ioStatus;
	FILE_STANDARD_INFORMATION FileInformation;
	//����NTDLL·��
	UNICODE_STRING uniFileName;
	RtlInitUnicodeString(&uniFileName, L"\\SystemRoot\\system32\\ntdll.dll");

	//��ʼ�����ļ�������
	OBJECT_ATTRIBUTES objectAttributes;
	InitializeObjectAttributes(&objectAttributes, &uniFileName,
		OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);
	//�����ļ�

	Status = IoCreateFile(&FileHandle, FILE_READ_ATTRIBUTES | SYNCHRONIZE, &objectAttributes,
		&ioStatus, 0, FILE_READ_ATTRIBUTES, FILE_SHARE_READ, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT,
		NULL, 0, CreateFileTypeNone, NULL, IO_NO_PARAMETER_CHECKING);
	if (!NT_SUCCESS(Status))
		return 0;
	//��ȡ�ļ���Ϣ

	Status = ZwQueryInformationFile(FileHandle, &ioStatus, &FileInformation,
		sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation);
	if (!NT_SUCCESS(Status)) {
		ZwClose(FileHandle);
		return 0;
	}
	//�ж��ļ���С�Ƿ����
	if (FileInformation.EndOfFile.HighPart != 0) {
		ZwClose(FileHandle);
		return 0;
	}
	//ȡ�ļ���С
	ULONG uFileSize = FileInformation.EndOfFile.LowPart;


	//�����ڴ�
	PVOID pBuffer = kmalloc(uFileSize + 0x100);
	if (pBuffer == NULL) {
		ZwClose(FileHandle);
		return 0;
	}

	//��ͷ��ʼ��ȡ�ļ�
	LARGE_INTEGER byteOffset;
	byteOffset.LowPart = 0;
	byteOffset.HighPart = 0;
	Status = ZwReadFile(FileHandle, NULL, NULL, NULL, &ioStatus, pBuffer, uFileSize, &byteOffset, NULL);
	if (!NT_SUCCESS(Status)) {
		ZwClose(FileHandle);
		return 0;
	}
	//ȡ��������
	PIMAGE_DOS_HEADER  pDosHeader;
	PIMAGE_NT_HEADERS  pNtHeaders;
	PIMAGE_SECTION_HEADER pSectionHeader;
	ULONGLONG     FileOffset;//������64λ���ģ��������ﲻ��32���ֽ�
	PIMAGE_EXPORT_DIRECTORY pExportDirectory;
	//DLL�ڴ�����ת��DOSͷ�ṹ
	pDosHeader = (PIMAGE_DOS_HEADER)pBuffer;
	//ȡ��PEͷ�ṹ
	pNtHeaders = (PIMAGE_NT_HEADERS)((ULONGLONG)pBuffer + pDosHeader->e_lfanew);
	//�ж�PEͷ��������Ƿ�Ϊ��

	if (pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress == 0)
		return 0;

	//ȡ��������ƫ��
	FileOffset = pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

	//ȡ����ͷ�ṹ
	pSectionHeader = (PIMAGE_SECTION_HEADER)((ULONGLONG)pNtHeaders + sizeof(IMAGE_NT_HEADERS));
	PIMAGE_SECTION_HEADER pOldSectionHeader = pSectionHeader;

	//�����ڽṹ���е�ַ����
	for (UINT16 Index = 0; Index < pNtHeaders->FileHeader.NumberOfSections; Index++, pSectionHeader++)
	{
		if (pSectionHeader->VirtualAddress <= FileOffset && FileOffset <= pSectionHeader->VirtualAddress + pSectionHeader->SizeOfRawData)
			FileOffset = FileOffset - pSectionHeader->VirtualAddress + pSectionHeader->PointerToRawData;
	}

	//�������ַ
	pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)((ULONGLONG)pBuffer + FileOffset);

	//ȡ������������ַ
	PLONG AddressOfFunctions;
	FileOffset = pExportDirectory->AddressOfFunctions;

	//�����ڽṹ���е�ַ����
	pSectionHeader = pOldSectionHeader;
	for (UINT16 Index = 0; Index < pNtHeaders->FileHeader.NumberOfSections; Index++, pSectionHeader++)
	{
		if (pSectionHeader->VirtualAddress <= FileOffset && FileOffset <= pSectionHeader->VirtualAddress + pSectionHeader->SizeOfRawData)
			FileOffset = FileOffset - pSectionHeader->VirtualAddress + pSectionHeader->PointerToRawData;
	}
	AddressOfFunctions = (PLONG)((ULONGLONG)pBuffer + FileOffset);//����ע��һ��foa��rva

	//ȡ��������������
	PUSHORT AddressOfNameOrdinals;
	FileOffset = pExportDirectory->AddressOfNameOrdinals;

	//�����ڽṹ���е�ַ����
	pSectionHeader = pOldSectionHeader;
	for (UINT16 Index = 0; Index < pNtHeaders->FileHeader.NumberOfSections; Index++, pSectionHeader++)
	{
		if (pSectionHeader->VirtualAddress <= FileOffset && FileOffset <= pSectionHeader->VirtualAddress + pSectionHeader->SizeOfRawData)
			FileOffset = FileOffset - pSectionHeader->VirtualAddress + pSectionHeader->PointerToRawData;
	}
	AddressOfNameOrdinals = (PUSHORT)((ULONGLONG)pBuffer + FileOffset);//ע��һ��foa��rva

	//ȡ�������������
	PULONG AddressOfNames;
	FileOffset = pExportDirectory->AddressOfNames;

	//�����ڽṹ���е�ַ����
	pSectionHeader = pOldSectionHeader;
	for (UINT16 Index = 0; Index < pNtHeaders->FileHeader.NumberOfSections; Index++, pSectionHeader++)
	{
		if (pSectionHeader->VirtualAddress <= FileOffset && FileOffset <= pSectionHeader->VirtualAddress + pSectionHeader->SizeOfRawData)
			FileOffset = FileOffset - pSectionHeader->VirtualAddress + pSectionHeader->PointerToRawData;
	}
	AddressOfNames = (PULONG)((ULONGLONG)pBuffer + FileOffset);//ע��һ��foa��rva

	//����������
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
			*	Zwϵ�к���ȫ�����ַ�ʽ���� syscall 4�ֽ�֮����Ƿ�����
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

			// �ص�����
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