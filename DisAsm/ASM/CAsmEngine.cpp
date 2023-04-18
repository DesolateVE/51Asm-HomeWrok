#include "CAsmEngine.h"

#include <assert.h>
#include <iostream>

union CodeType
{
	struct
	{
		byte w : 1;
		byte d : 1;
		byte : 6;
	}CodeType_dw;
	struct
	{
		byte reg : 3;
		byte w : 1;
		byte : 4;
	}CodeType_wr;
	struct
	{
		byte rm : 3;
		byte reg : 3;
		byte mod : 2;
	}mod_reg_rm;
	struct
	{
		byte : 3;
		byte reg : 2;
		byte : 3;
	}mod_sreg_rm;
};

static const char* c_szCodeIndex[] = {
"100010dw","1100011w","1011wr","1010000w","1010001w","10001110","10001100"," ",		// MOV
"11111111","01010r","000R110"," ",													// PUSH
"10001111","01011r","000R111"," ",													// POP
"1000011w","10010r"," ",															// XCHG
"1110010w","1110110w"," ",															// IN
"1110011w","1110111w"," ",															// OUT																	
"11010111"," ",																		// XLAT
"10001101"," ",																		// LEA 
"11000101"," ",																		// LDS
"11000100"," ",																		// LES
"10011111"," ",																		// LANF
"10011110"," ",																		// SANF
"10011100"," ",																		// PUSHF
"10011101"," ",																		// POPF
"000000dw","100000sw","0000010w"," ",												// ADD
"000100dw","100000sw","0001010w"," ",												// ADC
"1111111w","01000r"," ",															// INC
"00110111"," ",																		// AAA
"00100111"," ",																		// DAA
"001010dw","100000sw","0010110w"," ",												// SUB
"000110dw","100000sw","0001110w"," ",												// SBB
"1111111w","01001r"," ",															// DEC
"1111011w"," ",																		// NEG
"0011101w","0011100w","100000sw","0011110w"," ",									// CMP
"00111111"," ",																		// AAS
"00101111"," ",																		// DAS
"1111011w"," ",																		// MUL
"1111011w"," ",																		// IMUL
"11010100"," ",																		// AAM
"1111011w"," ",																		// DIV
"1111011w"," ",																		// IDIV
"11010101"," ",																		// AAD
"10011000"," ",																		// CBW
"10011001"," ",																		// CWD
"1111011w"," ",																		// NOT
"110100vw"," ",																		// SHL/SAL
"110100vw"," ",																		// SHR
"110100vw"," ",																		// SAR
"110100vw"," ",																		// ROL
"110100vw"," ",																		// ROR
"110100vw"," ",																		// RCL
"110100vw"," ",																		// RCR
"001000dw","1000000w","0010010w"," ",												// AND
"1000010w","1111011w","1010100w"," ",												// TEST
"000010dw","1000000w","0000110w"," ",												// OR
"001100dw","1000000w","0011010w"," ",												// XOR
"1111001z"," ",																		// REP
"1010010w"," ",																		// MOVS
"1010011w"," ",																		// CMPS
"1010111w"," ",																		// SCAS
"1010110w"," ",																		// LODS
"1010101w"," ",																		// STOS
"11101000","11111111","10011010","11111111"," ",									// CALL
"11101001","11101011","11111111","11101010","11111111"," ",							// JMP
"11000011","11000010"," ",															// RET
"11001011","11001010"," ",															// RETF
"01110100"," ",																		// JE/JZ
"01111100"," ",																		// JL/JNGE
"01111110"," ",																		// JLE/JNG
"01110010"," ",																		// JB/JNAE
"01110110"," ",																		// JBE/JNA
"01111010"," ",																		// JP/JPE
"01110000"," ",																		// JO
"01111000"," ",																		// JS
"01110101"," ",																		// JNE/JNZ
"01111101"," ",																		// JNL/JGE
"01111111"," ",																		// JNLE/JG
"01110011"," ",																		// JNB/JAE
"01110111"," ",																		// JNBE/JA
"01111011"," ",																		// JNP/JPO
"01110001"," ",																		// JNO
"01111001"," ",																		// JNS
"11100010"," ",																		// LOOP
"11100001"," ",																		// LOOPZ/LOOPE
"11100000"," ",																		// LOOPNZ/LOOPNE
"11100011"," ",																		// JCXZ
"11001101","11001100"," ",															// INT
"11001110"," ",																		// INTO
"11001111"," ",																		// IRET
"11111000"," ",																		// CLC
"11110101"," ",																		// CMC
"11111001"," ",																		// STC
"11111100"," ",																		// CLD
"11111101"," ",																		// STD
"11111010"," ",																		// CLI
"11111011"," ",																		// STI
"11110100"," ",																		// HLT
"10011011"," ",																		// WAIT
"11011xxx"," ",																		// ESC
"11110000"																			// LOCK
};

