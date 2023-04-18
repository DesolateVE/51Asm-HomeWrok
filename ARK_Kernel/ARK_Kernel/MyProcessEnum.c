#include "MyProcessEnum.h"

//���µ�������ͷ�ļ���ȫ��������
//NTKERNELAPI NTSTATUS PsLookupProcessByProcessId(HANDLE Id, PEPROCESS* Process);
//NTKERNELAPI NTSTATUS PsLookupThreadByThreadId(HANDLE Id, PETHREAD* Thread);
//NTKERNELAPI PEPROCESS IoThreadToProcess(PETHREAD Thread);
//NTKERNELAPI VOID NTAPI KeAttachProcess(PEPROCESS Process);										// �ѱ���̭��ʹ��KeStackAttachProcess
//NTKERNELAPI VOID NTAPI KeDetachProcess();
//NTKERNELAPI VOID NTAPI KeStackAttachProcess(PEPROCESS Process, PKAPC_STATE ApcState);
//NTKERNELAPI VOID NTAPI KeUnstackDetachProcess(PKAPC_STATE ApcState);

// ZwOpenThread = 0x12E			����Ҫ��SSDT��ȡ����ɵ���������
// ZwTerminateThread = 0x53
typedef NTSTATUS(*PFUNCTION_TERMINATE_THREAD)(HANDLE, DWORD);
NTSTATUS ZwTerminateThread(HANDLE hThread, DWORD dwExitCode)
{
	PFUNCTION_TERMINATE_THREAD pFun = (PFUNCTION_TERMINATE_THREAD)GetSSDTFunctionAddress(0x53);
	return pFun(hThread, dwExitCode);
}

// ���ݽ���ID���ؽ���EPROCESS��ʧ�ܷ���NULL
PEPROCESS LookupProcess(HANDLE Pid)
{
	PEPROCESS eprocess = NULL;
	if (NT_SUCCESS(PsLookupProcessByProcessId(Pid, &eprocess)))
		return eprocess;
	else
		return NULL;
}

// �����߳�ID�����߳�ETHREAD��ʧ�ܷ���NULL
PETHREAD LookupThread(HANDLE Tid)
{
	PETHREAD ethread;
	if (NT_SUCCESS(PsLookupThreadByThreadId(Tid, &ethread)))
		return ethread;
	else
		return NULL;
}

// ö��ָ�����̵��߳�
ULONG EnumThread(PEPROCESS Process, THREADENUM_CALLBACK pFun)
{
	PETHREAD ethrd = NULL;
	PEPROCESS eproc = NULL;
	ULONG nCount = 0;

	for (ULONG i = 4; i < 90000; i = i + 4) // һ����˵û�г���100000��PID��TID
	{
		ethrd = LookupThread((HANDLE)i);
		if (ethrd != NULL)
		{
			// ����߳���������
			eproc = IoThreadToProcess(ethrd);
			if (eproc == Process)
			{
				// ��ӡ��ETHREAD��TID
				// ״̬ = 0x184
				// �߳����ȼ� = 0x233
				// DbgPrint("[THREAD]ETHREAD=%p TID=%ld\n", ethrd, (ULONGLONG)PsGetThreadId(ethrd));
				pFun(ethrd);
				nCount++;
			}
			ObDereferenceObject(ethrd);
		}
	}
	return nCount;
}

