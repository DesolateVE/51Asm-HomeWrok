.586
.model flat, stdcall
.stack 4096
option casemap:none
 
include		windows.inc
include	    	user32.inc
include		kernel32.inc
include		msvcrt.inc
include		comdlg32.inc
include		w2k\ntdll.inc

includelib	kernel32.lib
includelib	user32.lib
includelib	msvcrt.lib
includelib	comdlg32.lib
;includelib	w2k\ntdll.lib	≥ÂÕª¡À
includelib	DisAsm.lib

include		Debuger.inc
include		OutPut.asm
include		InPut.asm
include		DebugEvent.asm

.code
start:
	invoke DisAsmEngineInit
	invoke PrintMenu
	invoke MenuSelect
	invoke DisAsmEngineDestory
end start
 