static const char* c_szOperation[] = {
"MOV","PUSH","POP","XCHG","IN","OUT","XLAT","LEA","LDS","LES","LANF","SANF","PUSHF","POPF","ADD","ADC",
"INC","AAA","DAA","SUB","SBB","DEC","NEG","CMP","AAS","DAS","MUL","IMUL","AAM","DIV","IDIV","AAD","CBW","CWD","NOT","SHL",
"SHR","SAR","ROL","ROR","RCL","RCR","AND","TEST","OR","XOR","REP","MOVS","CMPS","SCAS","LODS","STOS",
"CALL","JMP","RET","RETF","JE","JL","JLE","JB","JBE","JP","JO","JS","JNE","JNL","JNLE","JNB","JNBE","JNP","JNO","JNS","LOOP",
"LOOPZ","LOOPNZ","JCXZ","INT","INTO","IRET","CLC","CMC","STC","CLD","STD","CLI","STI","HLT","WAIT","ESC","LOCK"
};

static const char* c_szReg[] = {
"AL","AX",
"CL","CX",
"DL","DX",
"BL","BX",
"AH","SP",
"CH","BP",
"DH","SI",
"BH","DI"
};

static const char* c_szSegment[] = {
	"ES","CS","SS","DS"
};

static const char* c_szRegMemory[] = {
	"BX+SI","BX+DI","BP+SI","BP+DI","SI","DI","BP","BX"
};

static const char* c_szPtr[] = {
	"BYTE PTR","WORD PTR","DWORD PTR"
};

CAsmEngine::CAsmEngine()
{
	m_pCurCode = nullptr;
	SetAsmAddress(0x0100);
	m_szDisAsm = "Invoke ReadCode() First!";
	EngineInit();
}

CAsmEngine::~CAsmEngine()
{

}

void CAsmEngine::EngineInit()
{
	InitCodeMap();
}

void CAsmEngine::CrashCode0xFF()
{
	CodeType* pType = (CodeType*)(m_pCurCode);
	bool w = pType->CodeType_dw.w;
	bool ptr = false;
	byte reg = (pType + 1)->mod_reg_rm.reg;

	switch (reg)
	{
	case 0b110:
		m_szDisAsm = "PUSH\t";
		break;
	case 0b000:
		m_szDisAsm = "INC\t";
		ptr = true;
		break;
	case 0b001:
		m_szDisAsm = "DEC\t";
		ptr = true;
		break;
	case 0b010:
		m_szDisAsm = "CALL\t";
		break;
	case 0b011:
		m_szDisAsm = "CALL\tFAR ";
		break;
	case 0b100:
		m_szDisAsm = "JUMP\t";
		break;
	case 0b101:
		m_szDisAsm = "JUMP\tFAR ";
		break;
	}

	m_szDisAsm += Readmod_rm(w, ptr);
}

