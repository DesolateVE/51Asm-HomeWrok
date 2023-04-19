.data
	szMenuChoose db "1. 调试进程",CRLF
	db "2. 帮助",CRLF
	db "3. 退出",CRLF,0
	szHelpInfo db "Help info test",CRLF,0
	AddressFormat db "%08X",0
	ByteFormat db "%02X",0
	CharFormat db "%c",0
	CharUnKnownFormat db ".",0
	TableFormat db " ",0
	TableEndFormat db "|",0
	NextLineFormat db CRLF,0
	RegFormat db "EAX  = %08X  EBX = %08X  ECX = %08X  EDX = %08X  ESI = %08X",CRLF
	db "EDI  = %08X  ESP = %08X  EBP = %08X  FS  = %08X  GS  = %08X",CRLF
	db "CS   = %08X  DS  = %08X  ES  = %08X  SS  = %08X  EIP = %08X",CRLF
	db "FLAG = %08X",CRLF,0
	BPTableTitleFormat db "Hint",09h,"Address",09h,09h,"PointType",CRLF,0
	BPTableInfoFormat db "%02X",09h,"%08X",09h,"%s",CRLF,0
	szBP_NORMAL db "TYPE_BROKE_NORMAL",0
	szBP_HARD db "TYPE_BROKE_HARD",0
	szBP_MEMORY db "TYPE_BROKE_MEMORY",0
	BPPageTableTitleFormat db "Page",09h,09h,"Length",09h,09h,"Property",CRLF,0
	BPPageTableInfoFormat db "%08X",09h,"%04X",09h,09h,"%08X",CRLF,0
	MoudleTitleNameForamt db "Name",0
	MoudleTitleBaseForamt db "Base",0
	MoudleNameFormat db "%-30S",0
	MoudleBaseFormat db "%p",CRLF,0
.code

PrintDisAsmEx proc address:dword,count:dword
	
	invoke MyReadProcessMemory,address,100
	invoke GetDisAsm,eax,address,100,100
	invoke PrintDisAsm,eax,count
	invoke FreeDisAsmInfo
	
	ret

PrintDisAsmEx endp

PrintBlank proc uses ebx num:dword
	mov ebx,0
	.while ebx < num
		invoke crt_printf,addr TableFormat
		inc ebx
	.endw
	ret
PrintBlank endp

MySetTextColor proc color:dword
	invoke GetStdHandle,STD_OUTPUT_HANDLE
	invoke SetConsoleTextAttribute,eax,color
	ret
MySetTextColor endp

PrintMenu proc
	invoke MySetTextColor,6
	invoke crt_printf,addr szMenuChoose
	invoke MySetTextColor,15
	ret
PrintMenu endp

PrintHelp proc
	invoke MySetTextColor,12
	invoke crt_printf,addr szHelpInfo
	ret
PrintHelp endp

PrintDisAsm proc uses ebx edx ecx pcs_insn:ptr cs_insn,count:dword
	LOCAL @times:dword
	LOCAL @length:dword
	
	mov esi,pcs_insn
	assume esi:ptr cs_insn
	
	invoke MySetTextColor,15
	
	mov ebx,0
	.while ebx < count
	
		;打印地址
		lea eax,[esi].address
		add eax,4
		mov eax,[eax]
		invoke crt_printf,addr AddressFormat,eax
		
		;格式化
		invoke PrintBlank,2
		invoke crt_printf,addr TableEndFormat
		invoke PrintBlank,2
		
		;打印代码
		lea eax,[esi].bytes
		xor edx,edx
		mov dx,word ptr [eax]
		mov @length,edx
		mov edx,0
		.while edx < @length
			lea eax,[esi].bytes
			add eax,2
			add eax,edx
			xor ecx,ecx
			mov cl,byte ptr [eax]
			
			;crt_printf会改变edx寄存器
			push edx
			invoke crt_printf,addr ByteFormat,ecx
			pop edx
			
			inc edx
		.endw
		
		;格式化
		mov eax,@length
		mov ecx,2
		mul ecx
		mov ecx,20
		sub ecx,eax		
		invoke PrintBlank,ecx
		invoke crt_printf,addr TableEndFormat
		invoke PrintBlank,2
		
		;打印指令前缀
		.if [esi].mnemonic[0] == 'p'
			invoke MySetTextColor,9
		.elseif [esi].mnemonic[0] == 'c' && [esi].mnemonic[1] == 'a'
			invoke MySetTextColor,47
		.elseif [esi].mnemonic[0] == 'm' || [esi].mnemonic[0] == 't'
			invoke MySetTextColor,11
		.elseif [esi].mnemonic[0] == 'j'
			invoke MySetTextColor,6
		.endif
		invoke crt_printf,addr [esi].mnemonic
		invoke MySetTextColor,15
		
		;格式化
		invoke crt_strlen,addr [esi].mnemonic
		mov ecx,8
		sub ecx,eax		
		invoke PrintBlank,ecx
		
		;打印操作数
		invoke crt_printf,addr [esi].op_str
		
		;换行
		invoke crt_printf,addr NextLineFormat
		
		add esi,size cs_insn
		inc ebx
	.endw
	
	assume esi:nothing
	
	ret

