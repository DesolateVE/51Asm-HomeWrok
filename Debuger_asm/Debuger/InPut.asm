.data
szCommand db 256 dup(0)
szExePath db 256 dup(0)
szInputPre db ">> ",0
szErrorWarn db "�����ָ������",CRLF,0
szFileMode db "r",0
szFileModeSave db "w",0
szScriptFinish db "------------- Script End -------------",CRLF,0
szScriptSave db 256 dup(0)

.code
MenuSelect proc
	LOCAL @stOF:OPENFILENAME
	
	invoke RtlZeroMemory,addr @stOF,sizeof @stOF
	
	mov @stOF.lStructSize,sizeof @stOF
	mov @stOF.nMaxFile,MAX_PATH
	mov @stOF.Flags,OFN_PATHMUSTEXIST or OFN_FILEMUSTEXIST
	mov @stOF.lpstrFile, offset szExePath
	
	invoke crt_printf,addr NextLineFormat
	invoke crt_printf,addr szInputPre
	invoke crt_gets,addr szCommand
	
	;����
	invoke crt_printf,addr NextLineFormat
	invoke crt_printf,addr NextLineFormat
	invoke crt_printf,addr NextLineFormat
	
	;�������Խ���
	.if szCommand[0] == '1'
		;��ȡ�ļ�·��
		invoke GetOpenFileName,addr @stOF
		;ѭ�����������Ϣ
		invoke DebugLoop
	;��ʾ����	
	.elseif szCommand[0] == '2'
		invoke PrintHelp
	;�˳�����	
	.elseif szCommand[0] == '3'
		invoke ExitProcess,NULL	
	.endif
	ret
MenuSelect endp

EasyCommand proc uses esi
	LOCAL @pStrEnd:ptr byte
	LOCAL @CurrentAddr:dword
	
	;���浱ǰ��ַ
	invoke GetEip
	mov @CurrentAddr,eax
	