void CAsmEngine::CrashCode0x81()
{
	CodeType* pType = (CodeType*)(m_pCurCode);
	bool s = pType->CodeType_dw.d;
	bool w = pType->CodeType_dw.w;
	byte reg = (pType + 1)->mod_reg_rm.reg;

	switch (reg)
	{
	case 0b000:
		m_szDisAsm = "ADD";
		break;
	case 0b010:
		m_szDisAsm = "ADC";
		break;
	case 0b101:
		m_szDisAsm = "SUB";
		break;
	case 0b011:
		m_szDisAsm = "SBB";
		break;
	case 0b111:
		m_szDisAsm = "CMP";
		break;
	case 0b100:
		m_szDisAsm = "AND";
		break;
	case 0b001:
		m_szDisAsm = "OR";
		break;
	case 0b110:
		m_szDisAsm = "XOR";
		break;
	}

	m_szDisAsm += "\t";
	mod_xxx_rm(w, s);
}

void CAsmEngine::CrashCode0xF7()
{
	CodeType* pType = (CodeType*)(m_pCurCode);
	bool w = pType->CodeType_dw.w;
	byte reg = (pType + 1)->mod_reg_rm.reg;

	switch (reg)
	{
	case 0b011:
		m_szDisAsm = "NEG";
		break;
	case 0b100:
		m_szDisAsm = "MUL";
		break;
	case 0b101:
		m_szDisAsm = "IMUL";
		break;
	case 0b110:
		m_szDisAsm = "DIV";
		break;
	case 0b111:
		m_szDisAsm = "IDIV";
		break;
	case 0b010:
		m_szDisAsm = "NOT";
		break;
	case 0b000:
		m_szDisAsm = "TEST\t";
		mod_xxx_rm(w);
		return;
	}
	m_szDisAsm += "\t" + Readmod_rm(w, true);
}

byte CAsmEngine::ReadCode(byte* pCode)
{
	if (m_CodeIndex.find(*pCode) == m_CodeIndex.end())
	{
		m_szDisAsm = "UnKnown Code";
		return 1;
	}
	m_pCurCode = pCode;
	ReadByte();
	byte offset = ++m_pCurCode - pCode;
	m_AsmAddress += offset;
	return offset;
}

