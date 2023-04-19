TRUE equ 1
FALSE equ 0
ENDL equ 0dh,0ah

Exit macro
    MOV AH, 4CH
    INT 21H
endm

Contact struct
    m_enable db 1
    m_name db 16 dup(?)
    m_phone db 16 dup(?)
    m_address db 31 dup(?) 
Contact ends

; data
data segment
    g_szChoose db "[1]Add Contact",ENDL
    db "[2]Delete Contact",ENDL
    db "[3]Search Contact",ENDL
    db "[4]Update Contact",ENDL
    db "[5]Exit   Contact",ENDL,'$'
    g_szSearchChoose db "[1]Show All Contacts",ENDL
    db "[2]Search By ID",ENDL
    db "[3]Search By Name",ENDL,'$'
    g_szInputName db "Input Name:",ENDL,'$'
    g_szInputPhone db "Input Phone:",ENDL,'$'
    g_szInputAddress db "Input Address:",ENDL,'$'
    g_szInputSearchID db "Input Search ID",ENDL,'$'
    g_szInputSearchName db "Input Search Name",ENDL,'$'
    g_szFileTipSucess db "System:[Open File Sucess]",ENDL,'$'
    g_szFileTipFailed db "System:[Open File Failed]",ENDL,'$'
    g_szContactNumb db "Contacts Numb: ",'$'
    CRLF db ENDL,'$'
    g_szFile db "C:\\Contact.db",0
    g_hFile dw ?
    g_Count dw 0
    g_CurSelect dw 0
    g_Contact Contact<>
    g_StrBuffer db 32
    db ?
    db 32 dup(?)
    g_Enable db 0
data ends

; stack
stack segment stack
    db 255 dup(0)
stack ends

; code
code segment

printf proc c uses dx szSrc:word
    mov dx,szSrc
    mov ah,09h
    int 21h
    ret
printf endp

getchar proc c
    mov ah,01h
    int 21h
    ret
getchar endp

putchar proc c uses dx char:word
    mov ah,02h
    mov dx,char
    int 21h
    ret
putchar endp

printNumbByte proc c uses cx dx num:word
    mov dx,num				
	mov cl,04h
	shr dl,cl
    .IF dl >= 0ah
        add dl,07h
    .ENDIF
    add dl,30h
    invoke putchar,dl

    mov dx,num
    and dl,0fh
    .IF dl >= 0ah
        add dl,07h
    .ENDIF
    add dl,30h
    invoke putchar,dl
    ret
printNumbByte endp

printNumb proc c uses cx num:word
    mov cx,num
    invoke printNumbByte,ch
    invoke printNumbByte,cl
    ret
printNumb endp

StrLen PROC far c uses cx di sz:word
    mov cx, -1
    mov al, '$'
    mov di, sz
    repnz scasb
    mov ax, -1
    sub ax, cx
    dec ax
    ret
StrLen ENDP

Strstr PROC far c uses cx bx si di szSrc:word, szDst:word
    local @wSrcLen:word
    local @wDstLen:word

    invoke StrLen, szSrc
    mov @wSrcLen, ax
    invoke StrLen, szDst
    mov @wDstLen, ax

    .IF ax == 1
        MOV DI,szSrc
        MOV CX,@wSrcLen 
        mov bx, szDst
        MOV AL, [bx] 
        CLD 
AGN:    
        SCASB 
        JZ FIN 
        DEC CX 
        JNZ AGN 
        MOV AL,0 
        JMP OVR 
FIN:    
        MOV AL,0FFH   
OVR:    
        .IF al == 0
            jmp FAILED
        .ELSE
            jmp SUCESS
        .ENDIF
    .ENDIF


    cmp @wSrcLen, ax 
    jl FAILED
    mov si, szSrc
CMPLOOP:   
    mov di, szDst
    mov cx, @wDstLen
    repz cmpsb
    test cx, cx
    jz SUCESS
    inc si
    push si
    sub si, szSrc
    cmp si, @wSrcLen
    pop si
    jg FAILED
    jmp CMPLOOP
FAILED:
    xor ax, ax 
    ret
SUCESS:
    sub si, @wDstLen
    mov ax, si
    ret    
Strstr ENDP

setFilePoint proc c uses bx cx dx off:word
    mov bx,ds:[g_hFile]
    mov ah,42h
    mov cx,00h
    mov dx,off
    mov al,00h
    int 21h
    ret