PrintDisAsm endp

PrintData proc uses ebx ecx edx address:dword
	LOCAL @Count:dword
	
	invoke MyReadProcessMemory,address,80h
	
	mov @Count,NULL
	xor ebx,ebx
	.while ebx < 8
		push ebx
		;打印地址
		invoke crt_printf,addr AddressFormat,address
		
		;格式化
		invoke PrintBlank,2
		invoke crt_printf,addr TableEndFormat
		invoke PrintBlank,2
		
		;打印字节
		xor ecx,ecx
		.while ecx < 10h
			push ecx
			mov edx,offset g_CodeBuffer
			xor eax,eax
			mov ebx,@Count
			mov al,byte ptr [edx+ebx]
			
			invoke crt_printf,addr ByteFormat,eax
			invoke PrintBlank,1
			
			inc @Count
			pop ecx
			inc ecx
		.endw
		
		;格式化
		invoke PrintBlank,1
		invoke crt_printf,addr TableEndFormat
		invoke PrintBlank,2
		
		;打印ASCII
		xor ecx,ecx
		.while ecx < 10h
			push ecx
			mov edx,offset g_CodeBuffer
			xor eax,eax
			mov ebx,@Count
			mov al,byte ptr [edx+ebx]
			
			.if al >= 32 && al <= 126
				invoke crt_printf,addr CharFormat,eax
			.else
				invoke crt_printf,addr CharUnKnownFormat
			.endif
			
			invoke PrintBlank,1
			
			inc @Count
			pop ecx
			inc ecx
		.endw
		
		;换行
		invoke crt_printf,addr NextLineFormat
		
		add address,10h
		pop ebx
		inc ebx
	.endw
	
	ret

PrintData endp

PrintReg proc
	LOCAL @ctx:CONTEXT
	LOCAL @hThread:HANDLE
	
	invoke OpenThread, THREAD_ALL_ACCESS, FALSE, g_dwThreadID
	mov @hThread,eax
	
	mov @ctx.ContextFlags, CONTEXT_FULL
    	invoke GetThreadContext, @hThread, addr @ctx
	
	invoke CloseHandle, @hThread
	invoke crt_printf,addr RegFormat,@ctx.regEax,@ctx.regEbx,@ctx.regEcx,@ctx.regEdx,@ctx.regEsi, \
		@ctx.regEdi,@ctx.regEsp,@ctx.regEbp,@ctx.regFs,@ctx.regGs, \
		@ctx.regCs,@ctx.regDs,@ctx.regEs,@ctx.regSs,@ctx.regEip, \
		@ctx.regFlag
	
	ret

PrintReg endp