void CAsmEngine::ReadByte()
{
	int nCodeIndex = m_CodeIndex[*m_pCurCode];
	int nNameIndex = m_NameIndex[nCodeIndex];
	m_szDisAsm = c_szOperation[nNameIndex];
	m_szDisAsm += "\t";

	CodeType* pType = (CodeType*)m_pCurCode;
	switch (nCodeIndex)
	{
		// 冲突索引 0xFF
	case 7:		// PUSH
	case 33:	// INC
	case 43:	// DEC
	case 87:	// CALL
	case 89:	// CALL
	case 92:	// JMP
	case 94:	// JMP
		CrashCode0xFF();
		break;

		// 冲突索引 0x81
	case 28:	// ADD
	case 31:	// ADC
	case 38:	// SUB
	case 41:	// SBB
	case 48:	// CMP
	case 69:	// AND
	case 75:	// OR
	case 78:	// XOR
		CrashCode0x81();
		break;

		// 冲突索引 0xF7
	case 45:	// NEG
	case 52:	// MUL
	case 53:	// IMUL
	case 55:	// DIV
	case 56:	// IDIV
	case 60:	// NOT
	case 72:	// TEST
		CrashCode0xF7();
		break;

		// 冲突索引 0xD3
	case 61:	// SHL
	case 62:	// SHR
	case 63:	// SAR
	case 64:	// ROL
	case 65:	// ROR
	case 66:	// RCL
	case 67:	// RCR
		vw_mod_rm();
		break;

	case 1:		// MOV
		mod_xxx_rm(pType->CodeType_dw.w);
		break;
	case 2:		// MOV
		m_szDisAsm += c_szReg[pType->CodeType_wr.reg * 2 + pType->CodeType_wr.w] + string(",") + (pType->CodeType_wr.w ? Readm16() : Readm8());
		break;
	case 3:		// MOV
		m_szDisAsm += c_szReg[pType->CodeType_dw.w] + string(",") + Readaddr();
		break;
	case 4:		// MOV
		m_szDisAsm += Readaddr() + "," + c_szReg[pType->CodeType_dw.w];
		break;
	case 5:		// MOV
		m_szDisAsm += c_szSegment[(pType + 1)->mod_sreg_rm.reg] + string(",") + Readmod_rm();
		break;
	case 6:		// MOV
		m_szDisAsm += Readmod_rm() + "," + c_szSegment[(pType + 1)->mod_sreg_rm.reg];
		break;

	case 10:	// POP
		m_szDisAsm += Readmod_rm();
		break;

	case 8:		// PUSH
	case 11:	// POP
		m_szDisAsm += c_szReg[pType->CodeType_wr.reg * 2 + 1];
		break;

	case 9:		// PUSH
	case 12:	// POP
		m_szDisAsm += c_szSegment[pType->mod_sreg_rm.reg];
		break;

	case 13:	// XCHG
		mod_reg_rm(true, pType->CodeType_dw.w);
		break;
	case 14:	// XCHG
		m_szDisAsm += c_szReg[pType->CodeType_wr.reg * 2 + 1] + string(",") + c_szReg[1];
		break;

	case 15:	// IN
		m_szDisAsm += c_szReg[0] + string(",") + Readm8();
		break;
	case 16:	// IN
		m_szDisAsm += c_szReg[1] + string(",") + c_szReg[5];
		break;
	case 17:	// OUT
		m_szDisAsm += Readm8() + string(",") + c_szReg[0];
		break;
	case 18:	// OUT
		m_szDisAsm += c_szReg[5] + string(",") + c_szReg[1];
		break;

	case 20:	// LEA
	case 21:	// LDS
	case 22:	// LES
		mod_reg_rm();
		break;

	case 34:	// INC
	case 44:	// DEC
		m_szDisAsm += c_szReg[pType->CodeType_wr.reg * 2 + 1];
		break;

	case 0:		// MOV
	case 27:	// ADD
	case 30:	// ADC
	case 37:	// SUB
	case 40:	// SBB
	case 46:	// CMP
	case 47:	// CMP
	case 68:	// AND
	case 74:	// OR
	case 77:	// XOR
		mod_reg_rm(pType->CodeType_dw.d, pType->CodeType_dw.w);
		break;

	case 71:	// TEST
		mod_reg_rm(true, pType->CodeType_dw.w);
		break;

	case 80:	// REP
		repPrefix();
		break;
	case 81:	// MOVS
	case 82:	// CMPS
	case 83:	// SCAS
	case 84:	// LODS
	case 85:	// STOS
		strOperation(pType->CodeType_dw.w);
		break;
	case 86:	// CALL
	case 90:	// JMP
		m_szDisAsm += Readdisp(true);
		break;
	case 91:	// JMP
		m_szDisAsm += Readdisp(false);
		break;


	case 96:	// RET
	case 98:	// RETF
		m_szDisAsm += Readm16();
		break;

	case 119:	// INT
		m_szDisAsm += Readm8();
		break;

	case 132:	// ESC
		m_szDisAsm += c_szPtr[2] + string(" ") + Readmod_rm();
		break;

	case 88:	// CALL
	case 93:	// JMP
		offset_seg();
		break;

		// disp
	case 99:	// JE 
	case 100:	// JL 
	case 101:	// JLE 
	case 102:	// JB 
	case 103:	// JBE 
	case 104:	// JP 
	case 105:	// JO
	case 106:	// JS
	case 107:	// JNE 
	case 108:	// JNL 
	case 109:	// JNLE 
	case 110:	// JNB 
	case 111:	// JNBE 
	case 112:	// JNP 
	case 113:	// JNO
	case 114:	// JNS
	case 115:	// LOOP
	case 116:	// LOOPZ 
	case 117:	// LOOPNZ 
	case 118:	// JCXZ
		m_szDisAsm += Readdisp(false);
		break;

		// ax_data()
	case 29:	// ADD	
	case 32:	// ADC
	case 39:	// SUB
	case 42:	// SBB
	case 49:	// CMP
	case 70:	// AND
	case 73:	// TEST		
	case 76:	// OR
	case 79:	// XOR
		ax_data();
		break;

		// 需要指针移位
	case 54:	// AAM
	case 57:	// AAD
		++m_pCurCode;
		break;

		// 无需操作
	case 23:	// LANF
	case 24:	// SANF
	case 25:	// PUSHF
	case 26:	// POPF
	case 19:	// XLAT
	case 50:	// AAS
	case 51:	// DAS
	case 35:	// AAA
	case 36:	// DAA
	case 58:	// CBW
	case 59:	// CWD
	case 95:	// RET
	case 97:	// RETF
	case 120:	// INT
	case 121:	// INTO
	case 122:	// IRET
	case 123:	// CLC
	case 124:	// CMC
	case 125:	// STC
	case 126:	// CLD
	case 127:	// STD
	case 128:	// CLI
	case 129:	// STI
	case 130:	// HLT
	case 131:	// WAIT
	case 133:	// LOCK
		m_szDisAsm.pop_back();	// 把制表符"\t"弹出
		break;
	}
}

