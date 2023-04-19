.data
	
	
.code

GetEip proc
	LOCAL @ctx:CONTEXT
	LOCAL @hThread:HANDLE
	
	invoke OpenThread, THREAD_ALL_ACCESS, FALSE, g_dwThreadID
	mov @hThread,eax
	
	mov @ctx.ContextFlags, CONTEXT_FULL
    	invoke GetThreadContext, @hThread, addr @ctx
    	
    	invoke CloseHandle, @hThread
	mov eax,@ctx.regEip
	
	ret
GetEip endp

SetTF proc 
	LOCAL @ctx:CONTEXT
	LOCAL @hThread:HANDLE
	
	invoke OpenThread, THREAD_ALL_ACCESS, FALSE, g_dwThreadID
	mov @hThread,eax
	
	mov @ctx.ContextFlags, CONTEXT_FULL
    	invoke GetThreadContext, @hThread, addr @ctx
    	
    	or @ctx.regFlag, 100h
	invoke SetThreadContext, @hThread, addr @ctx
	invoke CloseHandle, @hThread
	
	ret
SetTF endp

DecEIP proc
	LOCAL @ctx:CONTEXT
	LOCAL @hThread:HANDLE
	
	invoke OpenThread, THREAD_ALL_ACCESS, FALSE, g_dwThreadID
	mov @hThread,eax
	
	mov @ctx.ContextFlags, CONTEXT_FULL
    	invoke GetThreadContext, @hThread, addr @ctx
    	
    	dec @ctx.regEip
	invoke SetThreadContext, @hThread, addr @ctx
	invoke CloseHandle, @hThread
	
	ret

DecEIP endp

RemoveBrokePointEx proc uses ecx esi Hint:dword
	mov ecx,Hint
	mov eax,4
	mul ecx
	
	.if g_pBreakPointArray[eax] != NULL
		mov esi,g_pBreakPointArray[eax]
		assume esi:ptr pt_broke
		
		push eax
		push esi
		
		.if [esi].cType == TYPE_BROKE_NORMAL		;һ��ϵ�
			xor eax,eax
			mov al,[esi].Other.oldCode
			mov g_CodeBuffer[0],al
			;��ԭ����
			invoke MyWriteProcessMomory,[esi].pAddr,1
			
			mov eax,g_CCResetAddr
			.if eax == [esi].pAddr
				mov g_CCResetAddr,NULL
			.endif

		.elseif [esi].cType == TYPE_BROKE_HARD		;Ӳ���ϵ�
		
			invoke RemoveOrSetHardPoint,[esi].Other.nDRNum,TRUE
			
			mov eax,g_nDRNum
			sub eax,10h
			.if [esi].Other.nDRNum == eax
				mov g_nDRNum,NULL
			.endif
			
		.elseif [esi].cType == TYPE_BROKE_MEMORY	;�ڴ�ϵ�
			invoke RemoveOrSetMemoryPoint,esi
			
			mov eax,g_MemHint
			sub eax,10h
			.if eax == Hint
				mov g_MemHint,NULL
			.endif
		.endif
		
		;�ͷ��ڴ�
		pop esi
		invoke crt_free,esi
		
		;ָ���ÿ�
		pop eax
		mov g_pBreakPointArray[eax],NULL

	.endif
	
	ret
RemoveBrokePointEx endp

AddBrokePoint proc uses ecx esi edi PointInfo:ptr pt_broke
	LOCAL @flag:dword
	
	mov @flag,FALSE
	mov ecx,0
	.while ecx < 40
		.if g_pBreakPointArray[ecx*4] == NULL
			;crt_malloc��ʹ��ecx�Ĵ���
			push ecx
			invoke crt_malloc,size pt_broke
			pop ecx
			mov dword ptr g_pBreakPointArray[ecx*4],eax
			invoke crt_memcpy,eax,PointInfo,size pt_broke
			mov @flag,TRUE
			.break
		.endif
		inc ecx
	.endw
	mov eax,@flag
	ret
AddBrokePoint endp

SetBrokePoint proc pos:dword,isOnce:dword
	LOCAL @ptinfo:pt_broke
	
	invoke MyReadProcessMemory,pos,1
	mov eax,pos
	mov @ptinfo.pAddr,eax
	mov eax,isOnce
	mov @ptinfo.isOnce,eax
	mov @ptinfo.cType,TYPE_BROKE_NORMAL
	mov al,g_CodeBuffer[0]
	mov @ptinfo.Other.oldCode,al
	
	;����ϵ��
	invoke AddBrokePoint,addr @ptinfo
	.if eax == FALSE
		;�ϵ����� TODO
		
		ret
	.endif
	
	;д��0xCC
	mov eax,offset g_CodeBuffer
	mov byte ptr [eax],0cch
	invoke MyWriteProcessMomory,pos,1
	
	ret
