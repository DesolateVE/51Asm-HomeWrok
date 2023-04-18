#pragma once
#include <map>
#include <vector>
#include <string>
#include <windows.h>

using namespace std;

class CAsmEngine
{
public:
	CAsmEngine();
	~CAsmEngine();

public:
	byte ReadCode(byte* pCode);
	string GetDisAsm() const { return m_szDisAsm; };
	WORD GetAsmAddress() const { return m_AsmAddress; }
	void SetAsmAddress(WORD val) { m_AsmAddress = val; }

private:
	void ReadByte();
	void InitCodeMap();
	void mod_reg_rm(bool d = 1, bool w = 1);
	void mod_xxx_rm(bool w, bool s = 0);
	void ax_data();
	void vw_mod_rm();
	void repPrefix();
	void strOperation(bool w);
	void offset_seg();
	void EngineInit();
	void CrashCode0xFF();
	void CrashCode0x81();
	void CrashCode0xF7();

	string Readmod_rm(bool w = 1, bool ptr = 0);
	string Readaddr();
	string Readm16(bool sign = false);
	string Readm8(bool sign = false);
	string Readdisp(bool w);
	string Readmx(bool w, bool sign);

	map<byte, int> m_CodeIndex;
	vector<int> m_NameIndex;
	byte* m_pCurCode;
	string m_szDisAsm;
	WORD m_AsmAddress;

#ifdef _DEBUG
public:
	map<byte, byte> m_test;
	void PrintClashCode();
#endif
};