void CAsmEngine::mod_reg_rm(bool d /*= 1*/, bool w /*= 1*/)
{
	CodeType* pType = (CodeType*)(m_pCurCode + 1);
	int reg = pType->mod_reg_rm.reg;

	string sz_reg = c_szReg[reg * 2 + w];
	string sz_rm;
	string sz_Left;
	string sz_Right;

	sz_rm = Readmod_rm(w, false);

	if (d)
	{
		sz_Left = sz_reg;
		sz_Right = sz_rm;
	}
	else
	{
		sz_Left = sz_rm;
		sz_Right = sz_reg;
	}

	m_szDisAsm += sz_Left + "," + sz_Right;
}

void CAsmEngine::mod_xxx_rm(bool w, bool s /*= 0*/)
{
	string sz_rm;
	string sz_data;

	sz_rm = Readmod_rm(w, true);

	if (w && !s)
	{
		sz_data = Readm16();
	}
	else
	{
		sz_data = Readm8();
	}

	m_szDisAsm += sz_rm + "," + sz_data;
}

void CAsmEngine::ax_data()
{
	CodeType* pType = (CodeType*)(m_pCurCode);
	bool w = pType->CodeType_dw.w;

	string sz_reg = c_szReg[w];
	string sz_data = w ? Readm16() : Readm8();

	m_szDisAsm += sz_reg + "," + sz_data;
}

void CAsmEngine::vw_mod_rm()
{
	CodeType* pType = (CodeType*)(m_pCurCode);
	bool v = pType->CodeType_dw.d;
	bool w = pType->CodeType_dw.w;

	string sz_Operation;
	string sz_rm = Readmod_rm(w);

	switch ((++pType)->mod_reg_rm.reg)
	{
	case 0b100:
		sz_Operation = "SHL";
		break;
	case 0b101:
		sz_Operation = "SHR";
		break;
	case 0b111:
		sz_Operation = "SAR";
		break;
	case 0b000:
		sz_Operation = "ROL";
		break;
	case 0b001:
		sz_Operation = "ROR";
		break;
	case 0b010:
		sz_Operation = "RCL";
		break;
	case 0b011:
		sz_Operation = "RCR";
		break;
	}

	m_szDisAsm = sz_Operation + "\t" + sz_rm + ",";

	if (v)
	{
		m_szDisAsm += "CL";
	}
	else
	{
		m_szDisAsm += "1";
	}
}