SetBrokePoint endp

SetHardBrokePoint proc uses ecx ebx esi edx pos:dword,len:dword,ctype:dword
	LOCAL @ptinfo:pt_broke
	LOCAL @hThread:HANDLE
	LOCAL @Dr7Flag:DWORD
	LOCAL @ctx:CONTEXT
	
	invoke OpenThread, THREAD_ALL_ACCESS, FALSE, g_dwThreadID
	mov @hThread,eax
	mov @ctx.ContextFlags,CONTEXT_DEBUG_REGISTERS
    	invoke GetThreadContext, @hThread, addr @ctx
	
	mov @Dr7Flag,NULL
	
	xor ecx,ecx
	.while ecx < 40
		mov esi,g_pBreakPointArray[ecx*4]
		.if esi != NULL
			assume esi:ptr pt_broke
		
			.if [esi].cType == TYPE_BROKE_HARD
				.if [esi].Other.nDRNum == 0
					or @Dr7Flag,1
				.elseif [esi].Other.nDRNum == 1
					or @Dr7Flag,4
				.elseif [esi].Other.nDRNum == 2
					or @Dr7Flag,16
				.elseif [esi].Other.nDRNum == 3
					or @Dr7Flag,64
				.endif
			.endif
			assume esi:nothing	
		.endif
		inc ecx
	.endw
	
	xor ecx,ecx
	mov eax,1
	mov ebx,4
	.while ecx < 4
		pushad
		
		.if !(eax & @Dr7Flag)
			
			push ecx			

			;Ӳ���ϵ��־λ
			or @ctx.iDr7,eax
			
			;���Ⱥ�����
			xor edx,edx
			or edx,len
			shl edx,2
			or edx,ctype
			mov eax,4
			push edx
			mul ecx
			pop edx
			add eax,10h
			mov cl,al
			shl edx,cl
			or @ctx.iDr7,edx
			
			;д���ַ
			lea eax,@ctx.iDr0
			pop ecx
			mov ebx,pos		
			mov dword ptr [eax+ecx*4],ebx
			
			;�������ݵ��ϵ��
			mov eax,pos
			mov @ptinfo.pAddr,eax
			mov eax,len
			mov @ptinfo.nSize,eax
			mov @ptinfo.cType,TYPE_BROKE_HARD
			mov @ptinfo.Other.nDRNum,ecx
			invoke AddBrokePoint,addr @ptinfo
			
			.break
		.endif
		
		popad
		mul ebx
		inc ecx
	.endw
	
	invoke SetThreadContext, @hThread, addr @ctx
	invoke CloseHandle, @hThread
	
	ret
SetHardBrokePoint endp

SetMemoryBrokePoint proc uses ebx pos:dword,len:dword,ctype:dword
	LOCAL @MemInfo:MEMORY_BASIC_INFORMATION
	LOCAL @SystemInfo:SYSTEM_INFO
	LOCAL @OldPageType:dword
	LOCAL @ptinfo:pt_broke
	
	invoke GetSystemInfo,addr @SystemInfo
	invoke VirtualQueryEx,g_hProcess,pos,addr @MemInfo,size MEMORY_BASIC_INFORMATION
	
	;�ж��ڴ��ַ�Ƿ񳬹���ҳ
	mov eax,pos
	add eax,len
	mov ebx,@MemInfo.BaseAddress
	add ebx,@SystemInfo.dwPageSize
	
	;������Χֱ�ӷ���
	.if eax >= ebx
		ret
	.endif
	
	;�趨�ڴ�ҳ����
	invoke VirtualProtectEx,g_hProcess,pos,len,ctype,addr @OldPageType
	
	;����ϵ���Ϣ
	mov eax,pos
	mov @ptinfo.pAddr,eax
	
	mov eax,len
	mov @ptinfo.nSize,eax
	
	mov eax,@OldPageType
	mov @ptinfo.dwOldType,eax
	
	mov eax,@MemInfo.BaseAddress
	mov @ptinfo.Other.nPage,eax
	
	mov @ptinfo.cType,TYPE_BROKE_MEMORY
	
	invoke AddBrokePoint,addr @ptinfo
	
	ret
SetMemoryBrokePoint endp

