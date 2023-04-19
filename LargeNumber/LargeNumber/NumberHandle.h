#pragma once

#define ArrMax 5000

//特大数处理

typedef unsigned long long NumberArr;

class NumberHandle
{
private:
	NumberArr		Large_Number[ArrMax]{};					//8位存储数组
	unsigned long		Number_SP = 1;							//数组使用大小指示器
	unsigned char		Number_Length;							//实际十六进制数组存储长度

public:
	void Init_Number(NumberArr num);
	void Large_Number_Add(NumberHandle NHandle);
	void Large_Number_Mul(NumberArr num);
	void ShowNumber();
};