PrintBrokePointPageTable proc uses esi ecx
	
	;遍历断点表
	invoke crt_printf,addr BPPageTableTitleFormat
	mov ecx,0
	.while ecx < 40
		push ecx
		.if g_pBreakPointArray[ecx*4] != NULL
			mov esi,g_pBreakPointArray[ecx*4]
			assume esi:ptr pt_broke
			.if [esi].cType == TYPE_BROKE_MEMORY
				invoke crt_printf,addr BPPageTableInfoFormat,[esi].Other.nPage,[esi].nSize,[esi].dwOldType
			.endif
			assume esi:nothing
		.endif
		pop ecx
		inc ecx
	.endw
	
	ret

PrintBrokePointPageTable endp

PrintBrokePointTable proc uses esi ecx BPType:dword
	LOCAL @pstr:dword
	
	;遍历断点表
	invoke crt_printf,addr BPTableTitleFormat
	mov ecx,0
	.while ecx < 40
		.if g_pBreakPointArray[ecx*4] != NULL
			mov esi,g_pBreakPointArray[ecx*4]
			assume esi:ptr pt_broke
			mov eax,BPType
			.if [esi].cType == eax
				.if BPType == TYPE_BROKE_NORMAL
					mov @pstr,offset szBP_NORMAL
				.elseif BPType == TYPE_BROKE_HARD
					mov @pstr,offset szBP_HARD
				.elseif BPType == TYPE_BROKE_MEMORY
					mov @pstr,offset szBP_MEMORY
				.endif
				push ecx
				invoke crt_printf,addr BPTableInfoFormat,ecx,[esi].pAddr,@pstr
				pop ecx
			.endif
			assume esi:nothing
		.endif
		inc ecx
	.endw
	ret

PrintBrokePointTable endp

PrintMoudleTable proc
	LOCAL @dwAddr:dword
	LOCAL @dwBegin:dword
	
	mov eax,g_Teb
	add eax,30h
	invoke MyReadProcessMemory,eax,4
	
	mov eax,dword ptr g_CodeBuffer[0]
	add eax,0Ch
	invoke MyReadProcessMemory,eax,4
	
	mov eax,dword ptr g_CodeBuffer[0]
	add eax,0Ch
	invoke MyReadProcessMemory,eax,4
	
	mov eax,dword ptr g_CodeBuffer[0]
	mov @dwBegin,eax
	mov @dwAddr,eax
	
	invoke crt_printf,addr MoudleTitleNameForamt
	invoke PrintBlank,26
	invoke crt_printf,addr MoudleTitleBaseForamt
	invoke crt_printf,addr NextLineFormat
	
	.repeat
		mov eax,@dwAddr
		add eax,30h
		invoke MyReadProcessMemory,eax,4
		mov eax,dword ptr g_CodeBuffer[0]
		invoke MyReadProcessMemory,eax,100
		
		invoke crt_wcslen,addr g_CodeBuffer
		.if eax != 0
			invoke MySetTextColor,11
			invoke crt_printf,addr MoudleNameFormat,addr g_CodeBuffer
			invoke MySetTextColor,15
			mov eax,@dwAddr
			add eax,18h
			invoke MyReadProcessMemory,eax,4
			mov eax,dword ptr g_CodeBuffer[0]
			invoke crt_printf,addr MoudleBaseFormat,eax
		.endif
	
		invoke MyReadProcessMemory,@dwAddr,4
		mov eax,dword ptr g_CodeBuffer[0]
		mov @dwAddr,eax
		mov eax,@dwBegin
	.until eax == @dwAddr
	
	ret

PrintMoudleTable endp

DoDumpEx proc
	LOCAL @stOF:OPENFILENAME
	
	invoke RtlZeroMemory,addr @stOF,sizeof @stOF
	mov @stOF.lStructSize,sizeof @stOF
	mov @stOF.nMaxFile,MAX_PATH
	mov @stOF.Flags,OFN_PATHMUSTEXIST
	mov @stOF.lpstrFile, offset szExePath
	invoke GetSaveFileName,addr @stOF
	
	mov eax,offset szExePath
	invoke DoDump,eax,g_hProcess,g_dwBaseOfImage
	invoke crt_printf,addr NextLineFormat
	
	ret

DoDumpEx endp