// ö��ָ�����̵�ģ��
ULONG EnumMoudle(PEPROCESS Process, MODULEENUM_CALLBACK pFun)
{
	SIZE_T Peb = 0;
	SIZE_T Ldr = 0;
	PLIST_ENTRY ModListHead = NULL;
	PLIST_ENTRY Module = NULL;
	KAPC_STATE ks;
	ULONG nCount = 0;

	// EPROCESS��ַ��Ч���˳�
	if (!MmIsAddressValid(Process))
		return 0;
	// ��ȡPEB��ַ
	Peb = (SIZE_T)PsGetProcessPeb(Process);
	// PEB��ַ��Ч���˳�
	if (!Peb)
		return 0;
	// ��������
	KeStackAttachProcess(Process, &ks);
	__try
	{
		// ���LDR��ַ
		// offset peb.ldr = 0x18
		Ldr = Peb + 0x18;
		// �����Ƿ�ɶ������ɶ����׳��쳣�˳�
		ProbeForRead((CONST PVOID)Ldr, 8, 8);
		// �������ͷ
		// offset peb.ldr.InLoadOrderModuleList = 0x10
		ModListHead = (PLIST_ENTRY)(*(PULONG64)Ldr + 0x10);
		// �ٴβ��Կɶ���
		ProbeForRead((CONST PVOID)ModListHead, 8, 8);
		// ��õ�һ��ģ�����Ϣ
		Module = ModListHead->Flink;
		while (ModListHead != Module)
		{
			// ��ӡ��Ϣ����ַ����С��DLL·��
			//DbgPrint("[MODULE]Base=%p Size=%ld Path=%wZ\n",
			//	(PVOID)(((PLDR_DATA_TABLE_ENTRY)Module)->DllBase),
			//	(ULONG)(((PLDR_DATA_TABLE_ENTRY)Module)->SizeOfImage),
			//	&(((PLDR_DATA_TABLE_ENTRY)Module)->FullDllName));

			pFun((PLDR_DATA_TABLE_ENTRY)Module);
			nCount++;
			Module = Module->Flink;

			// ������һ��ģ����Ϣ�Ŀɶ���
			ProbeForRead((CONST PVOID)Module, 80, 8);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DbgPrint("[EnumModule]__except (EXCEPTION_EXECUTE_HANDLER)");
	}
	// ȡ����������
	KeUnstackDetachProcess(&ks);

	return nCount;
}

// ö�ٽ���
ULONG EnumProcess(PROCESSENUM_CALLBACK pFun)
{
	PEPROCESS eproc = NULL;
	ULONG nCount = 0;

	for (ULONG i = 4; i < 90000; i = i + 4) // һ����˵û�г���100000��PID��TID
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

// ��������
void ZwKillProcess(HANDLE Pid)
{
	HANDLE hProcess = NULL;
	CLIENT_ID ClientId;
	OBJECT_ATTRIBUTES oa;
	// ���CID
	ClientId.UniqueProcess = Pid; // �����޸�Ϊ��Ҫ��PID
	ClientId.UniqueThread = 0;
	// ���OA
	oa.Length = sizeof(oa);
	oa.RootDirectory = 0;
	oa.ObjectName = 0;
	oa.Attributes = 0;
	oa.SecurityDescriptor = 0;
	oa.SecurityQualityOfService = 0;
	// �򿪽��̣���������Ч�����������
	ZwOpenProcess(&hProcess, 1, &oa, &ClientId);
	if (hProcess)
	{
		ZwTerminateProcess(hProcess, 0);
		ZwClose(hProcess);
	};
}

// �����߳�
void ZwKillThread(HANDLE Tid)
{
	HANDLE hThread = NULL;
	CLIENT_ID ClientId;
	OBJECT_ATTRIBUTES oa;
	// ���CID
	ClientId.UniqueProcess = 0;
	ClientId.UniqueThread = Tid; // �����޸�Ϊ��Ҫ��TID
	// ���OA
	oa.Length = sizeof(oa);
	oa.RootDirectory = 0;
	oa.ObjectName = 0;
	oa.Attributes = 0;
	oa.SecurityDescriptor = 0;
	oa.SecurityQualityOfService = 0;

	// ���̣߳���������Ч�����������
	ZwOpenThread(&hThread, 1, &oa, &ClientId);
	if (hThread)
	{
		ZwTerminateThread(hThread, 0);
		ZwClose(hThread);
	};
}