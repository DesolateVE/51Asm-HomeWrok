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
		if (Large_Number[i] >> 60)			//以第61位来判断是否满足进位
		{
			Large_Number[i + 1] += 1;		//进位
			Large_Number[i] <<= 4;		//将第61位抹零
			Large_Number[i] >>= 4;
			if (i == Number_SP - 1)		//是否需要开辟新的数组
			{
				Number_SP++;
			}
		}
	}
}

void NumberHandle::Large_Number_Mul(NumberArr num)
{
	Number_Length = 10;
	NumberArr Temp = 0;							//临时进位存储
	for (long int i = 0; i < Number_SP; i++)
	{
		Large_Number[i] *= num;
		if (Temp)
		{
			Large_Number[i] += Temp;
		}
		if (Temp = (Large_Number[i] >> 40))		//以前24位来判断是否需要进位
		{
			Large_Number[i] <<= 24;				//进位区抹零
			Large_Number[i] >>= 24;
			if (i == Number_SP - 1)				//是否需要开辟新的数组
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