setFilePoint endp

readFileByte proc c uses bx cx dx buf:word,count:word,off:word
    ; move file point
    invoke setFilePoint,off

    ; read to buffer
    mov bx,ds:[g_hFile]
    mov cx,count
    mov dx,buf
    mov ah,3fh
    int 21h
    ret
readFileByte endp 

writeFileByte proc c uses bx cx dx buf:word,count:word,off:word
    ; move file point
    invoke setFilePoint,off

    ; write from buffer
    mov bx,ds:[g_hFile]
    mov cx,count
    mov dx,buf
    mov ah,40h
    int 21h
    ret
writeFileByte endp

readCount proc c
    invoke readFileByte,offset g_Count,2,0
    ret
readCount endp

readInput proc c uses dx bx
    ; wait input
    mov dx,offset g_StrBuffer
    mov ah,0ah
    int 21h

    ; add '$' in str end
    mov bx,offset g_StrBuffer
    mov ax,0
    mov al,[bx+1]
    add bx,ax
    mov byte ptr [bx+2],'$'
    ret
readInput endp

readInputNumb proc c uses si bx cx dx
    local @ncount:word
    local @nlength:word
    local @char:word
    local @off:word

    invoke readInput
    
    mov @ncount,0
    mov ax,0
    lea bx,[offset g_StrBuffer + 1]
    mov al,[bx]
    mov @nlength,ax
    mov @off,ax
    add @off,1
    add @off,offset g_StrBuffer
    pushf
    std
    mov dx,0
    mov si,@off
    .WHILE dx < @nlength
        lodsb
        mov bl,al
        sub bl,'0'
        mov cx,dx
        mov ax,1
        push bx
        mov bl,10
        .WHILE cx > 0
            mul bl
            dec cx
        .ENDW 
        pop bx
        mul bl
        add @ncount,ax
        inc dx
    .ENDW
    popf
    mov ax,@ncount
    ret
readInputNumb endp

scanf proc c uses bx cx dx si di dest:word
    ; write in buffer
    invoke readInput

    mov bx,offset g_StrBuffer
    ; set si di first
    lea si,[bx+2]
    mov di,dest

    ; copy str to dest
    mov ax,0
    mov al,[bx+1]
    add ax,1
    mov cx,ax
    pushf
    cld
    rep movsb
    popf
    ret
scanf endp

printContactInfo proc c uses bx
    invoke putchar,'I'
    invoke putchar,'D'
    invoke putchar,':'
    invoke printNumb,ds:[g_CurSelect]
    invoke putchar,' '

    invoke putchar,'n'
    invoke putchar,':'
    lea bx,g_Contact.m_name
    invoke printf,bx
    invoke putchar,' '
                    
    invoke putchar,'p'
    invoke putchar,':'
    lea bx,g_Contact.m_phone
    invoke printf,bx
    invoke putchar,' '
                    
    invoke putchar,'a'
    invoke putchar,':'
    lea bx,g_Contact.m_address
    invoke printf,bx
    invoke printf,offset CRLF

    ret
printContactInfo endp

; start
start:
assume cs:code, ds:data, ss:stack
    mov ax,data
    mov ds,ax
    mov es,ax
    ; read or create file
    mov ah,3dh
    mov al,02h
    mov dx,offset g_szFile
    int 21h
    jnc Success
    invoke printf,offset g_szFileTipFailed
    Exit