void CAsmEngine::repPrefix()
{
	CodeType* pType = (CodeType*)(m_pCurCode);
	bool z = pType->CodeType_dw.w;

	string sz_prefix;
	if (z)
	{
		sz_prefix = "REP";
	}
	else
	{
		sz_prefix = "REPNZ";
	}
	
	++m_pCurCode;
	ReadByte();
	m_szDisAsm = sz_prefix + "\t" + m_szDisAsm;
}

void CAsmEngine::strOperation(bool w)
{
	m_szDisAsm.pop_back();

	if (w)
	{
		m_szDisAsm.push_back('W');
	}
	else
	{
		m_szDisAsm.push_back('B');
	}
}

void CAsmEngine::offset_seg()
{
	string sz_right = Readm16(false);
	string sz_left = Readm16(false);

	m_szDisAsm += sz_left + ":" + sz_right;
}

std::string CAsmEngine::Readmod_rm(bool w /*= 1*/, bool ptr /*= 0*/)
{
	CodeType* pType = (CodeType*)(++m_pCurCode);
	int mod = pType->mod_reg_rm.mod;
	int rm = pType->mod_reg_rm.rm;

	string sz_rm;

	switch (mod)
	{
	case 0:
		if (rm == 6)
		{
			sz_rm = Readm16();
		}
		else
		{
			sz_rm = c_szRegMemory[rm];
		}
		break;
	case 1:
		sz_rm = c_szRegMemory[rm];
		sz_rm += Readm8(true);
		break;
	case 2:
		sz_rm = c_szRegMemory[rm];
		sz_rm += Readm16(true);
		break;
	case 3:
		sz_rm = c_szReg[rm * 2 + w];
		break;
	}

	if (mod != 3)
	{
		sz_rm = "[" + sz_rm + "]";
		if (ptr)
		{
			sz_rm = string(c_szPtr[w]) + " " + sz_rm;
		}
	}

	return sz_rm;
}

std::string CAsmEngine::Readaddr()
{
	string sz_Rtn = Readm16();
	sz_Rtn = "[" + sz_Rtn + "]";

	return sz_Rtn;
}

std::string CAsmEngine::Readm16(bool sign /*= false*/)
{
	return Readmx(true, sign);
}

std::string CAsmEngine::Readm8(bool sign /*= false*/)
{
	return Readmx(false, sign);
}

std::string CAsmEngine::Readdisp(bool w)
{
	WORD disp;
	char sz_imm[8];

	if (w)
	{
		disp = *(WORD*)(m_pCurCode + 1);
		disp += 3;
	}
	else
	{
		disp = *(byte*)(m_pCurCode + 1);
		disp += 2;
	}
	disp += m_AsmAddress;

	sprintf_s(sz_imm, sizeof(sz_imm), "%04X", disp);

	m_pCurCode += 1 + w;
	return sz_imm;
}

std::string CAsmEngine::Readmx(bool w, bool sign)
{
	string szRtn;
	char sz_imm[8];
	int imm;

	if (w)
	{
		imm = *(short*)(m_pCurCode + 1);
	}
	else
	{
		imm = *(char*)(m_pCurCode + 1);
	}

	if (sign)
	{
		if (imm < 0 && imm > -90)
		{
			imm = -imm;
			szRtn = "-";
		}
		else
		{
			szRtn = "+";
		}
	}

	if (w)
	{
		sprintf_s(sz_imm, sizeof(sz_imm), "%04X", (WORD)imm);
	}
	else
	{
		sprintf_s(sz_imm, sizeof(sz_imm), "%02X", (BYTE)imm);
	}

	szRtn += sz_imm;
	m_pCurCode += 1 + w;

	return szRtn;
}