ReInput:
	
	invoke crt_printf,addr NextLineFormat
	invoke crt_printf,addr szInputPre
	;�Ƿ�ִ�нű�
	.if g_bScript == FALSE
		invoke crt_gets,addr szCommand
	.else
		invoke ReadOneCommand	
		.if eax
			invoke crt_printf,addr szScriptFinish
			jmp ReInput
		.endif
		invoke crt_printf,addr szCommand
	.endif
	invoke crt_printf,addr NextLineFormat
	
	.if szCommand[0] == 't'
	;��������
		invoke SaveOneCommand
			
		invoke SetTF
		mov g_bIsStep,TRUE
		
	.elseif szCommand[0] == 'p'
	;��������
		invoke SaveOneCommand
		
		invoke MyReadProcessMemory,@CurrentAddr,100
		invoke GetDisAsm,eax,@CurrentAddr,100,100
		mov esi,eax
		assume esi:ptr cs_insn
		;�Ƿ���CALL
		.if [esi].mnemonic[0] == 'c' && [esi].mnemonic[1] == 'a'
			;����һ���趨CC�ϵ�
			add esi,size cs_insn
			lea eax,[esi].address
			add eax,4
			mov eax,dword ptr[eax]
			push eax
			invoke SetBrokePoint,eax,TRUE
			pop eax
			;���浥��CC�ϵ�λ��
			mov g_CCStepAddr,eax
		.else
			invoke SetTF
			mov g_bIsStep,TRUE
		.endif
		
		invoke FreeDisAsmInfo
		
	.elseif szCommand[0] == 'g'
		invoke SaveOneCommand
		.if szCommand[1] == ' '
			;ִ�е�ָ��λ��
			invoke strtol,addr szCommand[2],addr @pStrEnd,16
			invoke SetBrokePoint,eax,TRUE
		.endif
		
	.elseif szCommand[0] == 'u'
		.if szCommand[1] == NULL
			;�����뵱ǰλ��
			invoke PrintDisAsmEx,@CurrentAddr,10
		.else		
			;������ָ��λ��		
			invoke strtol,addr szCommand[2],addr @pStrEnd,16
			invoke PrintDisAsmEx,eax,10
		.endif
		
		jmp ReInput
	
	.elseif szCommand[0] == 'd'
		
		.if szCommand[1] == 'd'		
			.if szCommand[2] == NULL
				;��ʾ��ǰ�ڴ�����
				invoke PrintData,@CurrentAddr
			.else
				;��ʾָ���ڴ�����
				invoke strtol,addr szCommand[3],addr @pStrEnd,16
				invoke PrintData,eax
			.endif
		.elseif szCommand[1] == 'u' && szCommand[2] == 'm' && szCommand[3] == 'p'
			;dmp����
			invoke DoDumpEx	
		.endif
		
		jmp ReInput
		
	.elseif szCommand[0] == 'r'
		;�Ĵ���	
		invoke PrintReg
		jmp ReInput
		
	.elseif szCommand[0] == 'e'
		.if szCommand[1] == ' '
			;�޸��ڴ�����
			invoke SaveOneCommand
			
			invoke CommandForSetMemory,addr szCommand[2]
			
		.elseif szCommand[1] == NULL
			
		
		.elseif szCommand[1] == 's'
			;�����ű�
			invoke SaveScriptFile
		.endif
		
		jmp ReInput
		
	.elseif szCommand[0] == 'b'
		.if szCommand[1] == 'p'
			.if szCommand[2] == ' '
				;һ��ϵ�
				invoke SaveOneCommand
				
				invoke strtol,addr szCommand[3],addr @pStrEnd,16
				invoke SetBrokePoint,eax,FALSE
				
			.elseif szCommand[2] == 'l'
				;һ��ϵ��б�
				invoke PrintBrokePointTable,TYPE_BROKE_NORMAL
				
			.elseif szCommand[2] == 'c' && szCommand[3] == ' '
				;ɾ��һ��ϵ�
				invoke SaveOneCommand
				
				invoke strtol,addr szCommand[4],addr @pStrEnd,16
				invoke RemoveBrokePointEx,eax
				
			.endif
			
			jmp ReInput
			
		.elseif szCommand[1] == 'h'
			.if szCommand[2] == ' '
				;Ӳ���ϵ�
				invoke SaveOneCommand
				
				invoke CommandForSetTypePoint,addr szCommand[3],TRUE
				
			.elseif szCommand[2] == 'l'
				;Ӳ���ϵ��б�
				invoke PrintBrokePointTable,TYPE_BROKE_HARD
				
			.elseif szCommand[2] == 'c' && szCommand[3] == ' '
				;ɾ��Ӳ���ϵ�
				invoke SaveOneCommand
				
				invoke strtol,addr szCommand[4],addr @pStrEnd,16
				invoke RemoveBrokePointEx,eax
				
			.endif
			
			jmp ReInput
		
		.elseif szCommand[1] == 'm'
			.if szCommand[2] == ' '
				;�ڴ�ϵ�
				invoke SaveOneCommand
				
				invoke CommandForSetTypePoint,addr szCommand[3],FALSE
			.elseif szCommand[2] == 'l'
				;�ڴ�ϵ��б�
				invoke PrintBrokePointTable,TYPE_BROKE_MEMORY
				
			.elseif szCommand[2] == 'p' && szCommand[3] == 'l'
				;��ҳ�ϵ��б�
				invoke PrintBrokePointPageTable
				
			.elseif szCommand[2] == 'c' && szCommand[3] == ' '
				;ɾ���ڴ�ϵ�
				invoke SaveOneCommand
				
				invoke strtol,addr szCommand[4],addr @pStrEnd,16
				invoke RemoveBrokePointEx,eax
			.endif
			
			jmp ReInput
			
		.endif
	 	
	.elseif szCommand[0] == 'l' && szCommand[1] == 's'
		;����ű�
		invoke ReadScriptFile
		
		jmp ReInput
	.elseif szCommand[0] == 'q'
		;�˳�����
		mov g_bIsExit,TRUE	
		
	.elseif szCommand[0] == 'm' && szCommand[1] == 'l'
		;�鿴ģ��
		invoke PrintMoudleTable
		
		jmp ReInput
	.else
		;����ָ����ʾ
		pushad
		invoke MySetTextColor,4
		invoke crt_printf,addr szErrorWarn
		invoke MySetTextColor,15
		popad
		
		;�ȴ���������
		jmp ReInput
	.endif
	
	mov eax,DBG_CONTINUE
	
	ret

EasyCommand endp