DebugLoop proc uses ebx
	LOCAL @si:STARTUPINFO
	LOCAL @pi:PROCESS_INFORMATION
	LOCAL @de:DEBUG_EVENT
	LOCAL @dwStatus:DWORD
	
	invoke RtlZeroMemory,addr @si,size @si
	invoke RtlZeroMemory,addr @pi,size @pi
	invoke RtlZeroMemory,addr @de,size @de
	
	invoke CreateProcess, NULL, addr szExePath, NULL, NULL, FALSE, \
	DEBUG_ONLY_THIS_PROCESS,\
	NULL, NULL,\
	addr @si,\
	addr @pi
	
	mov eax,@pi.hProcess
	mov g_hProcess,eax
	mov @dwStatus,DBG_CONTINUE
	
	;ѭ�����յ����¼�
	.while TRUE
	
		;DEBUG_EVENT�ṹ�����㣬���ȴ������¼�
		invoke RtlZeroMemory, addr @de, size @de
		invoke WaitForDebugEvent, addr @de, INFINITE
		
		;�����߳�ID
		mov eax,@de.dwThreadId
		mov g_dwThreadID,eax
		
		;�쳣����
		.if @de.dwDebugEventCode == EXCEPTION_DEBUG_EVENT
			invoke OnExceptionEvent,addr @de
			mov @dwStatus,eax
			
		;�̵߳����¼�
		.elseif @de.dwDebugEventCode == CREATE_THREAD_DEBUG_EVENT
		
		;���̳�ʼ�߳��¼�
		.elseif @de.dwDebugEventCode == CREATE_PROCESS_DEBUG_EVENT
			invoke OnProcessCreateEvent,addr @de
			mov @dwStatus,eax
			
		;�߳��˳�
		.elseif @de.dwDebugEventCode == EXIT_THREAD_DEBUG_EVENT
		
		;�����˳�
		.elseif @de.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT
		
		;DLL����
		.elseif @de.dwDebugEventCode == LOAD_DLL_DEBUG_EVENT
		
		;DLLж��
		.elseif @de.dwDebugEventCode == UNLOAD_DLL_DEBUG_EVENT
		
		;DebugString��Ϣ
		.elseif @de.dwDebugEventCode == OUTPUT_DEBUG_STRING_EVENT
		
		.endif
		
		.if g_bIsExit == TRUE
			ret
		.endif
		
		;�ύ�¼�����
		invoke ContinueDebugEvent, @de.dwProcessId, @de.dwThreadId, @dwStatus
		
	.endw
	
	ret
DebugLoop endp

RemoveOrSetMemoryPoint proc uses esi pPoint:ptr pt_broke
	LOCAL @OldPageType:dword
	
	mov esi,pPoint
	assume esi:ptr pt_broke
	
	pushad
	invoke VirtualProtectEx,g_hProcess,[esi].pAddr,[esi].nSize,[esi].dwOldType,addr @OldPageType
	popad
	
	mov eax,@OldPageType
	mov [esi].dwOldType,eax
	
	assume esi:ptr noting
	
	ret

RemoveOrSetMemoryPoint endp

RemoveOrSetHardPoint proc uses ecx edx nDRNum:dword,isRomove:dword
	LOCAL @ptinfo:pt_broke
	LOCAL @hThread:HANDLE
	LOCAL @Dr7Flag:DWORD
	LOCAL @ctx:CONTEXT
	
	invoke OpenThread, THREAD_ALL_ACCESS, FALSE, g_dwThreadID
	mov @hThread,eax
	mov @ctx.ContextFlags,CONTEXT_DEBUG_REGISTERS
    	invoke GetThreadContext, @hThread, addr @ctx
	
	.if isRomove == TRUE
		.if nDRNum == 0
			mov eax,0FFFFFFFCh
			and @ctx.iDr7,eax
		.elseif nDRNum == 1
			mov eax,0FFFFFFF3h
			and @ctx.iDr7,eax
		.elseif nDRNum == 2
			mov eax,0FFFFFFCFh
			and @ctx.iDr7,eax
		.elseif nDRNum == 3
			mov eax,0FFFFFF3Fh
			and @ctx.iDr7,eax	
		.endif
	.else
		.if nDRNum == 0
			mov eax,1
			or @ctx.iDr7,eax
		.elseif nDRNum == 1
			mov eax,4
			or @ctx.iDr7,eax
		.elseif nDRNum == 2
			mov eax,16
			or @ctx.iDr7,eax
		.elseif nDRNum == 3
			mov eax,64
			or @ctx.iDr7,eax	
		.endif	
	.endif
	
	invoke SetThreadContext, @hThread, addr @ctx
	invoke CloseHandle, @hThread
	
	ret

RemoveOrSetHardPoint endp

