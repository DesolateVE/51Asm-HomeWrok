CRLF equ 0dh,0ah

;断点种类
TYPE_BROKE_NORMAL 	equ 0
TYPE_BROKE_HARD		equ 1
TYPE_BROKE_MEMORY	equ 2

cs_insn struc				;c++有结构体对齐，所以这里的大小要作相应调整
	id		dd ?		;编号
	address		dq ?		;地址
	csize   	dd ?		;大小
	bytes		db 18 dup(?)	;代码
	mnemonic	db 32 dup(?)	;前缀
	op_str		db 162 dup(?)	;操作数
	pcs_detail	dd ?		;详细
cs_insn ends

pt_info union
	oldCode		db ?	;断点以前值
	nPage		dd ?	;断点分页
	nDRNum		dd ?	;标记硬件断点位置
pt_info ends

pt_broke struc
	pAddr		dd ?	;地址
	isOnce		dd ?	;一次性
	nSize		dd ?	;断点尺寸
	cType		dd ?	;断点类型
	dwOldType	dd ?	;之前的页面属性
	Other		pt_info<>
pt_broke ends

DebugLoop proto								;异常消息循环
OnExceptionEvent proto pDE:ptr DEBUG_EVENT				;异常事件处理
OnProcessCreateEvent proto pDE:ptr DEBUG_EVENT				;进程创建事件

DisAsmEngineInit proto c						;反汇编引擎初始化
DisAsmEngineDestory proto c						;反汇编引擎析构
GetDisAsm proto c buff:dword,caddr:dword,csize:dword,count:dword	;反汇编
FreeDisAsmInfo proto c							;释放反汇编内存
DoDump proto c PathName:ptr byte,hprocess:HANDLE,dwBaseOfImage:dword	;Dump
DoDumpEx proto

GetDRNum proto
SetTF proto								;设置单步标志位
DecEIP proto								;EIP-1
GetEip proto								;获取当前EIP
MyReadProcessMemory proto address:dword,csize:dword			;读取内存
MyWriteProcessMomory proto address:dword,csize:dword			;写入内存
AddBrokePoint proto PointInfo:ptr pt_broke				;保存断点

SetStepInto proto							;设定步入
SetStepOver proto							;设定步过
SetBrokePoint proto pos:dword,isOnce:dword				;设置一般断点
SetHardBrokePoint proto pos:dword,len:dword,ctype:dword			;设置硬件断点
SetMemoryBrokePoint proto pos:dword,len:dword,ctype:dword		;设置内存断点

RemoveBrokePointEx proto Hint:dword					;删除断点，自动判断断点类型

PrintBlank proto num:dword						;打印空白区
PrintDisAsm proto pcs_insn:ptr cs_insn,count:dword			;打印反汇编
PrintDisAsmEx proto address:dword, count:dword				;打印反汇编Ex
PrintData proto address:dword						;打印数据
PrintReg proto								;打印寄存器
PrintBrokePointTable proto BPType:dword					;打印一般断点表
PrintMoudleTable proto

CommandForSetTypePoint proto pCommand:ptr byte,IsHardOrMemory:dword	;解析硬件和内存断点指令

RemoveOrSetHardPoint proto nDRNum:dword,isRomove:dword			;硬件断点开关
RemoveOrSetMemoryPoint proto pPoint:ptr pt_broke			;内存断点开关
CommandForSetMemory proto pCommand:ptr byte				;内存断点开关

ReadScriptFile proto
ReadOneCommand proto
SaveOneCommand proto
SaveScriptFile proto


.data
g_hProcess dd 0
g_dwThreadID dd 0
g_CodeBuffer db 200 dup(0)
g_pBreakPointArray dd 40 dup(0)
g_bIsExit dd 0
g_CCStepAddr dd 0					
g_bIsStep dd 0								;单步命令
g_nDRNum dd 0
g_MemHint dd 0	
g_CCResetAddr dd 0
g_pFILE dd 0
g_bScript dd 0								;脚本标志
g_Teb dd 0
g_dwBaseOfImage dd 0