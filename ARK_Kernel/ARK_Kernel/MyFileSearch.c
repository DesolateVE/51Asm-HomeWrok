#include "MyFileSearch.h"

HANDLE kFindFirstFile(LPTSTR lpDirectory, PFILE_BOTH_DIR_INFORMATION pDir, ULONG uLength)
{
	//DbgPrint("[51asm] FUNCTION --> %s()\n", __FUNCTION__);

	HANDLE hFind = INVALID_HANDLE_VALUE;
	OBJECT_ATTRIBUTES oa = { 0 };
	IO_STATUS_BLOCK ioStatus = { 0 };

	NTSTATUS ntStatus;
	UNICODE_STRING ustrFolder;

	WCHAR szBuffer[MAX_PATH];
	RtlStringCbCopyW(szBuffer, sizeof(szBuffer), L"\\DosDevices\\");
	RtlStringCbCatW(szBuffer, sizeof(szBuffer), lpDirectory);
	RtlInitUnicodeString(&ustrFolder, szBuffer);

	//DbgPrint("[51asm] FilePath --> %wZ\n", &ustrFolder);

	InitializeObjectAttributes(
		&oa,
		&ustrFolder,
		OBJ_KERNEL_HANDLE |			// 内核句柄，不提供给三环使用
		OBJ_CASE_INSENSITIVE,		// 不区分大小
		NULL,
		NULL
	);

	ntStatus = IoCreateFile(
		&hFind,
		FILE_LIST_DIRECTORY | SYNCHRONIZE | FILE_ANY_ACCESS,
		&oa,
		&ioStatus,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_OPEN,
		FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT,
		NULL,
		0,
		CreateFileTypeNone,
		NULL,
		IO_NO_PARAMETER_CHECKING
	);

	//DbgPrint("[51asm] IoCreateFile --> %p\n", ntStatus);
	//DbgPrint("[51asm] hFile --> %p\n", hFind);

	if (NT_SUCCESS(ntStatus) && hFind != INVALID_HANDLE_VALUE)
	{
		ntStatus = ZwQueryDirectoryFile(
			hFind,							// File Handle
			NULL,							// Event
			NULL,							// Apc routine
			NULL,							// Apc context
			&ioStatus,						// IoStatusBlock
			pDir,							// FileInformation
			uLength,							// Length
			FileBothDirectoryInformation,		// FileInformationClass
			TRUE,							// ReturnSingleEntry
			NULL,							// FileName
			FALSE							//RestartScan
		);

		if (!NT_SUCCESS(ntStatus))
		{
			ZwClose(hFind);
			hFind = INVALID_HANDLE_VALUE;
		}
	}

	return hFind;
}

BOOLEAN kFindNextFile(HANDLE hFind, PFILE_BOTH_DIR_INFORMATION pDir, ULONG uLength)
{
	IO_STATUS_BLOCK ioStatus = { 0 };
	NTSTATUS ntStatus;

	ntStatus = ZwQueryDirectoryFile(
		hFind,						// File Handle
		NULL,						// Event
		NULL,						// Apc routine
		NULL,						// Apc context
		&ioStatus,					// IoStatusBlock
		pDir,						// FileInformation
		uLength,						// Length
		FileBothDirectoryInformation,  // FileInformationClass
		FALSE,						// ReturnSingleEntry
		NULL,						// FileName
		FALSE						// RestartScan
	);

	if (NT_SUCCESS(ntStatus))
		return TRUE;
	else
		return FALSE;
}

ULONG SearchDirectory(LPTSTR lpPath, FILESEARCH_CALLBACK pFun)
{
	//WCHAR szFileName[MAX_PATH];
	ULONG uFileCount = 0;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	PFILE_BOTH_DIR_INFORMATION pDir;
	ULONG uDirSize = sizeof(FILE_BOTH_DIR_INFORMATION) + MAX_PATH * 2;

	pDir = kmalloc(uDirSize);
	if (pDir == NULL)
	{
		DbgPrint("[51asm] kmalloc failed\n");
		return uFileCount;
	}

	hFind = kFindFirstFile(lpPath, pDir, uDirSize);
	kfree(pDir);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		pDir = kmalloc(uDirSize * 0x100);
		if (pDir == NULL)
		{
			DbgPrint("[51asm] kmalloc failed\n");
			return uFileCount;
		}

		PFILE_BOTH_DIR_INFORMATION pthis = pDir;
		if (kFindNextFile(hFind, pthis, uDirSize * 0x100))
		{
			while (TRUE)
			{
				//RtlZeroMemory(szFileName, sizeof(szFileName));
				//RtlCopyMemory(szFileName, pthis->FileName, pthis->FileNameLength);

				/*if (wcscmp(szFileName, L"..") == 0)
				{
					goto Next;
				}*/

				if (pthis->FileName[0] == L'.' && pthis->FileName[1] == L'.' && pthis->FileNameLength == 4)
				{
					goto Next;
				}

				//if (pthis->FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				//{
				//	DbgPrint("[51asm] 目录 --> %S\n", szFileName);
				//}
				//else
				//{
				//	DbgPrint("[51asm] 文件 --> %S\n", szFileName);
				//}

				pFun(pthis);		// 回调函数
				uFileCount++;		// 文件数量

			Next:

				if (pthis->NextEntryOffset == 0)
				{
					break;
				}

				pthis = (PFILE_BOTH_DIR_INFORMATION)((char*)pthis + pthis->NextEntryOffset);
			}
		}

		kfree(pDir);
		ZwClose(hFind);
	}

	return uFileCount;
}

BOOLEAN DeleteFiles(PWCHAR pFileName)
{
	WCHAR szBuffer[MAX_PATH];
	RtlStringCbCopyW(szBuffer, sizeof(szBuffer), L"\\DosDevices\\");
	RtlStringCbCatW(szBuffer, sizeof(szBuffer), pFileName);

	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	UNICODE_STRING StrFile;
	RtlInitUnicodeString(&StrFile, szBuffer);
	OBJECT_ATTRIBUTES objAttribute = { 0 };
	ULONG ulAttributes = OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE;
	InitializeObjectAttributes(&objAttribute, &StrFile, ulAttributes, NULL, NULL);
	// 删除指定文件/文件夹
	ntStatus = ZwDeleteFile(&objAttribute);
	return TRUE;
}

HANDLE KernelCreateFile(PUNICODE_STRING pStrFile, BOOLEAN bIsDIr)
{
	HANDLE hFile = NULL;
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	IO_STATUS_BLOCK StatusBlock = { 0 };
	ULONG ulShareAccess = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
	ULONG ulCreateOpt = FILE_SYNCHRONOUS_IO_NONALERT;
	// 初始化OBJECT_ATTRIBUTES的内容
	OBJECT_ATTRIBUTES objAttributes = { 0 };
	ULONG ulAttributes = OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE;
	InitializeObjectAttributes(&objAttributes, pStrFile, ulAttributes, NULL, NULL);
	// 创建文件对象
	ulCreateOpt |= bIsDIr ? FILE_DIRECTORY_FILE : FILE_NON_DIRECTORY_FILE;
	ntStatus = ZwCreateFile(&hFile, FILE_LIST_DIRECTORY | SYNCHRONIZE | FILE_ANY_ACCESS, &objAttributes, &StatusBlock, 0, FILE_ATTRIBUTE_NORMAL, ulShareAccess, FILE_OPEN_IF, ulCreateOpt, NULL, 0);
	if (!NT_SUCCESS(ntStatus))
		return (HANDLE)-1;
	return hFile;
}