OnExceptionEvent proc uses esi ebx ecx edi pDE:ptr DEBUG_EVENT
	LOCAL @bIsSelfBreak:dword
	LOCAL @dwStatus:dword
	LOCAL @bIsCCStep:dword
	LOCAL @CurrentAddr:dword
	LOCAL @bIsHardPoint:dword
	LOCAL @bIsMemHit:dword
	LOCAL @nCount:dword
	LOCAL @EvaAddr:dword
	LOCAL @nDRNum:dword
	
	mov esi,pDE
	assume esi:ptr DEBUG_EVENT
	;��ȡ�쳣ֵ
	mov ebx,[esi].u.Exception.pExceptionRecord.ExceptionCode
	;��ȡ�ϵ㵥����ַ
	mov eax,[esi].u.Exception.pExceptionRecord.ExceptionAddress
	mov @CurrentAddr,eax
	;��ȡ�쳣���ʵ�ַ
	mov eax,[esi].u.Exception.pExceptionRecord.ExceptionInformation[4]
	mov @EvaAddr,eax
	
	mov @dwStatus,DBG_CONTINUE
	
	;�����쳣
	.if ebx == EXCEPTION_ACCESS_VIOLATION
		
		mov @bIsMemHit,FALSE
		
		;�����ϵ���Ƿ����ڴ�ϵ�
		mov @nCount,NULL
		.while @nCount < 40
			mov eax,@nCount
			mov eax,g_pBreakPointArray[eax*4]
			.if eax != NULL
				mov esi,eax
				assume esi:ptr pt_broke
				mov eax,@EvaAddr
				
				.if [esi].cType == TYPE_BROKE_MEMORY
					mov ebx,[esi].Other.nPage
					add ebx,1000h
					
					;�Ƿ��Ǹ�ҳ��Ķϵ�
					.if eax >= [esi].Other.nPage && eax <= ebx
						;�Ƿ��Ƕϵ�Ŀ��
						mov ebx,[esi].pAddr
						add ebx,[esi].nSize
						.if eax >= [esi].pAddr && eax <= ebx
							mov @bIsMemHit,TRUE
						.endif
						;�رնϵ�
						invoke RemoveOrSetMemoryPoint,esi	
						
						;�ؿ����
						mov eax,@nCount
						mov g_MemHint,eax
						add g_MemHint,10h
						invoke SetTF
						
						.break
					.endif
				.endif
			.endif
			inc @nCount
		.endw
		
		.if @bIsMemHit == TRUE
			;��ʾ�����
			invoke PrintDisAsmEx,@CurrentAddr,1
			
			;�л�������̨
			invoke EasyCommand
			mov @dwStatus,eax
		.else
			mov @dwStatus,DBG_CONTINUE
		.endif

	;�ϵ�	
	.elseif ebx == EXCEPTION_BREAKPOINT
	
		;�ֲ���־��ʼ��
		mov @bIsSelfBreak,FALSE
		mov @bIsCCStep,FALSE
		
		;�����ϵ��
		mov ecx,0
		.while ecx < 40
			.if g_pBreakPointArray[ecx*4] != NULL
				mov esi,g_pBreakPointArray[ecx*4]
				assume esi:ptr pt_broke
				mov eax,@CurrentAddr
				.if [esi].pAddr == eax
					;��ԭ����
					mov al,[esi].Other.oldCode
					mov g_CodeBuffer[0],al
					invoke MyWriteProcessMomory,@CurrentAddr,1
					;�Ƿ��ǵ����ϵ�
					mov eax,g_CCStepAddr
					.if [esi].pAddr == eax
						mov @bIsCCStep,TRUE
						;����ַ���
						mov g_CCStepAddr,NULL
					.endif
					;�����һ���Զϵ���ɾ��
					.if [esi].isOnce == TRUE
						;�ͷ��ڴ棬���ÿ�ָ��
						pushad
						invoke crt_free,esi
						popad
						mov g_pBreakPointArray[ecx*4],NULL
					.else	
						;�����һ���Զϵ���Ҫ����CC
						mov eax,[esi].pAddr
						mov g_CCResetAddr,eax
						invoke SetTF
					.endif
					mov @bIsSelfBreak,TRUE
					.break
				.endif
			.endif
			inc ecx
		.endw
		
		;�����Լ����쳣��������
		.if @bIsSelfBreak == FALSE
			mov eax,DBG_EXCEPTION_NOT_HANDLED
			ret
		.endif
	
		;��ʾ�����
		invoke PrintDisAsmEx,@CurrentAddr,1

		
		
		;����EIPָ��
		invoke DecEIP
		
		;�л�������̨
		invoke EasyCommand
		mov @dwStatus,eax
		
	;����	
	.elseif ebx == EXCEPTION_SINGLE_STEP
		
		mov @bIsHardPoint,FALSE
		
		;��������ϵ�
		.if g_CCResetAddr != NULL
			mov eax,offset g_CodeBuffer
			mov byte ptr [eax],0cch
			invoke MyWriteProcessMomory,g_CCResetAddr,1
			mov g_CCResetAddr,NULL
		.endif
		
		;����Ӳ���ϵ�
		.if g_nDRNum != NULL
			mov eax,g_nDRNum
			sub eax,10h
			invoke RemoveOrSetHardPoint,eax,FALSE
			mov g_nDRNum,NULL
		.endif
		
		;�����ڴ�ϵ�
		.if g_MemHint != NULL
			mov eax,g_MemHint
			sub eax,10h
			
			lea ebx,g_pBreakPointArray
			mov eax,[ebx+eax*4]
			
			.if eax != NULL
				invoke RemoveOrSetMemoryPoint,eax
			.endif
			
			mov g_MemHint,NULL
		.endif
		
		
		;�ж��Ƿ���Ӳ���ϵ�
		invoke GetDRNum
		.if eax != NULL
			sub eax,10h
			mov @nDRNum,eax
			
			xor ecx,ecx
			.while ecx < 40
				mov eax,g_pBreakPointArray[ecx*4]
				.if eax != NULL
					mov esi,eax
					assume esi:ptr pt_broke
				
					mov eax,@nDRNum
					.if [esi].cType == TYPE_BROKE_HARD && [esi].Other.nDRNum == eax
						;�رնϵ�
						invoke RemoveOrSetHardPoint,[esi].Other.nDRNum,TRUE
						mov eax,[esi].Other.nDRNum
						add eax,10h
					
						;����
						mov g_nDRNum,eax
						invoke SetTF
					
						;Ӳ���ϵ��־
						mov @bIsHardPoint,TRUE
						.break
					.endif
				.endif
			inc ecx
			.endw
			
		.endif
		
		
		;�Ƿ����������Ӳ���ϵ�
		.if g_bIsStep == TRUE || @bIsHardPoint == TRUE
			mov g_bIsStep,FALSE
			
			invoke PrintDisAsmEx,@CurrentAddr,1
			invoke EasyCommand
			
			mov @dwStatus,eax
		.endif
		
	.endif
	
	assume esi:nothing
	mov eax,@dwStatus
	
	ret

