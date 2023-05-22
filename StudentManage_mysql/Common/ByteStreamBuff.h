#pragma once
#include <vector>
using namespace std;

//简单的字节流缓冲区
class CByteStreamBuff
{
public:
	CByteStreamBuff();
	~CByteStreamBuff();

	//写入缓冲区, 附加在缓冲区末尾
	void Write(const char* pBuff, int nSize); 
	//从缓冲区读取指定字节数, 读取的数据自动从缓冲区删除
	void Read(char* pBuff, int nSize);
	//从缓冲区读取指定字节数, 读取的数据不会从缓冲区删除
	void Peek(char* pBuff, int nSize);
	//获取缓冲区中数据的大小
	int GetSize()const;
	
private:
	vector<char> m_vctBuff;
};

