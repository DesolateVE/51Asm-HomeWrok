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
		
		.if [esi].cType == TYPE_BROKE_NORMAL		;一般断点
			xor eax,eax
			mov al,[esi].Other.oldCode
			mov g_CodeBuffer[0],al
			;还原数据
			invoke MyWriteProcessMomory,[esi].pAddr,1
			
			mov eax,g_CCResetAddr
			.if eax == [esi].pAddr
				mov g_CCResetAddr,NULL
			.endif

		.elseif [esi].cType == TYPE_BROKE_HARD		;硬件断点
		
			invoke RemoveOrSetHardPoint,[esi].Other.nDRNum,TRUE
			
			mov eax,g_nDRNum
			sub eax,10h
			.if [esi].Other.nDRNum == eax
				mov g_nDRNum,NULL
			.endif
			
		.elseif [esi].cType == TYPE_BROKE_MEMORY	;内存断点
			invoke RemoveOrSetMemoryPoint,esi
			
			mov eax,g_MemHint
			sub eax,10h
			.if eax == Hint
				mov g_MemHint,NULL
			.endif
		.endif
		
		;释放内存
		pop esi
		invoke crt_free,esi
		
		;指针置空
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
			;crt_malloc会使用ecx寄存器
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
	
	;加入断点表
	invoke AddBrokePoint,addr @ptinfo
	.if eax == FALSE
		;断点已满 TODO
		
		ret
	.endif
	
	;写入0xCC
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

			;硬件断点标志位
			or @ctx.iDr7,eax
			
			;长度和属性
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
			
			;写入地址
			lea eax,@ctx.iDr0
			pop ecx
			mov ebx,pos		
			mov dword ptr [eax+ecx*4],ebx
			
			;保存数据到断点表
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
	
	;判断内存地址是否超过分页
	mov eax,pos
	add eax,len
	mov ebx,@MemInfo.BaseAddress
	add ebx,@SystemInfo.dwPageSize
	
	;超出范围直接返回
	.if eax >= ebx
		ret
	.endif
	
	;设定内存页属性
	invoke VirtualProtectEx,g_hProcess,pos,len,ctype,addr @OldPageType
	
	;保存断点信息
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
	
	;循环接收调试事件
	.while TRUE
	
		;DEBUG_EVENT结构体置零，并等待调试事件
		invoke RtlZeroMemory, addr @de, size @de
		invoke WaitForDebugEvent, addr @de, INFINITE
		
		;保存线程ID
		mov eax,@de.dwThreadId
		mov g_dwThreadID,eax
		
		;异常处理
		.if @de.dwDebugEventCode == EXCEPTION_DEBUG_EVENT
			invoke OnExceptionEvent,addr @de
			mov @dwStatus,eax
			
		;线程调试事件
		.elseif @de.dwDebugEventCode == CREATE_THREAD_DEBUG_EVENT
		
		;进程初始线程事件
		.elseif @de.dwDebugEventCode == CREATE_PROCESS_DEBUG_EVENT
			invoke OnProcessCreateEvent,addr @de
			mov @dwStatus,eax
			
		;线程退出
		.elseif @de.dwDebugEventCode == EXIT_THREAD_DEBUG_EVENT
		
		;进程退出
		.elseif @de.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT
		
		;DLL加载
		.elseif @de.dwDebugEventCode == LOAD_DLL_DEBUG_EVENT
		
		;DLL卸载
		.elseif @de.dwDebugEventCode == UNLOAD_DLL_DEBUG_EVENT
		
		;DebugString消息
		.elseif @de.dwDebugEventCode == OUTPUT_DEBUG_STRING_EVENT
		
		.endif
		
		.if g_bIsExit == TRUE
			ret
		.endif
		
		;提交事件处理
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
	;获取异常值
	mov ebx,[esi].u.Exception.pExceptionRecord.ExceptionCode
	;获取断点单步地址
	mov eax,[esi].u.Exception.pExceptionRecord.ExceptionAddress
	mov @CurrentAddr,eax
	;获取异常访问地址
	mov eax,[esi].u.Exception.pExceptionRecord.ExceptionInformation[4]
	mov @EvaAddr,eax
	
	mov @dwStatus,DBG_CONTINUE
	
	;访问异常
	.if ebx == EXCEPTION_ACCESS_VIOLATION
		
		mov @bIsMemHit,FALSE
		
		;遍历断点表是否是内存断点
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
					
					;是否是该页面的断点
					.if eax >= [esi].Other.nPage && eax <= ebx
						;是否是断点目标
						mov ebx,[esi].pAddr
						add ebx,[esi].nSize
						.if eax >= [esi].pAddr && eax <= ebx
							mov @bIsMemHit,TRUE
						.endif
						;关闭断点
						invoke RemoveOrSetMemoryPoint,esi	
						
						;重开标记
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
			;显示反汇编
			invoke PrintDisAsmEx,@CurrentAddr,1
			
			;切换到控制台
			invoke EasyCommand
			mov @dwStatus,eax
		.else
			mov @dwStatus,DBG_CONTINUE
		.endif

	;断点	
	.elseif ebx == EXCEPTION_BREAKPOINT
	
		;局部标志初始化
		mov @bIsSelfBreak,FALSE
		mov @bIsCCStep,FALSE
		
		;遍历断点表
		mov ecx,0
		.while ecx < 40
			.if g_pBreakPointArray[ecx*4] != NULL
				mov esi,g_pBreakPointArray[ecx*4]
				assume esi:ptr pt_broke
				mov eax,@CurrentAddr
				.if [esi].pAddr == eax
					;还原代码
					mov al,[esi].Other.oldCode
					mov g_CodeBuffer[0],al
					invoke MyWriteProcessMomory,@CurrentAddr,1
					;是否是单步断点
					mov eax,g_CCStepAddr
					.if [esi].pAddr == eax
						mov @bIsCCStep,TRUE
						;将地址清空
						mov g_CCStepAddr,NULL
					.endif
					;如果是一次性断点则删除
					.if [esi].isOnce == TRUE
						;释放内存，并置空指针
						pushad
						invoke crt_free,esi
						popad
						mov g_pBreakPointArray[ecx*4],NULL
					.else	
						;如果非一次性断点需要重设CC
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
		
		;不是自己的异常不做处理
		.if @bIsSelfBreak == FALSE
			mov eax,DBG_EXCEPTION_NOT_HANDLED
			ret
		.endif
	
		;显示反汇编
		invoke PrintDisAsmEx,@CurrentAddr,1

		
		
		;回退EIP指针
		invoke DecEIP
		
		;切换到控制台
		invoke EasyCommand
		mov @dwStatus,eax
		
	;单步	
	.elseif ebx == EXCEPTION_SINGLE_STEP
		
		mov @bIsHardPoint,FALSE
		
		;重设软件断点
		.if g_CCResetAddr != NULL
			mov eax,offset g_CodeBuffer
			mov byte ptr [eax],0cch
			invoke MyWriteProcessMomory,g_CCResetAddr,1
			mov g_CCResetAddr,NULL
		.endif
		
		;重设硬件断点
		.if g_nDRNum != NULL
			mov eax,g_nDRNum
			sub eax,10h
			invoke RemoveOrSetHardPoint,eax,FALSE
			mov g_nDRNum,NULL
		.endif
		
		;重设内存断点
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
		
		
		;判断是否是硬件断点
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
						;关闭断点
						invoke RemoveOrSetHardPoint,[esi].Other.nDRNum,TRUE
						mov eax,[esi].Other.nDRNum
						add eax,10h
					
						;重设
						mov g_nDRNum,eax
						invoke SetTF
					
						;硬件断点标志
						mov @bIsHardPoint,TRUE
						.break
					.endif
				.endif
			inc ecx
			.endw
			
		.endif
		
		
		;是否是命令单步或硬件断点
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
	
	;入口点设置一次性断点
	mov eax,[esi].u.CreateProcessInfo.lpStartAddress
	invoke SetBrokePoint,eax,TRUE
	
	;保存Teb
	mov eax,[esi].u.CreateProcessInfo.lpThreadLocalBase
	mov g_Teb,eax
	
	;保存模块地址
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