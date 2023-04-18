#include "MyRegisterEnum.h"

PVOID GetFunctionAddr(PCWSTR FunctionName)
{
	UNICODE_STRING UniCodeFunctionName;
	RtlInitUnicodeString(&UniCodeFunctionName, FunctionName);
	return MmGetSystemRoutineAddress(&UniCodeFunctionName);
}

//NTKERNELAPI NTSTATUS ZwRenameKey(HANDLE KeyHandle, PUNICODE_STRING NewName);
typedef NTSTATUS(__fastcall* ZWRENAMEKEY)(HANDLE KeyHandle, PUNICODE_STRING NewName);
ZWRENAMEKEY MyZwRenameKey = NULL;

void RegCreateKey(LPWSTR KeyName)
{
	OBJECT_ATTRIBUTES objectAttributes;
	UNICODE_STRING usKeyName;
	NTSTATUS ntStatus;
	HANDLE hRegister;
	RtlInitUnicodeString(&usKeyName, KeyName);
	InitializeObjectAttributes(&objectAttributes,
		&usKeyName,
		OBJ_CASE_INSENSITIVE,//对大小写敏感
		NULL,
		NULL);
	ntStatus = ZwCreateKey(&hRegister, KEY_ALL_ACCESS, &objectAttributes, 0, NULL, REG_OPTION_NON_VOLATILE, NULL);
	if (NT_SUCCESS(ntStatus))
	{
		ZwClose(hRegister);
		DbgPrint("ZwCreateKey success!\n");
	}
	else
	{
		DbgPrint("ZwCreateKey failed!\n");
	}
}

void RegRenameKey(LPWSTR OldKeyName, LPWSTR NewKeyName)
{
	OBJECT_ATTRIBUTES objectAttributes;
	HANDLE hRegister;
	NTSTATUS ntStatus;
	UNICODE_STRING usOldKeyName, usNewKeyName;
	RtlInitUnicodeString(&usOldKeyName, OldKeyName);
	RtlInitUnicodeString(&usNewKeyName, NewKeyName);
	InitializeObjectAttributes(&objectAttributes,
		&usOldKeyName,
		OBJ_CASE_INSENSITIVE,//对大小写敏感
		NULL,
		NULL);
	ntStatus = ZwOpenKey(&hRegister, KEY_ALL_ACCESS, &objectAttributes);
	if (NT_SUCCESS(ntStatus))
	{
		ntStatus = MyZwRenameKey(hRegister, &usNewKeyName);
		ZwFlushKey(hRegister);
		ZwClose(hRegister);
		DbgPrint("ZwRenameKey success!\n");
	}
	else
	{
		DbgPrint("ZwRenameKey failed!\n");
	}
}

void RegDeleteKey(LPWSTR KeyName)
{
	OBJECT_ATTRIBUTES objectAttributes;
	UNICODE_STRING usKeyName;
	NTSTATUS ntStatus;
	HANDLE hRegister;
	RtlInitUnicodeString(&usKeyName, KeyName);
	InitializeObjectAttributes(&objectAttributes,
		&usKeyName,
		OBJ_CASE_INSENSITIVE,//对大小写敏感
		NULL,
		NULL);
	ntStatus = ZwOpenKey(&hRegister, KEY_ALL_ACCESS, &objectAttributes);
	if (NT_SUCCESS(ntStatus))
	{
		ntStatus = ZwDeleteKey(hRegister);
		ZwClose(hRegister);
		DbgPrint("ZwDeleteKey success!\n");
	}
	else
	{
		DbgPrint("ZwDeleteKey failed!\n");
	}
}

void RegSetValueKey(LPWSTR KeyName, LPWSTR ValueName, DWORD DataType, PVOID DataBuffer, DWORD DataLength)
{
	OBJECT_ATTRIBUTES objectAttributes;
	UNICODE_STRING usKeyName, usValueName;
	NTSTATUS ntStatus;
	HANDLE hRegister;
	//ULONG Type;
	RtlInitUnicodeString(&usKeyName, KeyName);
	RtlInitUnicodeString(&usValueName, ValueName);
	InitializeObjectAttributes(&objectAttributes,
		&usKeyName,
		OBJ_CASE_INSENSITIVE,//对大小写敏感
		NULL,
		NULL);
	ntStatus = ZwOpenKey(&hRegister, KEY_ALL_ACCESS, &objectAttributes);
	if (NT_SUCCESS(ntStatus))
	{
		ntStatus = ZwSetValueKey(hRegister, &usValueName, 0, DataType, DataBuffer, DataLength);
		ZwFlushKey(hRegister);
		ZwClose(hRegister);
		DbgPrint("ZwSetValueKey success!\n");
	}
	else
	{
		DbgPrint("ZwSetValueKey failed!\n");
	}
}