OnExceptionEvent endp

OnProcessCreateEvent proc uses esi pDE:ptr DEBUG_EVENT

	mov esi,pDE
	assume esi:ptr DEBUG_EVENT
	
	;��ڵ�����һ���Զϵ�
	mov eax,[esi].u.CreateProcessInfo.lpStartAddress
	invoke SetBrokePoint,eax,TRUE
	
	;����Teb
	mov eax,[esi].u.CreateProcessInfo.lpThreadLocalBase
	mov g_Teb,eax
	
	;����ģ���ַ
	mov eax,[esi].u.CreateProcessInfo.lpBaseOfImage
	mov g_dwBaseOfImage,eax
	
	assume esi:nothing

	mov eax,DBG_CONTINUE
	ret
	
OnProcessCreateEvent endp

MyReadProcessMemory proc uses ecx address:dword,csize:dword
	LOCAL @dwRead:dword
	
	invoke ReadProcessMemory,g_hProcess,address,addr g_CodeBuffer,csize,addr @dwRead
	mov eax,offset g_CodeBuffer
	ret
MyReadProcessMemory endp

MyWriteProcessMomory proc uses ecx address:dword,csize:dword
	LOCAL @dwWrite:dword
	
	invoke WriteProcessMemory,g_hProcess,address,addr g_CodeBuffer,csize,addr @dwWrite
	ret
MyWriteProcessMomory endp

GetDRNum proc
	LOCAL @ctx:CONTEXT
	LOCAL @hThread:HANDLE
	
	invoke OpenThread, THREAD_ALL_ACCESS, FALSE, g_dwThreadID
	mov @hThread,eax
	
	mov @ctx.ContextFlags, CONTEXT_DEBUG_REGISTERS
    	invoke GetThreadContext, @hThread, addr @ctx
	invoke CloseHandle, @hThread
	
	mov eax,@ctx.iDr6
	.if eax & 1
		mov eax,0
	.elseif eax & 2
		mov eax,1
	.elseif eax & 4
		mov eax,2
	.elseif eax & 8
		mov eax,3
	.elseif eax == 0
		mov eax,0
		ret
	.endif
	
	add eax,10h
	
	ret
GetDRNum endp