#pragma once

#include <iostream>
#include <map>
#include <mutex>
#include <thread>
#include <winsock2.h>
#include <MSWSock.h>

#include "ByteStreamBuff.h"
#include "CCrc32.h"
#include "InitSockLib.h"

#pragma comment(lib,"Mswsock.lib")
#pragma  comment(lib,"ws2_32.lib") 

using namespace std;

#define MAXDATASIZE 1459

enum FLAG
{
	SYN = 0x01,
	ACK = 0x02,
	PSH = 0x04,
	FIN = 0x08
};

enum RequestType
{
	RT_ACCEPT,
	RT_RECV,
	RT_SEND
};

#pragma pack(push,1)
struct Package
{
	Package() {}
	Package(char flag, int seq, char* buf = nullptr, int len = 0) :
		m_chFlag(flag),
		m_nSeq(seq)
	{
		if (buf != nullptr)
		{
			m_nCheck = CCrc32::crc32((unsigned char*)buf, len);
			memcpy(m_buf, buf, len);
			m_nlen = len;
		}
	}

	char	m_chFlag;				// ��־
	int		m_nSeq;					// ���
	int		m_nCheck;				// У��ֵ
	int		m_nlen;					// ���ݳ���
	char	m_buf[MAXDATASIZE];		// ������
};

struct Request : public OVERLAPPED
{
	RequestType		m_rt;
	SOCKET			m_sock;
};

struct AcceptRequest : public Request
{
	AcceptRequest(SOCKET sock)
	{
		ZeroMemory(this, sizeof(*this));

		m_rt = RT_ACCEPT;
		m_sock = sock;
	}

	uint8_t m_bufOut[(sizeof(sockaddr) + 16) * 2] = {};
	DWORD m_dwOut;
};

struct RecvRequest : public Request
{
	RecvRequest(SOCKET sock)
	{
		ZeroMemory(this, sizeof(*this));

		m_rt = RT_RECV;
		m_sock = sock;
		m_wsabuf.len = sizeof(m_buf);
		m_wsabuf.buf = (CHAR*)m_buf;
	}

	uint8_t		m_buf[sizeof(Package)] = {};
	WSABUF		m_wsabuf;
	DWORD		m_dwFlag;
};

struct SendRequest :public Request
{
	SendRequest(SOCKET sock, uint8_t* sz)
	{
		ZeroMemory(this, sizeof(*this));

		m_rt = RT_SEND;
		m_sock = sock;
		memcpy((char*)m_buf, sz, sizeof(Package));
		m_wsabuf.len = sizeof(m_buf);
		m_wsabuf.buf = (CHAR*)m_buf;
	}

	uint8_t		m_buf[sizeof(Package)] = {};
	WSABUF		m_wsabuf;
	DWORD		m_dwFlag;
};

struct PackageInfo
{
	PackageInfo() {}
	PackageInfo(bool bSended, time_t tm, const Package& pkg) :
		m_bSended(bSended),
		m_tmLastTime(tm),
		m_pkg(pkg)
	{}

	bool m_bSended;
	time_t m_tmLastTime;
	Package m_pkg;
};
#pragma pack(pop)

class CUMTSocket
{
public:
	bool	Accept(const char* szIp, unsigned short wPort);
	bool	Connect(const char* szIp, unsigned short wPort);
	int		Recv(char* pBuff, size_t nBufSize);
	int		Send(char* pBuff, size_t nBufSize);
	void	Close();
	void	StratWork();
	BOOL	PostRecv(SOCKET sock);
	BOOL	PostSend(SOCKET sock, uint8_t* sz, sockaddr_in siDst);

private:
	SOCKET					m_sock;
	sockaddr_in				m_siSelf;			// �����ַ
	sockaddr_in				m_siDst;			// Ŀ���ַ
	int						m_nNextSendSeq = 0;	// ��һ������seq
	int						m_nNextRecvSeq = 0;	// ��һ���յİ���seq
	map<int, PackageInfo>	m_mpSend;			// ������map
	recursive_mutex			m_mtxForSend;		// ͬ����
	map<int, Package>		m_mpRecv;			// ���հ�map
	recursive_mutex			m_mtxForRecv;		// ͬ����
	CByteStreamBuff			m_buf;				// ���ݻ�����
	recursive_mutex			m_mtxForBuf;		// ͬ����
	bool					m_bRunning = false;
	HANDLE					m_hIocp;

private:
	void SendThreadProc();
	void RecvThreadProc();
	void HandleThreadProc();
	void OnRecv(Package& pkg);
};