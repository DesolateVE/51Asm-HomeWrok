#pragma once
#include <vector>
using namespace std;

//�򵥵��ֽ���������
class CByteStreamBuff
{
public:
	CByteStreamBuff();
	~CByteStreamBuff();

	//д�뻺����, �����ڻ�����ĩβ
	void Write(const char* pBuff, int nSize); 
	//�ӻ�������ȡָ���ֽ���, ��ȡ�������Զ��ӻ�����ɾ��
	void Read(char* pBuff, int nSize);
	//�ӻ�������ȡָ���ֽ���, ��ȡ�����ݲ���ӻ�����ɾ��
	void Peek(char* pBuff, int nSize);
	//��ȡ�����������ݵĴ�С
	int GetSize()const;
	
private:
	vector<char> m_vctBuff;
};

