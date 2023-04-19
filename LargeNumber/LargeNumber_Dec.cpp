#include <stdio.h>
#include <iostream>

#define N 100
#define Max 100

using namespace std;

class LargeNumber
{
private:
	unsigned int num[Max]{ 0 };	//4位存储数组
	unsigned int temp[Max]{ 0 };	//进位值数组
	unsigned int factorial = 1;
	bool istempzero();
public:
	void MULValue(int n);
	void printnum();
	LargeNumber()
	{
		num[0] = 1;
	}
};

bool LargeNumber::istempzero()
{
	for (int i = 0; i < factorial; i++)
	{
		if (temp[i] != 0)
		{
			return false;
		}
	}
	return true;
}

void LargeNumber::MULValue(int n)
{
	for (size_t i = 0; i < factorial; i++)
	{
		num[i] *= n;
		if (num[i] > 9999)
		{
			temp[i] = num[i] / 10000;	//进位的值
			num[i] = num[i] % 10000;	//继续保留4位
			if (i == factorial - 1)	//最高4位溢出，则增长数组下标
			{
				factorial++;
			}
		}
	}
	while (!istempzero())
	{
		for (size_t i = 1; i < factorial; i++)
		{
			num[i] = num[i] + temp[i - 1];
			temp[i - 1] = 0;
			if (num[i] > 9999)
			{
				temp[i] = num[i] / 10000;	//进位的值
				num[i] = num[i] % 10000;	//继续保留4位
				if (i == factorial - 1)	//最高4位溢出，则增长数组下标
				{
					factorial++;
				}
			}
		}
	}
}

void LargeNumber::printnum()
{
	bool flag = false;
	bool first = false;
	for (int i = 0; i < Max; i++)
	{
		if (num[Max - i - 1] != 0)
		{
			flag = true;
		}
		if (flag)
		{
			if (!first)
			{
				printf("%d", num[Max - i - 1]);
				first = true;
			}
			else
			{
				printf("%04d", num[Max - i - 1]);
			}
		}
	}
}

int main()
{
	LargeNumber LN;
	for (int i = 1; i <= N; i++)
	{
		LN.MULValue(i);
	}
	LN.printnum();
	system("pause");
}