Success:
    mov g_hFile,ax
    invoke printf,offset g_szFileTipSucess
    
    ; loop
    .WHILE TRUE

    ; read file head
    invoke readCount

    ; print contact numb
    invoke printf,offset g_szContactNumb
    invoke printNumb,g_Count
    invoke putchar,'-'
    invoke putchar,'>'
    invoke printNumb,g_CurSelect
    invoke printf,offset CRLF
    invoke printf,offset CRLF

    ; print Tip Info
    invoke printf,offset g_szChoose

    ; wait input
    invoke getchar

    ; new line
    push ax
    invoke printf,offset CRLF
    pop ax

    ; jump choose

    ; case Add
    .IF al == '1'
        ; input tip
        ; read input to buffer
        ; buffer to struct
        ; ... three times
        ; write struct to file
        ; contact count++ 
        ; curSelect = count
        mov g_Contact.m_enable , 1

        invoke printf,offset g_szInputName
        lea bx,g_Contact.m_name
        invoke scanf,bx
        invoke printf,bx
        invoke printf,offset CRLF

        invoke printf,offset g_szInputPhone
        lea bx,g_Contact.m_phone
        invoke scanf,bx
        invoke printf,bx
        invoke printf,offset CRLF

        invoke printf,offset g_szInputAddress
        lea bx,g_Contact.m_address
        invoke scanf,bx
        invoke printf,bx
        invoke printf,offset CRLF

        mov bx,size Contact
        mov ax,g_Count
        mul bl
        add ax,2
        inc g_Count
        push g_Count 
        pop g_CurSelect
        dec g_CurSelect
        mov cx,ax
    
        invoke writeFileByte,offset g_Count,2,0
        invoke writeFileByte,offset g_Contact,bx,cx
    .ENDIF

    ; case Del   
    .IF al == '2'
        ; write curselect.enable = false to file
        ; contact count--
        mov bx,size Contact
        mov ax,g_CurSelect
        ;dec g_Count
        dec g_CurSelect
        mul bl
        add ax,2
        mov cx,ax
        
        ;invoke writeFileByte,offset g_Count,2,0
        invoke writeFileByte,offset g_Enable,1,cx
    .ENDIF

     ; case Search
    .IF al == '3'
        ; Show All
        ; ID search
        ; Name search
        ; update curselect

        ; print tip info
        invoke printf,offset g_szSearchChoose
        ; wait input
        invoke getchar
        ; new line
        push ax
        invoke printf,offset CRLF
        pop ax

        ; seg init
        mov cx,0
        mov g_CurSelect,0

        ; case show all
        .IF al == '1'
            .WHILE cx < g_Count
                mov ax,size Contact
                mul g_CurSelect
                add ax,2

                ; read file
                invoke readFileByte,offset g_Contact,size Contact,ax
                .IF g_Contact.m_enable == 1
                    invoke printContactInfo
                .ENDIF
                inc g_CurSelect
                inc cx
            .ENDW
                dec g_CurSelect
        .ENDIF

        ; case id search
        .IF al == '2'
            invoke printf,offset g_szInputSearchID
            invoke readInputNumb
            mov bx,ax

            invoke printNumb,bx
            invoke printf,offset CRLF
            .IF bx < g_Count
                mov ax,size Contact
                mul bx
                add ax,2

                ; read file
                invoke readFileByte,offset g_Contact,size Contact,ax
                .IF g_Contact.m_enable == 1
                    mov g_CurSelect,bx
                    invoke printContactInfo
                .ENDIF
            .ENDIF
        .ENDIF

        ; case name search
        .IF al == '3'
            invoke printf,offset g_szInputSearchName
            invoke readInput

            .WHILE cx < g_Count
                mov ax,size Contact
                mul g_CurSelect
                add ax,2

                ; read file
                invoke readFileByte,offset g_Contact,size Contact,ax
                .IF g_Contact.m_enable == 1
                    invoke Strstr,offset g_Contact.m_name,offset g_StrBuffer + 2
                    .IF ax != 0
                        invoke printContactInfo
                    .ENDIF
                .ENDIF
                inc g_CurSelect
                inc cx
            .ENDW
                dec g_CurSelect
        .ENDIF
    .ENDIF

    ; case Update
    .IF al == '4'
        mov g_Contact.m_enable , 1

        invoke printf,offset g_szInputName
        lea bx,g_Contact.m_name
        invoke scanf,bx
        invoke printf,bx
        invoke printf,offset CRLF

        invoke printf,offset g_szInputPhone
        lea bx,g_Contact.m_phone
        invoke scanf,bx
        invoke printf,bx
        invoke printf,offset CRLF

        invoke printf,offset g_szInputAddress
        lea bx,g_Contact.m_address
        invoke scanf,bx
        invoke printf,bx
        invoke printf,offset CRLF

        mov ax,size Contact
        mul g_CurSelect
        add ax,2
        invoke writeFileByte,offset g_Contact,size Contact,ax
    .ENDIF

    ; return Dos
    .IF al == '5'
        Exit
    .ENDIF
   
    
        
    ; clear screen
    ;MOV AH,0FH
    ;INT 10H
    ;MOV AH,0
    ;INT 10H
    invoke printf,offset CRLF
    invoke printf,offset CRLF

    .ENDW

code ends
end start