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
		if (mysql_real_query(m_pMysql, m_szSQL, strlen(m_szSQL)))	// ����������ˣ��Ӹ��߳�������
		{
			cout << "ִ�����ʧ�ܣ������Ƿ�����߼�����" << endl;
		}
		else
		{
			cout << "����ִ�����\n-----------------------\n" << m_szSQL << "\n-----------------------\n";
		}
		LeaveCriticalSection(&g_csMySQLQuery);

		// ��ѯ����践�ؽ��������ֱ�ӷ���
		//if (!strncmp(m_szSQL, "SELECT", strlen("SELECT")))	// ���������������
		if (!strncmp(m_szSQL, "SELECT", 6))
		{
			MYSQL_RES* res = mysql_store_result(m_pMysql);
			if (res != nullptr)
			{
				CSQLData* pData;
				int nSzie = CSQLData::MakeSqlDataPack(res, &pData);
				m_pSocket->Send((char*)&nSzie, sizeof(int));
				m_pSocket->Send((char*)pData, nSzie);
				cout << "����[" << pData->nColCnt << "]��[" << pData->nRowCnt << "]��" << endl;
				free(pData);
				mysql_free_result(res);
			}
			else
			{
				cout << "���ش�����" << endl;
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
	MYSQL* sqlConn = mysql_init(nullptr);       // mysql��ʼ��������˳����socket��ʼ�����
	sqlConn = mysql_real_connect(
		sqlConn,				// ���ݿ���
		nullptr,				// ��������IP��ַ
		"root",					// MYSQL �û���
		"toor",					// MYSQL ����
		"secondgradeproject",   // ���ݿ�����
		3306,					// �˿ں�
		NULL,					// �׽��ֻ������ܵ�
		0);						// ��־ѡ�һ��Ϊ0
	mysql_set_character_set(sqlConn, "gbk");    // ���ñ���

	CUMTSocket sock;
	sock.Accept("127.0.0.1", 9530);
	cout << "�ѽ�������" << endl;

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

		//cout << "ִ�в�ѯ��� [" << DataBuff << "]" << endl;
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
		//		cout << "��Ч��䣬�����Ƿ�����߼�" << endl;
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
		//cout << "����" << pData->nColCnt << "��" << pData->nRowCnt << "��" << endl;
		//free(pData);
		//mysql_free_result(res);

		Sleep(1);
	}

	DeleteCriticalSection(&g_csMySQLQuery);
	mysql_close(sqlConn);
}