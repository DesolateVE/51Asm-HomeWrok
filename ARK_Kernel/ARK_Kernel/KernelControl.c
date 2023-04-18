#include "KernelControl.h"
#include "MyFileSearch.h"
#include "MyProcessEnum.h"
#include "MySSDT_Enum.h"
#include "MyPortEnum.h"
#include "MyEnumPageInfo.h"
#include "MyDriverEnum.h"
#include "MyIDT_Enum.h"
#include "MyGDT_Enum.h"
#include "MyRegisterEnum.h"


/************************************************************************/
/*                         对链表结构的支持                             */
/************************************************************************/

#define MAKE_LIST_STRUCT(STRUCT, OBJECTNAME) \
typedef struct _##STRUCT##_ENTRY \
{ \
	SINGLE_LIST_ENTRY		SingleListEntry; \
	STRUCT					OBJECTNAME; \
}STRUCT##_ENTRY, * P##STRUCT##_ENTRY; \
void Push##OBJECTNAME##Entry(PSINGLE_LIST_ENTRY ListHead, P##STRUCT##_ENTRY Entry) {PushEntryList(ListHead, &(Entry->SingleListEntry));} \
P##STRUCT##_ENTRY Pop##OBJECTNAME##Entry(PSINGLE_LIST_ENTRY ListHead) \
{ \
	PSINGLE_LIST_ENTRY SingleListEntry; \
	SingleListEntry = PopEntryList(ListHead); \
	return CONTAINING_RECORD(SingleListEntry, STRUCT##_ENTRY, SingleListEntry); \
} \
SINGLE_LIST_ENTRY g_##OBJECTNAME##List;

#define FAST_FIND(FUNCTION) \
nEntryNum = FUNCTION; \
nSize = sizeof(ULONG); \
*(PULONG)pBuffer = nEntryNum;

#define FAST_INFO(STRUCT, OBJECTNAME) \
while (TRUE) \
{ \
	pEntry = Pop##OBJECTNAME##Entry(&g_##OBJECTNAME##List); \
	if (pEntry == NULL) break; \
	RtlCopyMemory((P##STRUCT)pBuffer + nEntryNum, &((P##STRUCT##_ENTRY)pEntry)->OBJECTNAME, sizeof(STRUCT)); \
	kfree(pEntry); \
	nEntryNum++; \
} \
nSize = sizeof(STRUCT) * nEntryNum;


/*
*	[MAKELISTSTRUCT] 宏解释
*
*	MAKELISTSTRUCT(CFILEINFO, FileInfo) 等价于声明以下结构
*
*	typedef struct _CFILEINFO_ENTRY
*	{
*		SINGLE_LIST_ENTRY		SingleListEn
*		CFILEINFO				FileInfo;
*	}CFILEINFO_ENTRY, * PCFILEINFO_ENTRY;
*
*	void PushFileInfoEntry(PSINGLE_LIST_ENTRY ListHead, PCFILEINFO_ENTRY Entry)
*	{
*		PushEntryList(ListHead, &(Entry->SingleListEntry));
*	}
*
*	PCFILEINFO_ENTRY PopFileInfoEntry(PSINGLE_LIST_ENTRY ListHead)
*	{
*		PSINGLE_LIST_ENTRY SingleListEntry;
*		SingleListEntry = PopEntryList(ListHead);
*		return CONTAINING_RECORD(SingleListEntry, CFILEINFO_ENTRY, SingleListEntry);
*	}
*
*	SINGLE_LIST_ENTRY g_FileInfoList;
*/

MAKE_LIST_STRUCT(CFILEINFO, FileInfo)			// 文件信息链表
MAKE_LIST_STRUCT(CSSDTINFO, SSDTInfo)			// SSDT信息链表
MAKE_LIST_STRUCT(CPROCESS, ProcessInfo)			// 进程信息链表
MAKE_LIST_STRUCT(CTHREAD, ThreadInfo)			// 线程信息链表
MAKE_LIST_STRUCT(CMODULE, ModuleInfo)			// 模块信息链表
MAKE_LIST_STRUCT(CREGISTERINFO, RegisterInfo)	// 注册表信息链表

/************************************************************************/
/*                         文件搜索功能的回调                           */
/************************************************************************/

VOID FileSearchCallBack(PFILE_BOTH_DIR_INFORMATION pDir)
{
	PCFILEINFO_ENTRY pEntry = kmalloc(sizeof(CFILEINFO_ENTRY));

	RtlCopyMemory(pEntry->FileInfo.szFileName, pDir->FileName, pDir->FileNameLength);
	pEntry->FileInfo.bFileType = pDir->FileAttributes & FILE_ATTRIBUTE_DIRECTORY;
	pEntry->FileInfo.qwChangeTime = pDir->LastWriteTime;
	pEntry->FileInfo.qwFileSize = pDir->EndOfFile;

	PushFileInfoEntry(&g_FileInfoList, pEntry);
}

/************************************************************************/
/*                          SSDT遍历功能的回调                          */
/************************************************************************/

VOID SSDTEnumCallBack(PCSSDTINFO pInfo)
{
	PCSSDTINFO_ENTRY pEntry = kmalloc(sizeof(CSSDTINFO));
	RtlCopyMemory(&pEntry->SSDTInfo, pInfo, sizeof(CSSDTINFO));

	PushSSDTInfoEntry(&g_SSDTInfoList, pEntry);
}

/************************************************************************/
/*              模块信息遍历回调（驱动模块和进程模块同格式）            */
/************************************************************************/

VOID ModuleEnumCallBack(PLDR_DATA_TABLE_ENTRY pInfo)
{
	PCMODULE_ENTRY pEntry = kmalloc(sizeof(CMODULE));
	RtlZeroMemory(pEntry, sizeof(CMODULE));

	pEntry->ModuleInfo.dwSize = pInfo->SizeOfImage;
	pEntry->ModuleInfo.pEntryPoint = pInfo->EntryPoint;
	pEntry->ModuleInfo.pDllBase = pInfo->DllBase;
	RtlCopyMemory(pEntry->ModuleInfo.szBaseDllName, pInfo->BaseDllName.Buffer, pInfo->BaseDllName.Length);
	RtlCopyMemory(pEntry->ModuleInfo.szFullDllName, pInfo->FullDllName.Buffer, pInfo->FullDllName.Length);

	PushModuleInfoEntry(&g_ModuleInfoList, pEntry);
}

/************************************************************************/
/*                           进程信息回调                               */
/************************************************************************/

VOID ProcessEnumCallBack(PEPROCESS pInfo)
{
	PCPROCESS_ENTRY pEntry = kmalloc(sizeof(CPROCESS));
	RtlZeroMemory(pEntry, sizeof(CPROCESS));

	pEntry->ProcessInfo.hPID = PsGetProcessId(pInfo);
	pEntry->ProcessInfo.hPPID = PsGetProcessInheritedFromUniqueProcessId(pInfo);
	pEntry->ProcessInfo.pEProcess = pInfo;
	PUNICODE_STRING pName = (PUNICODE_STRING)(*(PULONGLONG)((char*)pInfo + 0x5c0));
	RtlCopyMemory(pEntry->ProcessInfo.szProcessName, pName->Buffer, pName->Length);

	PushProcessInfoEntry(&g_ProcessInfoList, pEntry);
}

/************************************************************************/
/*                           线程信息回调                               */
/************************************************************************/

VOID ThreadEnumCallBack(PETHREAD pInfo)
{
	PCTHREAD_ENTRY pEntry = kmalloc(sizeof(CTHREAD));

	pEntry->ThreadInfo.hTID = PsGetThreadId(pInfo);
	pEntry->ThreadInfo.pEThread = pInfo;
	pEntry->ThreadInfo.dwStatus = *((PUCHAR)pInfo + 0x184);
	pEntry->ThreadInfo.dwBasePriority = *((PUCHAR)pInfo + 0x233);

	PushThreadInfoEntry(&g_ThreadInfoList, pEntry);
}

/************************************************************************/
/*                            注册表信息回调                            */
/************************************************************************/

VOID RegisterEnumCallBack(PCREGISTERINFO pInfo)
{
	PCREGISTERINFO_ENTRY pEntry = kmalloc(sizeof(CREGISTERINFO));
	RtlCopyMemory(&pEntry->RegisterInfo, pInfo, sizeof(CREGISTERINFO));

	PushRegisterInfoEntry(&g_RegisterInfoList, pEntry);
}


NTSTATUS DriverEntry(DRIVER_OBJECT* DriverObject, PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);

	UNICODE_STRING ustrDevName;
	RtlInitUnicodeString(&ustrDevName, DEVICE_NAME);

	PDEVICE_OBJECT pDevObj = NULL;
	NTSTATUS Status = IoCreateDevice(
		DriverObject,
		0,
		&ustrDevName,
		FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN,
		FALSE,						//独占
		&pDevObj
	);
	if (!NT_SUCCESS(Status))
	{
		DbgPrint("[51asm] IoCreateDevice Status:%p\n", Status);
		return Status;
	}

	pDevObj->Flags |= DO_BUFFERED_IO;

	UNICODE_STRING ustrSymbolName;
	RtlInitUnicodeString(&ustrSymbolName, SYMBOL_NAME);
	Status = IoCreateSymbolicLink(&ustrSymbolName, &ustrDevName);
	if (!NT_SUCCESS(Status))
	{
		DbgPrint("[51asm] IoCreateDevice Status:%p\n", Status);
		if (pDevObj != NULL)
		{
			IoDeleteDevice(pDevObj);
		}
		return Status;
	}

	DriverObject->MajorFunction[IRP_MJ_CREATE] = &DispatchCreate;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = &DispatchClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = &DispatchControl;

	DriverObject->DriverUnload = Unload;

	// 初始化SSDT表地址
	g_pKeServiceDescriptorTable = (PSYSTEM_SERVICE_TABLE)GetKeServiceDescriptorTable64();

	// 初始化页表地址
	InitPageTableBase();

	return STATUS_SUCCESS;
}

VOID Unload(DRIVER_OBJECT* DriverObject)
{
	UNICODE_STRING ustrSymbolName;
	RtlInitUnicodeString(&ustrSymbolName, SYMBOL_NAME);
	IoDeleteSymbolicLink(&ustrSymbolName);

	if (DriverObject->DeviceObject != NULL)
	{
		IoDeleteDevice(DriverObject->DeviceObject);
	}
}

NTSTATUS DispatchCreate(DEVICE_OBJECT* DeviceObject, IRP* Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Irp);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS DispatchClose(DEVICE_OBJECT* DeviceObject, IRP* Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS DispatchControl(DEVICE_OBJECT* DeviceObject, IRP* Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	//DbgPrint("[51asm] FUNCTION --> %s()\n", __FUNCTION__);

	PIO_STACK_LOCATION pIrpStack = IoGetCurrentIrpStackLocation(Irp);

	PVOID pBuffer = Irp->AssociatedIrp.SystemBuffer;
	//ULONG nLength = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
	ULONG nIoControlCode = pIrpStack->Parameters.DeviceIoControl.IoControlCode;

	ULONG nSize = 0;
	ULONG nEntryNum = 0;
	PVOID pEntry = NULL;

	switch (nIoControlCode)
	{

	case IOCTL_FILE_FIND:
		FAST_FIND(SearchDirectory(pBuffer, FileSearchCallBack))
			break;
	case IOCTL_FILE_INFO:
		FAST_INFO(CFILEINFO, FileInfo)
			break;

	case IOCTL_DRIVER_FIND:
		FAST_FIND(SearchDriver(DeviceObject->DriverObject, ModuleEnumCallBack))
			break;
	case IOCTL_DRIVER_INFO:
		FAST_INFO(CMODULE, ModuleInfo)
			break;

	case IOCTL_SSDT_FIND:
		FAST_FIND(EnumSSDTFunInfo(SSDTEnumCallBack))
			break;
	case IOCTL_SSDT_INFO:
		FAST_INFO(CSSDTINFO, SSDTInfo);
		break;

	case IOCTL_PROCESS_FIND:
		FAST_FIND(EnumProcess(ProcessEnumCallBack))
			break;
	case IOCTL_PROCESS_INFO:
		FAST_INFO(CPROCESS, ProcessInfo)
			break;

	case IOCTL_THREAD_FIND:
		FAST_FIND(EnumThread((PEPROCESS)(*(PULONG64)pBuffer), ThreadEnumCallBack))
			break;
	case IOCTL_THREAD_INFO:
		FAST_INFO(CTHREAD, ThreadInfo)
			break;

	case IOCTL_MODULE_FIND:
		FAST_FIND(EnumMoudle((PEPROCESS)(*(PULONG64)pBuffer), ModuleEnumCallBack))
			break;
	case IOCTL_MODULE_INFO:
		FAST_INFO(CMODULE, ModuleInfo)
			break;


	case IOCTL_IDT_FIND:
		nEntryNum = EnumIDTEntry(pBuffer);
		nSize = sizeof(CIDTINFO) * nEntryNum;
		break;

	case IOCTL_GDT_FIND:
		nEntryNum = EnumGDTEntry(pBuffer);
		nSize = sizeof(CGDTINFO) * nEntryNum;
		break;

	case IOCTL_PAGE_FIND:
		nEntryNum = EnumPageInfo(((PCPAGEQUERY)pBuffer)->eProcess, &((PCPAGEQUERY)pBuffer)->Index, ((PCPAGEQUERY)pBuffer)->Type, pBuffer);
		nSize = sizeof(CPAGEINFO) * nEntryNum;
		break;

	case IOCTL_MEMORY_READ:
		nSize = MemoryRead(((PCMEMORYQUERY)pBuffer)->eProcess, ((PCMEMORYQUERY)pBuffer)->address, ((PCMEMORYQUERY)pBuffer)->length, pBuffer);
		break;
	case IOCTL_MEMORY_WRITE:
		MemoryWrite(((PCMEMORYQUERY)pBuffer)->eProcess, ((PCMEMORYQUERY)pBuffer)->address, ((PCMEMORYQUERY)pBuffer)->length, ((PCMEMORYQUERY)pBuffer)->value);
		break;

	case IOCTL_REGISTER_FIND:
		FAST_FIND(EnumRegister(pBuffer, RegisterEnumCallBack))
			break;
	case IOCTL_REGISTER_INFO:
		FAST_INFO(CREGISTERINFO, RegisterInfo)
			break;

	case IOCTL_FILE_DELETE:
		DeleteFiles(pBuffer);
		break;
	case IOCTL_PROCESS_CLOSE:
		ZwKillProcess(*(PHANDLE)pBuffer);
		break;

	default:
		break;
	}

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = nSize;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}