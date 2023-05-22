#include <iostream>

#include "../CPackage.h"
#include "CUMTSocket.h"
#include "CThreadPool.h"

#pragma comment(lib, "libmysql.lib")
#pragma comment(lib, "CUMTSocket.lib")

using namespace std;

CRITICAL_SECTION g_csMySQLQuery;

class CSQLQuery : public ITask
{
public:
	CSQLQuery(CUMTSocket* Sock, MYSQL* Mysql, const char* szSQL)
	{
		m_pSocket = Sock;
		m_pMysql = Mysql;
		strcpy_s(m_szSQL, szSQL);
	}
public:
	void ExcuteTask() override
	{
		EnterCriticalSection(&g_csMySQLQuery);
		if (mysql_real_query(m_pMysql, m_szSQL, strlen(m_szSQL)))	// 这里出问题了（加个线程锁？）
		{
			cout << "执行语句失败，请检查是否存在逻辑错误" << endl;
		}
		else
		{
			cout << "正在执行命令：\n-----------------------\n" << m_szSQL << "\n-----------------------\n";
		}
		LeaveCriticalSection(&g_csMySQLQuery);

		// 查询语句需返回结果，其他直接返回
		//if (!strncmp(m_szSQL, "SELECT", strlen("SELECT")))	// 不是这句代码的问题
		if (!strncmp(m_szSQL, "SELECT", 6))
		{
			MYSQL_RES* res = mysql_store_result(m_pMysql);
			if (res != nullptr)
			{
				CSQLData* pData;
				int nSzie = CSQLData::MakeSqlDataPack(res, &pData);
				m_pSocket->Send((char*)&nSzie, sizeof(int));
				m_pSocket->Send((char*)pData, nSzie);
				cout << "返回[" << pData->nColCnt << "]列[" << pData->nRowCnt << "]行" << endl;
				free(pData);
				mysql_free_result(res);
			}
			else
			{
				cout << "返回错误结果" << endl;
				CSQLData* pZero = (CSQLData*)malloc(sizeof(CSQLData));
				pZero->nColCnt = NULL;
				pZero->nRowCnt = NULL;
				pZero->cur_data = nullptr;
				int nSzie = sizeof(CSQLData);
				m_pSocket->Send((char*)&nSzie, sizeof(int));
				m_pSocket->Send((char*)pZero, nSzie);
				free(pZero);
			}
		}
		else
		{
			return;
		}
	}
private:
	CUMTSocket* m_pSocket;
	MYSQL* m_pMysql;
	char m_szSQL[0x100];
};

int main()
{
	MYSQL* sqlConn = mysql_init(nullptr);       // mysql初始化函数会顺带把socket初始化完成
	sqlConn = mysql_real_connect(
		sqlConn,				// 数据库句柄
		nullptr,				// 主机名或IP地址
		"root",					// MYSQL 用户名
		"toor",					// MYSQL 密码
		"secondgradeproject",   // 数据库名称
		3306,					// 端口号
		NULL,					// 套接字或命名管道
		0);						// 标志选项，一般为0
	mysql_set_character_set(sqlConn, "gbk");    // 设置编码

	CUMTSocket sock;
	sock.Accept("127.0.0.1", 9530);
	cout << "已建立连接" << endl;

	CThreadPool pool;
	pool.CreateThreadPool(10);

	InitializeCriticalSection(&g_csMySQLQuery);

	char DataBuff[0x100];
	int nBufLen;
	while (true)
	{
		sock.Recv((char*)&nBufLen, sizeof(int));
		sock.Recv(DataBuff, nBufLen);

		pool.AddTask(new CSQLQuery(&sock, sqlConn, DataBuff));

		//cout << "执行查询语句 [" << DataBuff << "]" << endl;
		//int nRet = mysql_real_query(sqlConn, DataBuff, strlen(DataBuff));
		//MYSQL_RES* res = mysql_store_result(sqlConn);

		//if (res == nullptr)
		//{
		//	if (strncmp(DataBuff, "SELECT", strlen("SELECT")))
		//	{
		//		continue;
		//	}
		//	else
		//	{
		//		cout << "无效语句，请检查是否存在逻辑" << endl;
		//		CSQLData* pZero = (CSQLData*)malloc(sizeof(CSQLData));
		//		pZero->nColCnt = NULL;
		//		pZero->nRowCnt = NULL;
		//		pZero->cur_data = nullptr;
		//		int nSzie = sizeof(CSQLData);
		//		sock.Send((char*)&nSzie, sizeof(int));
		//		sock.Send((char*)pZero, nSzie);
		//		free(pZero);
		//	}
		//}

		//CSQLData* pData;
		//int nSzie = CSQLData::MakeSqlDataPack(res, &pData);
		//sock.Send((char*)&nSzie, sizeof(int));
		//sock.Send((char*)pData, nSzie);
		//cout << "返回" << pData->nColCnt << "列" << pData->nRowCnt << "行" << endl;
		//free(pData);
		//mysql_free_result(res);

		Sleep(1);
	}

	DeleteCriticalSection(&g_csMySQLQuery);
	mysql_close(sqlConn);
}