#ifdef _DEBUG
void CAsmEngine::PrintClashCode()
{
	for (auto var = m_test.begin(); var != m_test.end(); var++)
	{
		cout << "冲突编号：" << dec << (unsigned short)var->first << "\t冲突操作：" << c_szOperation[m_NameIndex[var->first]] << "\t冲突详细：0x" << hex << (unsigned short)var->second << endl;
	}

	for (WORD i = 0; i <= 0xFF; i++)
	{
		if (m_CodeIndex.find(i) == m_CodeIndex.end())
		{
			cout << "未使用字节：" << hex << (unsigned short)i << endl;
		}
	}
}
#endif // _DEBUG

void CAsmEngine::InitCodeMap()
{
	vector<int> EnableFlag;
	int NameID = 0;

	for (size_t n = 0; n < sizeof(c_szCodeIndex) / sizeof(char*); n++)
	{
		byte nCode = 0;			// 汇编码
		int nBitPos = 0;		// 当前位
		int w_flag = -1;		// w偏移
		int d_flag = -1;		// d偏移
		int r_flag = -1;		// reg偏移
		int R_flag = -1;		// seg-reg偏移
		EnableFlag.clear();		// 占位标志

		const char* szExp = c_szCodeIndex[n];
		if (*szExp == ' ')
		{
			++NameID;
			continue;
		}

		m_NameIndex.push_back(NameID);
		for (int i = strlen(szExp); i > 0; i--)
		{
			switch (*(szExp + i - 1))
			{
			case '1':
				nCode += 1 << nBitPos;
			case '0':
				++nBitPos;
				break;
			case 'w':
				w_flag = nBitPos;
				++nBitPos;
				break;
			case 'z':
			case 'v':
			case 's':
			case 'd':
				d_flag = nBitPos;
				++nBitPos;
				break;
			case 'x':
				i -= 2;
			case 'r':
				r_flag = nBitPos;
				nBitPos += 3;
				break;
			case 'R':
				R_flag = nBitPos;
				nBitPos += 2;
				break;
			}
		}

		if (w_flag >= 0)
		{
			EnableFlag.push_back(2);
			EnableFlag.push_back(w_flag);
		}
		if (d_flag >= 0)
		{
			EnableFlag.push_back(2);
			EnableFlag.push_back(d_flag);
		}
		if (r_flag >= 0)
		{
			EnableFlag.push_back(8);
			EnableFlag.push_back(r_flag);
		}
		if (R_flag >= 0)
		{
			EnableFlag.push_back(4);
			EnableFlag.push_back(R_flag);
		}

		assert(EnableFlag.size() / 2 <= 2);

		if (EnableFlag.size() == 0)
		{

#ifdef _DEBUG
			auto var = m_CodeIndex.find(nCode);
			if (var != m_CodeIndex.end())
			{
				m_test[var->second] = var->first;
				m_test[n - NameID] = nCode;
			}
#endif // _DEBUG

			m_CodeIndex[nCode] = n - NameID;
			continue;
		}

		if (EnableFlag.size() / 2 == 1)		// 如果再多一点变量就得换递归枚举了
		{
			for (byte i = 0; i < EnableFlag[0]; i++)
			{
				byte add = i << EnableFlag[1];

#ifdef _DEBUG
				auto var = m_CodeIndex.find(nCode + add);
				if (var != m_CodeIndex.end())
				{
					m_test[var->second] = var->first;
					m_test[n - NameID] = nCode + add;
				}
#endif // _DEBUG

				m_CodeIndex[nCode + add] = n - NameID;
			}
		}
		else
		{
			for (byte i = 0; i < EnableFlag[0]; i++)
			{
				for (byte k = 0; k < EnableFlag[2]; k++)
				{
					byte add1 = i << EnableFlag[1];
					byte add2 = k << EnableFlag[3];

#ifdef _DEBUG
					auto var = m_CodeIndex.find(nCode + add1 + add2);
					if (var != m_CodeIndex.end())
					{
						m_test[var->second] = var->first;
						m_test[n - NameID] = nCode + add1 + add2;
					}
#endif // _DEBUG

					m_CodeIndex[nCode + add1 + add2] = n - NameID;
				}
			}
		}
	}
}