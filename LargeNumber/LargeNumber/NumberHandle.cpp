#include <stdio.h>
#include "NumberHandle.h"

void NumberHandle::Init_Number(NumberArr num)
{
	Large_Number[0] = num;
}

void NumberHandle::Large_Number_Add(NumberHandle NHandle)
{
	Number_Length = 15;
	if (NHandle.Number_SP > Number_SP)
	{
		Number_SP = NHandle.Number_SP;
	}
	for (long int i = 0; i < Number_SP; i++)
	{
		Large_Number[i] += NHandle.Large_Number[i];	
		if (Large_Number[i] >> 60)			//�Ե�61λ���ж��Ƿ������λ
		{
			Large_Number[i + 1] += 1;		//��λ
			Large_Number[i] <<= 4;		//����61λĨ��
			Large_Number[i] >>= 4;
			if (i == Number_SP - 1)		//�Ƿ���Ҫ�����µ�����
			{
				Number_SP++;
			}
		}
	}
}

void NumberHandle::Large_Number_Mul(NumberArr num)
{
	Number_Length = 10;
	NumberArr Temp = 0;							//��ʱ��λ�洢
	for (long int i = 0; i < Number_SP; i++)
	{
		Large_Number[i] *= num;
		if (Temp)
		{
			Large_Number[i] += Temp;
		}
		if (Temp = (Large_Number[i] >> 40))		//��ǰ24λ���ж��Ƿ���Ҫ��λ
		{
			Large_Number[i] <<= 24;				//��λ��Ĩ��
			Large_Number[i] >>= 24;
			if (i == Number_SP - 1)				//�Ƿ���Ҫ�����µ�����
			{
				Number_SP++;
			}
		}
	}
}

void NumberHandle::ShowNumber()
{
	printf("%I64X", Large_Number[Number_SP - 1]);
	for (long int i = Number_SP - 2; i >= 0; i--)
	{
		switch (Number_Length)
		{
		case 10:
			printf("%010I64X", Large_Number[i]);
			break;
		case 15:
			printf("%015I64X", Large_Number[i]);
			break;
		}	
	}
}

