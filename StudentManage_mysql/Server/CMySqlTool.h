#pragma once
#include "../Common/CUMTSocket.h"
#include "../CPackage.h"

CString EasyQueryTableCount(PCSTR table_name);
CString EasyQueryTable(PCSTR table_name, INT table_index);
CString EasyQueryTableFastInfo(PCSTR table_name);
CString EasyAddInfo(PCSTR table_name, PCSTR columns, PCSTR values);
CString EasyDeleteInfo(PCSTR table_name, INT PrimaryKey);
CString EasyUpdataInfo(PCSTR table_name, PCSTR expres, INT PrimaryKey);

CSQLData* RequestSQLData(CUMTSocket* pSocket, CString szSQL);
void OnlySendRequest(CUMTSocket* pSocket, CString szSQL);
void EasyCreateList(CSQLData* pData, CListCtrl* pList);
void EasyCreateCmbBox(CSQLData* pData, CComboBox* pBox);