CommandForSetTypePoint proc uses ebx esi pCommand:ptr byte,IsHardOrMemory:dword
	LOCAL @address:dword
	LOCAL @length:dword
	LOCAL @type:dword
	LOCAL @pStrEnd:ptr byte
	
	invoke crt_strlen,pCommand
	mov ebx,eax
	xor eax,eax
	mov esi,pCommand
	.while eax < ebx
		pushad
		
		mov bl,byte ptr[esi+eax]
		.if bl == ' '
			mov byte ptr[esi+eax],NULL
		.endif
		
		popad
		inc eax
	.endw
	
	invoke strtol,pCommand,addr @pStrEnd,16
	mov @address,eax
	add @pStrEnd,1
	
	invoke strtol,@pStrEnd,addr @pStrEnd,16
	mov @length,eax
	.if IsHardOrMemory == TRUE
		sub @length,1
	.endif
	
	
	mov eax,@pStrEnd
	mov bl,byte ptr[eax+1]
	
	.if IsHardOrMemory == TRUE
		.if bl == 'e'
			mov @type,0		
		.elseif bl == 'w'
			mov @type,1		
		.elseif bl == 'a'
			mov @type,3		
		.endif
	.else
		.if bl == 'r'
			mov @type,PAGE_NOACCESS		
		.elseif bl == 'w'
			mov @type,PAGE_EXECUTE_READ				
		.endif
	.endif
	
	
	.if IsHardOrMemory == TRUE
		invoke SetHardBrokePoint,@address,@length,@type
	.else
		invoke SetMemoryBrokePoint,@address,@length,@type
	.endif
	
	ret

CommandForSetTypePoint endp

CommandForSetMemory proc uses ebx esi pCommand:ptr byte
	LOCAL @pStrEnd:ptr byte
	LOCAL @address:dword
	LOCAL @newData:dword
	
	pushad
	invoke crt_strlen,pCommand
	mov ebx,eax
	
	
	xor eax,eax
	mov esi,pCommand
	.while eax < ebx
		pushad
		
		mov bl,byte ptr[esi+eax]
		.if bl == ' '
			mov byte ptr[esi+eax],NULL
		.endif
		
		popad
		inc eax
	.endw
	
	invoke strtol,pCommand,addr @pStrEnd,16
	mov @address,eax
	add @pStrEnd,1
	
	invoke strtol,@pStrEnd,addr @pStrEnd,16
	mov @newData,eax
	
	lea eax,g_CodeBuffer
	mov ebx,@newData
	mov dword ptr [eax],ebx
	
	invoke MyWriteProcessMomory,@address,4
	popad
	
	ret
CommandForSetMemory endp

ReadScriptFile proc
	LOCAL @stOF:OPENFILENAME
	
	invoke RtlZeroMemory,addr @stOF,sizeof @stOF
	
	mov @stOF.lStructSize,sizeof @stOF
	mov @stOF.nMaxFile,MAX_PATH
	mov @stOF.Flags,OFN_PATHMUSTEXIST or OFN_FILEMUSTEXIST
	mov @stOF.lpstrFile, offset szExePath
	
	invoke GetOpenFileName,addr @stOF
	
	pushad
	invoke crt_fopen,addr szExePath,addr szFileMode
	.if eax != NULL
		mov g_pFILE,eax	
		mov g_bScript,TRUE
	.endif
	popad
	
	ret

ReadScriptFile endp

ReadOneCommand proc
	LOCAL @Flag:dword
	
	invoke crt_feof,g_pFILE
	mov @Flag,eax
	.if eax
		mov g_bScript,FALSE
		invoke crt_fclose,g_pFILE
		mov eax,@Flag
		ret
	.endif
	
	invoke crt_fgets,addr szCommand,256,g_pFILE
	mov eax,@Flag
	
	ret

ReadOneCommand endp

SaveOneCommand proc
	xor eax,eax
	mov al, szScriptSave[0]

	invoke crt_strcpy,addr szScriptSave[eax+1],addr szCommand
	invoke crt_strlen,addr szCommand
	add al,1
	add szScriptSave[0],al
	
	ret

SaveOneCommand endp

SaveScriptFile proc uses ebx
	LOCAL @stOF:OPENFILENAME
	LOCAL @EndPos:dword
	
	invoke RtlZeroMemory,addr @stOF,sizeof @stOF
	
	mov @stOF.lStructSize,sizeof @stOF
	mov @stOF.nMaxFile,MAX_PATH
	mov @stOF.Flags,OFN_PATHMUSTEXIST
	mov @stOF.lpstrFile, offset szExePath
	
	invoke GetSaveFileName,addr @stOF
	
	pushad
	invoke crt_fopen,addr szExePath,addr szFileModeSave
	mov g_pFILE,eax
	
	xor eax,eax
	mov al,byte ptr szScriptSave[0]
	mov @EndPos,eax
	lea eax,szScriptSave
	add @EndPos,eax
	
	lea ebx,szScriptSave[1]
	
	.while TRUE
		invoke crt_fputs,ebx,g_pFILE
		invoke crt_strlen,ebx
		add ebx,eax
		add ebx,1
		
		mov eax,@EndPos
		.if eax <= ebx
			.break
		.endif
		
		invoke crt_fputs,addr NextLineFormat,g_pFILE
	.endw
	
	invoke crt_fclose,g_pFILE
	mov g_pFILE,NULL
	
	popad
	
	ret

SaveScriptFile endp