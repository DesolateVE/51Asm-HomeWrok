#include "MyProcessEnum.h"

//以下导出函数头文件中全有声明了
//NTKERNELAPI NTSTATUS PsLookupProcessByProcessId(HANDLE Id, PEPROCESS* Process);
//NTKERNELAPI NTSTATUS PsLookupThreadByThreadId(HANDLE Id, PETHREAD* Thread);
//NTKERNELAPI PEPROCESS IoThreadToProcess(PETHREAD Thread);
//NTKERNELAPI VOID NTAPI KeAttachProcess(PEPROCESS Process);										// 已被淘汰，使用KeStackAttachProcess
//NTKERNELAPI VOID NTAPI KeDetachProcess();
//NTKERNELAPI VOID NTAPI KeStackAttachProcess(PEPROCESS Process, PKAPC_STATE ApcState);
//NTKERNELAPI VOID NTAPI KeUnstackDetachProcess(PKAPC_STATE ApcState);

// ZwOpenThread = 0x12E			不需要从SSDT表取，变成导出函数了
// ZwTerminateThread = 0x53
typedef NTSTATUS(*PFUNCTION_TERMINATE_THREAD)(HANDLE, DWORD);
NTSTATUS ZwTerminateThread(HANDLE hThread, DWORD dwExitCode)
{
	PFUNCTION_TERMINATE_THREAD pFun = (PFUNCTION_TERMINATE_THREAD)GetSSDTFunctionAddress(0x53);
	return pFun(hThread, dwExitCode);
}

// 根据进程ID返回进程EPROCESS，失败返回NULL
PEPROCESS LookupProcess(HANDLE Pid)
{
	PEPROCESS eprocess = NULL;
	if (NT_SUCCESS(PsLookupProcessByProcessId(Pid, &eprocess)))
		return eprocess;
	else
		return NULL;
}

// 根据线程ID返回线程ETHREAD，失败返回NULL
PETHREAD LookupThread(HANDLE Tid)
{
	PETHREAD ethread;
	if (NT_SUCCESS(PsLookupThreadByThreadId(Tid, &ethread)))
		return ethread;
	else
		return NULL;
}

// 枚举指定进程的线程
ULONG EnumThread(PEPROCESS Process, THREADENUM_CALLBACK pFun)
{
	PETHREAD ethrd = NULL;
	PEPROCESS eproc = NULL;
	ULONG nCount = 0;

	for (ULONG i = 4; i < 90000; i = i + 4) // 一般来说没有超过100000的PID和TID
	{
		ethrd = LookupThread((HANDLE)i);
		if (ethrd != NULL)
		{
			// 获得线程所属进程
			eproc = IoThreadToProcess(ethrd);
			if (eproc == Process)
			{
				// 打印出ETHREAD和TID
				// 状态 = 0x184
				// 线程优先级 = 0x233
				// DbgPrint("[THREAD]ETHREAD=%p TID=%ld\n", ethrd, (ULONGLONG)PsGetThreadId(ethrd));
				pFun(ethrd);
				nCount++;
			}
			ObDereferenceObject(ethrd);
		}
	}
	return nCount;
}