void RegDeleteValueKey(LPWSTR KeyName, LPWSTR ValueName)
{
	OBJECT_ATTRIBUTES objectAttributes;
	UNICODE_STRING usKeyName, usValueName;
	NTSTATUS ntStatus;
	HANDLE hRegister;
	RtlInitUnicodeString(&usKeyName, KeyName);
	RtlInitUnicodeString(&usValueName, ValueName);
	InitializeObjectAttributes(&objectAttributes,
		&usKeyName,
		OBJ_CASE_INSENSITIVE,//对大小写敏感
		NULL,
		NULL);
	ntStatus = ZwOpenKey(&hRegister, KEY_ALL_ACCESS, &objectAttributes);
	if (NT_SUCCESS(ntStatus))
	{
		ntStatus = ZwDeleteValueKey(hRegister, &usValueName);
		ZwFlushKey(hRegister);
		ZwClose(hRegister);
		DbgPrint("ZwDeleteValueKey success!\n");
	}
	else
	{
		DbgPrint("ZwDeleteValueKey failed!\n");
	}
}

ULONG EnumRegister(PWCHAR pRegisterName, REGISTERENUM_CALLBACK pFun)
{
	ULONG ulCount = 0;
	ULONG ulSize, i;
	NTSTATUS ntStatus;
	PKEY_FULL_INFORMATION pfi;
	PKEY_VALUE_FULL_INFORMATION pvbi;
	OBJECT_ATTRIBUTES objectAttributes;
	HANDLE hRegister;
	UNICODE_STRING RegUnicodeString;
	CREGISTERINFO RegisterInfo;


	// 初始化UNICODE_STRING字符串
	RtlInitUnicodeString(&RegUnicodeString, pRegisterName);
	InitializeObjectAttributes(&objectAttributes, &RegUnicodeString, OBJ_CASE_INSENSITIVE, NULL, NULL);
	// 打开注册表
	ntStatus = ZwOpenKey(&hRegister, KEY_ALL_ACCESS, &objectAttributes);
	if (NT_SUCCESS(ntStatus))
		DbgPrint("Open Register Successfully\n");
	// 查询VALUE的大小
	ZwQueryKey(hRegister, KeyFullInformation, NULL, 0, &ulSize);
	pfi = (PKEY_FULL_INFORMATION)kmalloc(ulSize);
	ZwQueryKey(hRegister, KeyFullInformation, pfi, ulSize, &ulSize);
	ulCount = pfi->SubKeys + pfi->Values;
	// 遍历子项的名字

	for (i = 0; i < pfi->SubKeys; ++i)
	{
		ZwEnumerateKey(hRegister, i, KeyBasicInformation, NULL, 0, &ulSize);
		PKEY_BASIC_INFORMATION pbi = (PKEY_BASIC_INFORMATION)kmalloc(ulSize);
		ZwEnumerateKey(hRegister, i, KeyBasicInformation, pbi, ulSize, &ulSize);

		RtlZeroMemory(&RegisterInfo, sizeof(CREGISTERINFO));
		RegisterInfo.bType = 0xFFFF;
		RtlCopyMemory(RegisterInfo.szName, pbi->Name, pbi->NameLength);
		pFun(&RegisterInfo);

		kfree(pbi);
	}
	for (i = 0; i < pfi->Values; ++i)
	{
		//查询单个VALUE的大小
		ZwEnumerateValueKey(hRegister, i, KeyValueFullInformation, NULL, 0, &ulSize);
		pvbi = (PKEY_VALUE_FULL_INFORMATION)kmalloc(ulSize);
		//查询单个VALUE的详情
		ZwEnumerateValueKey(hRegister, i, KeyValueFullInformation, pvbi, ulSize, &ulSize);
		// 获取到值的名字

		RtlZeroMemory(&RegisterInfo, sizeof(CREGISTERINFO));
		RegisterInfo.bType = pvbi->Type;
		RtlCopyMemory(RegisterInfo.szName, pvbi->Name, pvbi->NameLength);
		if (pvbi->DataLength > MAX_PATH)	// 存不下，截断
		{
			pvbi->DataLength = MAX_PATH;
		}
		RtlCopyMemory(RegisterInfo.ucDate, (PCHAR)pvbi + pvbi->DataOffset, pvbi->DataLength);
		RegisterInfo.DateLength = pvbi->DataLength;
		pFun(&RegisterInfo);

		kfree(pvbi);
	}
	kfree(pfi);
	ZwClose(hRegister);

	return ulCount;
}