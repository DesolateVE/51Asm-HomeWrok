#pragma once

#define WIN32_LEAN_AND_MEAN

#include "include/mysql.h"
#include <string>

struct CSQLData
{
	INT		nColCnt;	// ����
	INT		nRowCnt;	// ����
	BYTE*	cur_data;	// ָ��ǰ����������
	BYTE	Data[0];	// ������Ȼ�����������������У���NULL�ָ������ֽڴ�ų���

	CSQLData() = delete;	// ������ֱ�Ӵ�������

	// ��Ҫ�ֶ��ͷ��ڴ�
	static INT MakeSqlDataPack(MYSQL_RES* pRes, CSQLData** ppData)
	{
		CSQLData* pSQLData = (CSQLData*)malloc(0x500);
		*ppData = pSQLData;

		pSQLData->nColCnt = pRes->field_count;
		pSQLData->nRowCnt = pRes->row_count;
		pSQLData->cur_data = nullptr;
		BYTE* pCurPos = pSQLData->Data;

		// ������
		MYSQL_FIELD* fields = pRes->fields;
		for (int i = 0; i < pSQLData->nColCnt; i++)
		{
			int szlen = fields[i].name_length;
			*pCurPos = szlen;
			strcpy_s((char*)++pCurPos, szlen + 1, fields[i].name);
			pCurPos += szlen + 1;
		}

		// ������
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

	static char* unPackSqlData(CSQLData* pData)	// ����ȡ������
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