// 枚举指定进程的模块
ULONG EnumMoudle(PEPROCESS Process, MODULEENUM_CALLBACK pFun)
{
	SIZE_T Peb = 0;
	SIZE_T Ldr = 0;
	PLIST_ENTRY ModListHead = NULL;
	PLIST_ENTRY Module = NULL;
	KAPC_STATE ks;
	ULONG nCount = 0;

	// EPROCESS地址无效则退出
	if (!MmIsAddressValid(Process))
		return 0;
	// 获取PEB地址
	Peb = (SIZE_T)PsGetProcessPeb(Process);
	// PEB地址无效则退出
	if (!Peb)
		return 0;
	// 依附进程
	KeStackAttachProcess(Process, &ks);
	__try
	{
		// 获得LDR地址
		// offset peb.ldr = 0x18
		Ldr = Peb + 0x18;
		// 测试是否可读，不可读则抛出异常退出
		ProbeForRead((CONST PVOID)Ldr, 8, 8);
		// 获得链表头
		// offset peb.ldr.InLoadOrderModuleList = 0x10
		ModListHead = (PLIST_ENTRY)(*(PULONG64)Ldr + 0x10);
		// 再次测试可读性
		ProbeForRead((CONST PVOID)ModListHead, 8, 8);
		// 获得第一个模块的信息
		Module = ModListHead->Flink;
		while (ModListHead != Module)
		{
			// 打印信息：基址、大小、DLL路径
			//DbgPrint("[MODULE]Base=%p Size=%ld Path=%wZ\n",
			//	(PVOID)(((PLDR_DATA_TABLE_ENTRY)Module)->DllBase),
			//	(ULONG)(((PLDR_DATA_TABLE_ENTRY)Module)->SizeOfImage),
			//	&(((PLDR_DATA_TABLE_ENTRY)Module)->FullDllName));

			pFun((PLDR_DATA_TABLE_ENTRY)Module);
			nCount++;
			Module = Module->Flink;

			// 测试下一个模块信息的可读性
			ProbeForRead((CONST PVOID)Module, 80, 8);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DbgPrint("[EnumModule]__except (EXCEPTION_EXECUTE_HANDLER)");
	}
	// 取消依附进程
	KeUnstackDetachProcess(&ks);

	return nCount;
}

// 枚举进程
ULONG EnumProcess(PROCESSENUM_CALLBACK pFun)
{
	PEPROCESS eproc = NULL;
	ULONG nCount = 0;

	for (ULONG i = 4; i < 90000; i = i + 4) // 一般来说没有超过100000的PID和TID
	{
		eproc = LookupProcess((HANDLE)i);
		if (eproc != NULL)
		{
			ObDereferenceObject(eproc);
			pFun(eproc);

			//DbgPrint("EPROCESS=%p PID=%ld PPID=%ld Name=%s\n",
			//	eproc,
			//	(ULONGLONG)PsGetProcessId(eproc),
			//	(ULONGLONG)PsGetProcessInheritedFromUniqueProcessId(eproc),
			//	PsGetProcessImageFileName(eproc));
			//EnumModule(eproc);

			nCount++;
		}
	}

	return nCount;
}

// 结束进程
void ZwKillProcess(HANDLE Pid)
{
	HANDLE hProcess = NULL;
	CLIENT_ID ClientId;
	OBJECT_ATTRIBUTES oa;
	// 填充CID
	ClientId.UniqueProcess = Pid; // 这里修改为你要的PID
	ClientId.UniqueThread = 0;
	// 填充OA
	oa.Length = sizeof(oa);
	oa.RootDirectory = 0;
	oa.ObjectName = 0;
	oa.Attributes = 0;
	oa.SecurityDescriptor = 0;
	oa.SecurityQualityOfService = 0;
	// 打开进程，如果句柄有效，则结束进程
	ZwOpenProcess(&hProcess, 1, &oa, &ClientId);
	if (hProcess)
	{
		ZwTerminateProcess(hProcess, 0);
		ZwClose(hProcess);
	};
}

// 结束线程
void ZwKillThread(HANDLE Tid)
{
	HANDLE hThread = NULL;
	CLIENT_ID ClientId;
	OBJECT_ATTRIBUTES oa;
	// 填充CID
	ClientId.UniqueProcess = 0;
	ClientId.UniqueThread = Tid; // 这里修改为你要的TID
	// 填充OA
	oa.Length = sizeof(oa);
	oa.RootDirectory = 0;
	oa.ObjectName = 0;
	oa.Attributes = 0;
	oa.SecurityDescriptor = 0;
	oa.SecurityQualityOfService = 0;

	// 打开线程，如果句柄有效，则结束进程
	ZwOpenThread(&hThread, 1, &oa, &ClientId);
	if (hThread)
	{
		ZwTerminateThread(hThread, 0);
		ZwClose(hThread);
	};
}