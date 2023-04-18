#pragma once

#include <Ntifs.h>			// 放在前面防止重定义
#include <Ntddk.h>
#include <ntimage.h>
#include <stddef.h>
#include <Ntstrsafe.h>

#include "IOCTLSTD.h"

#define INVALID_HANDLE_VALUE (HANDLE) - 1
#define kmalloc(_n) ExAllocatePool2(POOL_FLAG_PAGED, _n, 'VE')
#define kfree(_p)	ExFreePoolWithTag(_p, 'VE')

// [内核模块信息]
// 这里只定义了部分结构
typedef struct _LDR_DATA_TABLE_ENTRY {
	LIST_ENTRY		InLoadOrderLinks;
	LIST_ENTRY		InMemoryOrderLinks;
	LIST_ENTRY		InInitializationOrderLinks;
	PVOID			DllBase;
	PVOID			EntryPoint;
	ULONG			SizeOfImage;
	UNICODE_STRING	FullDllName;
	UNICODE_STRING 	BaseDllName;
	ULONG			Flags;
	USHORT			ObsoleteLoadCount;
	USHORT			TlsIndex;
	LIST_ENTRY		HashLinks;
	ULONG			TimeDateStamp;

	// 后续结构省略
	UCHAR			Other[1];
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

// [系统服务表]
typedef struct _SYSTEM_SERVICE_TABLE {
	PVOID  		ServiceTableBase;
	PVOID  		ServiceCounterTableBase;
	ULONGLONG  	NumberOfServices;
	PVOID  		ParamTableBase;
} SYSTEM_SERVICE_TABLE, * PSYSTEM_SERVICE_TABLE;

// [段寄存器]
#pragma pack(push,1)
typedef struct _SEGMENT_REGISTER {
	USHORT        Limit;
	ULONGLONG     BaseAddress;
} SEGMENT_REGISTER, * PSEGMENT_REGISTER;
#pragma pack(pop)

// [选择子]
typedef struct _SEGMENT_SELECTOR {
	USHORT	RPL : 2;
	USHORT	TI : 1;
	USHORT	SI : 13;
}SEGMENT_SELECTOR, * PSEGMENT_SELECTOR;

// [中断门和陷阱门描述符表]
typedef struct _INTERRPUT_TRAP_DESCRIPTOR {
	ULONG				TargetOffestLow1 : 16;
	ULONG				TargetSelector : 16;
	ULONG				IST : 3;
	ULONG : 5;
	ULONG				Type : 4;
	ULONG : 1;
	ULONG				DPL : 2;
	ULONG				P : 1;
	ULONG				TargetOffestLow2 : 16;
	ULONG				TargetOffestHigh;
	ULONG : 32;
} INTERRPUT_TRAP_DESCRIPTOR, * PINTERRPUT_TRAP_DESCRIPTOR;

// [代码段/数据段描述符]
typedef struct _CODEDATA_SEGEMENT_DESCRIPTOR {
	ULONG		SegmentLimitLow : 16;
	ULONG		BaseAddressLow : 16;
	ULONG		BaseAddressHigh1 : 8;
	ULONG		Type : 4;
	ULONG		S : 1;
	ULONG		DPL : 2;
	ULONG		P : 1;
	ULONG		SegmentLimitHigh : 4;
	ULONG		AVL : 1;
	ULONG		L : 1;
	ULONG		DB : 1;
	ULONG		G : 1;
	ULONG		BaseAddressHigh2 : 8;
}CODEDATA_SEGEMENT_DESCRIPTOR, * PCODEDATA_SEGEMENT_DESCRIPTOR;

// [系统段描述符]
typedef struct _SYSTEM_SEGEMENT_DESCRIPTOR {
	ULONGLONG	SegmentLimitLow : 16;
	ULONGLONG	BaseAddressLow : 16;
	ULONGLONG	BaseAddressLow1 : 8;
	ULONGLONG	Type : 4;
	ULONGLONG : 1;
	ULONGLONG	DPL : 2;
	ULONGLONG	P : 1;
	ULONGLONG	SegmentLimitHigh : 4;
	ULONGLONG	AVL : 1;
	ULONGLONG : 2;
	ULONGLONG	G : 1;
	ULONGLONG	BaseAddressLow2 : 8;
	ULONGLONG	BaseAddressHigh : 32;
	ULONGLONG : 32;
}SYSTEM_SEGEMENT_DESCRIPTOR, * PSYSTEM_SEGEMENT_DESCRIPTOR;

// [页表]
typedef struct _PAGE_TABLE_ENTRY {
	ULONGLONG	P : 1;
	ULONGLONG	RW : 1;				// 是否可写
	ULONGLONG	US : 1;				// User or System
	ULONGLONG	PWT : 1;
	ULONGLONG	PCD : 1;
	ULONGLONG	A : 1;				// 是否被访问
	ULONGLONG	D : 1;				// PTE用
	ULONGLONG	PAT : 1;			// PTE用
	ULONGLONG	G : 1;				// PTE用
	ULONGLONG	AVL : 3;
	ULONGLONG	BaseAddress : 40;
	ULONGLONG : 7;
	ULONGLONG	MPK : 4;			// 仅在 CR4.PKE = 1 有效
	ULONGLONG	NX : 1;				// 不可执行
}PAGE_TABLE_ENTRY, * PPAGE_TABLE_ENTRY;

//
//	已导出，但头文件未声明函数
//

NTKERNELAPI PCHAR		PsGetProcessImageFileName(IN PEPROCESS Process);
NTKERNELAPI HANDLE		PsGetProcessInheritedFromUniqueProcessId(IN PEPROCESS Process);
NTKERNELAPI PPEB		PsGetProcessPeb(PEPROCESS Process);
NTKERNELAPI NTSTATUS	ZwOpenThread(OUT PHANDLE ThreadHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes, IN PCLIENT_ID ClientId);

ULONGLONG	GetKeServiceDescriptorTable64();				// 获取SSDT表地址
VOID		InitPageTableBase();							// 动态获取页表地址
ULONGLONG	GetSSDTFunctionAddress(ULONGLONG Index);		// 通过编号获取函数地址

extern PSYSTEM_SERVICE_TABLE g_pKeServiceDescriptorTable;
extern PPAGE_TABLE_ENTRY g_PTE_BASE;
extern PPAGE_TABLE_ENTRY g_PDE_BASE;
extern PPAGE_TABLE_ENTRY g_PPE_BASE;
extern PPAGE_TABLE_ENTRY g_PXE_BASE;