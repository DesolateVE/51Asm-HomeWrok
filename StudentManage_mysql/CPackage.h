#pragma once

#define WIN32_LEAN_AND_MEAN

#include "include/mysql.h"
#include <string>

struct CSQLData
{
	INT		nColCnt;	// 列数
	INT		nRowCnt;	// 行数
	BYTE*	cur_data;	// 指向当前遍历的数据
	BYTE	Data[0];	// 列名，然后是行数据依次排列，用NULL分隔，首字节存放长度

	CSQLData() = delete;	// 不允许直接创建对象

	// 需要手动释放内存
	static INT MakeSqlDataPack(MYSQL_RES* pRes, CSQLData** ppData)
	{
		CSQLData* pSQLData = (CSQLData*)malloc(0x500);
		*ppData = pSQLData;

		pSQLData->nColCnt = pRes->field_count;
		pSQLData->nRowCnt = pRes->row_count;
		pSQLData->cur_data = nullptr;
		BYTE* pCurPos = pSQLData->Data;

		// 遍历列
		MYSQL_FIELD* fields = pRes->fields;
		for (int i = 0; i < pSQLData->nColCnt; i++)
		{
			int szlen = fields[i].name_length;
			*pCurPos = szlen;
			strcpy_s((char*)++pCurPos, szlen + 1, fields[i].name);
			pCurPos += szlen + 1;
		}

		// 遍历行
		MYSQL_ROWS* rows = pRes->data->data;
		for (int i = 0; i < pSQLData->nRowCnt; i++)
		{
			for (int k = 0; k < pSQLData->nColCnt; k++)
			{
				int szlen = strlen(rows->data[k]);
				*pCurPos = szlen;
				strcpy_s((char*)++pCurPos, szlen + 1, rows->data[k]);
				pCurPos += szlen + 1;
			}
			rows = rows->next;
		}

		return (DWORD_PTR)pCurPos - (DWORD_PTR)pSQLData;
	}

	static char* unPackSqlData(CSQLData* pData)	// 依次取出数据
	{
		if (pData->cur_data == nullptr)
		{
			pData->cur_data = pData->Data;
		}
		else
		{
			pData->cur_data += *(pData->cur_data) + 2;
		}
		return (char*)(pData->cur_data + 1);
	}
};