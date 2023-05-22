#include "pch.h"
#include "CMySqlTool.h"

#define PAGE_SIZE 10

CString EasyQueryTableCount(PCSTR table_name)
{
	CString szSql;
	szSql.Format("SELECT CEIL(COUNT(*) / %d) FROM %s", PAGE_SIZE, table_name);
	return szSql;
}

CString EasyQueryTable(PCSTR table_name, INT table_index)
{
	CString szSql;
	szSql.Format("SELECT * FROM %s LIMIT %d OFFSET %d", table_name, PAGE_SIZE, (table_index - 1) * PAGE_SIZE);
	return szSql;
}

CString EasyQueryTableFastInfo(PCSTR table_name)
{
	CString szSql;
	szSql.Format("SELECT %cid , %cname FROM %s", table_name[0], table_name[0], table_name);
	return szSql;
}

CString EasyAddInfo(PCSTR table_name, PCSTR columns, PCSTR values)
{
	CString szSql;
	szSql.Format("INSERT INTO %s(%s) VALUES(%s)", table_name, columns, values);
	return szSql;
}

CString EasyDeleteInfo(PCSTR table_name, INT PrimaryKey)
{
	CString szSql;
	szSql.Format("DELETE FROM %s WHERE %cid = %d", table_name, table_name[0], PrimaryKey);
	return szSql;
}

CString EasyUpdataInfo(PCSTR table_name, PCSTR expres, INT PrimaryKey)
{
	CString szSql;
	szSql.Format("UPDATE %s SET %s WHERE %cid = %d", table_name, expres, table_name[0], PrimaryKey);
	return szSql;
}

CSQLData* RequestSQLData(CUMTSocket* pSocket, CString szSQL)
{
	char* pBuf = (char*)malloc(0x200);
	int nBufLen = szSQL.GetLength() + 1;
	pSocket->Send((char*)&nBufLen, sizeof(int));
	pSocket->Send(szSQL.GetBuffer(), nBufLen);
	pSocket->Recv((char*)&nBufLen, sizeof(int));
	pSocket->Recv(pBuf, nBufLen);
	return (CSQLData*)pBuf;
}

void OnlySendRequest(CUMTSocket* pSocket, CString szSQL)
{
	int nBufLen = szSQL.GetLength() + 1;
	pSocket->Send((char*)&nBufLen, sizeof(int));
	pSocket->Send(szSQL.GetBuffer(), nBufLen);
}

void EasyCreateList(CSQLData* pData, CListCtrl* pList)
{
	// Delete all of the columns.
	pList->DeleteAllItems();
	int nColumnCount = pList->GetHeaderCtrl()->GetItemCount();
	for (int i = 0; i < nColumnCount; i++)
	{
		pList->DeleteColumn(0);
	}

	// 重新添加列
	for (size_t i = 0; i < pData->nColCnt; i++)
	{
		pList->InsertColumn(i, CSQLData::unPackSqlData(pData), LVCFMT_LEFT, 100);
	}

	// 依次添加数据
	for (size_t i = 0; i < pData->nRowCnt; i++)
	{
		pList->InsertItem(i, CSQLData::unPackSqlData(pData));
		for (size_t k = 1; k < pData->nColCnt; k++)
		{
			pList->SetItemText(i, k, CSQLData::unPackSqlData(pData));
		}
	}
}

// 约定好了，单表查询只返回ID和名称
void EasyCreateCmbBox(CSQLData* pData, CComboBox* pBox)
{
	// 清空数据
	pBox->ResetContent();

	// 跳过列名
	for (size_t i = 0; i < pData->nColCnt; i++)
	{
		CSQLData::unPackSqlData(pData);
	}

	// 添加数据
	for (size_t i = 0; i < pData->nRowCnt; i++)
	{
		INT ID = std::stoi(CSQLData::unPackSqlData(pData));
		pBox->AddString(CSQLData::unPackSqlData(pData));
		pBox->SetItemData(i, ID);
	}
}