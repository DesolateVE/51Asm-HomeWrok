#include "InitSockLib.h"

CInitSockLib::CInitSockLib()
{
	//��ʼ��
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return ;
	}
	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2) {
		WSACleanup();
		return ;
	}
}


CInitSockLib::~CInitSockLib()
{
	//����ʼ��
	WSACleanup();
}

//��̬��Ա,ʵ����һ������,�������ù��������,
//�Ӷ�ʵ�ֳ�ʼ��socket��ͷ���ʼ��
//ʡ�����������ط�ʵ��������
//CInitSockLib CInitSockLib::m_initsocklib;

