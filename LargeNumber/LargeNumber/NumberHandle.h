#pragma once

#define ArrMax 5000

//�ش�������

typedef unsigned long long NumberArr;

class NumberHandle
{
private:
	NumberArr		Large_Number[ArrMax]{};					//8λ�洢����
	unsigned long		Number_SP = 1;							//����ʹ�ô�Сָʾ��
	unsigned char		Number_Length;							//ʵ��ʮ����������洢����

public:
	void Init_Number(NumberArr num);
	void Large_Number_Add(NumberHandle NHandle);
	void Large_Number_Mul(NumberArr num);
	void ShowNumber();
};

