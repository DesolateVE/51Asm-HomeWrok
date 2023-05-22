#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "CUMTSocket.h"
#include "time.h"

bool CUMTSocket::Accept(const char* szIp, unsigned short wPort)
{
	m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_sock == INVALID_SOCKET)
	{
		return false;
	}

	m_siSelf.sin_family = AF_INET;
	m_siSelf.sin_port = htons(wPort);
	m_siSelf.sin_addr.S_un.S_addr = inet_addr(szIp);
	int nRet = bind(m_sock, (sockaddr*)&m_siSelf, sizeof(m_siSelf));
	if (nRet == SOCKET_ERROR)
	{
		return false;
	}

	Package pkgRecv;
	int nLen = sizeof(m_siDst);
	nRet = recvfrom(m_sock, (char*)&pkgRecv, sizeof(pkgRecv), 0, (sockaddr*)&m_siDst, &nLen);
	if (nRet == SOCKET_ERROR)
	{
		return false;
	}

	// 检查标志
	if (!pkgRecv.m_chFlag & SYN)
	{
		return false;
	}
	m_nNextRecvSeq = pkgRecv.m_nSeq + 1;

	// 发送ACK
	Package pkgSend(ACK | SYN, m_nNextSendSeq++);
	nRet = sendto(m_sock, (char*)&pkgSend, sizeof(pkgSend), 0, (sockaddr*)&m_siDst, sizeof(m_siDst));

	// 接收第三个包
	nRet = recvfrom(m_sock, (char*)&pkgRecv, sizeof(pkgRecv), 0, (sockaddr*)&m_siDst, &nLen);
	m_nNextRecvSeq = pkgRecv.m_nSeq + 1;

	// 启动工作线程
	StratWork();

	return true;
}

bool CUMTSocket::Connect(const char* szIp, unsigned short wPort)
{
	m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_sock == INVALID_SOCKET)
	{
		return false;
	}

	m_siDst.sin_family = AF_INET;
	m_siDst.sin_port = htons(wPort);
	m_siDst.sin_addr.S_un.S_addr = inet_addr(szIp);

	// 发送ACK
	Package pkgSend(SYN, m_nNextSendSeq++);
	int nRet = sendto(m_sock, (char*)&pkgSend, sizeof(pkgSend), 0, (sockaddr*)&m_siDst, sizeof(m_siDst));

	Package pkgRecv;
	int nLen = sizeof(m_siDst);
	nRet = recvfrom(m_sock, (char*)&pkgRecv, sizeof(pkgRecv), 0, (sockaddr*)&m_siDst, &nLen);
	if (nRet == SOCKET_ERROR)
	{
		return false;
	}
	m_nNextRecvSeq = pkgRecv.m_nSeq + 1;

	// 发送第三个包
	Package pkgAck(ACK, m_nNextSendSeq++);
	nRet = sendto(m_sock, (char*)&pkgAck, sizeof(pkgAck), 0, (sockaddr*)&m_siDst, sizeof(m_siDst));

	// 启动工作线程
	StratWork();

	return true;
}

int CUMTSocket::Recv(char* pBuff, size_t nBufSize)
{
	while (true)
	{
		this_thread::sleep_for(chrono::microseconds(1));

		lock_guard<recursive_mutex> lck(m_mtxForBuf);
		if (m_buf.GetSize() <= 0)
		{
			continue;
		}
		else
		{
			break;
		}
	}

	// 接收数据
	lock_guard<recursive_mutex> lck(m_mtxForBuf);

	size_t nAvailSize = nBufSize >= m_buf.GetSize() ? m_buf.GetSize() : nBufSize;
	m_buf.Read(pBuff, nAvailSize);

	return nAvailSize;
}

int CUMTSocket::Send(char* pBuff, size_t nBufSize)
{
	while (true)
	{
		this_thread::sleep_for(chrono::microseconds(1));

		lock_guard<recursive_mutex> lck(m_mtxForSend);
		if (m_mpSend.size() > 300)
		{
			continue;
		}
		else
		{
			break;
		}
	}

	size_t nCnt = (nBufSize % MAXDATASIZE) == 0 ? (nBufSize / MAXDATASIZE) : (nBufSize / MAXDATASIZE + 1);

	lock_guard<recursive_mutex> lck(m_mtxForSend);
	for (size_t i = 0; i < nCnt; i++)
	{
		if (i == nCnt - 1)
		{
			m_mpSend[m_nNextSendSeq] = PackageInfo(false, time(NULL), Package(PSH, m_nNextSendSeq, pBuff + i * MAXDATASIZE, nBufSize - i * MAXDATASIZE));
			++m_nNextSendSeq;
			break;
		}
		m_mpSend[m_nNextSendSeq] = PackageInfo(false, time(NULL), Package(PSH, m_nNextSendSeq, pBuff + i * MAXDATASIZE, MAXDATASIZE));
		++m_nNextSendSeq;
	}

	return nBufSize;
}

void CUMTSocket::Close()
{
	//TODO：四次挥手

	closesocket(m_sock);
}

void CUMTSocket::StratWork()
{
	m_bRunning = true;

	//创建IOCP对象
	m_hIocp = CreateIoCompletionPort(
		INVALID_HANDLE_VALUE,	// 创建对象时必须填INVALID_HANDLE_VALUE
		NULL,					// 创建对象时必须填NULL
		NULL,					// 用户自定义参数
		0						// 系统自动计算
	);

	//关联IOCP和socket
	HANDLE hRet = CreateIoCompletionPort(
		(HANDLE)m_sock,		// 被关联的socket
		m_hIocp,			// 关联的iocp对象
		NULL,				// 与被关联的socket相关的自定义参数
		0);

	for (size_t i = 0; i < 100; i++)
	{
		PostRecv(m_sock);
	}

	thread(&CUMTSocket::RecvThreadProc, this).detach();		// 收包线程
	thread(&CUMTSocket::SendThreadProc, this).detach();		// 发包线程
	thread(&CUMTSocket::HandleThreadProc, this).detach();	// 缓冲区线程
}

BOOL CUMTSocket::PostRecv(SOCKET sock)
{
	RecvRequest* pRR = new RecvRequest(sock);
	int nRet = WSARecvFrom(sock,
		&pRR->m_wsabuf,
		1,
		NULL,
		&pRR->m_dwFlag,
		NULL,
		NULL,
		pRR,
		NULL
	);

	return TRUE;
}

BOOL CUMTSocket::PostSend(SOCKET sock, uint8_t* sz, sockaddr_in siDst)
{
	SendRequest* pRR = new SendRequest(sock, sz);
	int nRet = WSASendTo(sock,
		&pRR->m_wsabuf,
		1,
		NULL,
		pRR->m_dwFlag,
		(sockaddr*)&siDst,
		sizeof(siDst),
		pRR,
		NULL
	);

	return TRUE;
}

void CUMTSocket::SendThreadProc()
{
	while (m_bRunning)
	{
		this_thread::sleep_for(chrono::microseconds(1));	// 让出资源

		lock_guard<recursive_mutex> lck(m_mtxForSend);
		for (auto& pi : m_mpSend)
		{
			if (!pi.second.m_bSended)
			{
				PostSend(m_sock, (uint8_t*)&pi.second.m_pkg, m_siDst);
				//sendto(m_sock, (char*)&pi.second.m_pkg, sizeof(pi.second.m_pkg), 0, (sockaddr*)&m_siDst, sizeof(m_siDst));
				pi.second.m_tmLastTime = time(NULL);
				pi.second.m_bSended = true;
			}
			else
			{
				// 判断是否超时
				if (time(NULL) - pi.second.m_tmLastTime > 2)
				{
					PostSend(m_sock, (uint8_t*)&pi.second.m_pkg, m_siDst);
					//sendto(m_sock, (char*)&pi.second.m_pkg, sizeof(pi.second.m_pkg), 0, (sockaddr*)&m_siDst, sizeof(m_siDst));
					pi.second.m_tmLastTime = time(NULL);
				}
			}
		}
	}
}

void CUMTSocket::RecvThreadProc()
{
	while (m_bRunning)
	{
		//从队列中获取完成的请求
		DWORD dwNumberOfBytes = 0;
		ULONG_PTR dwKey = 0; //自定义参数
		Request* pOV = NULL;
		BOOL bRet = GetQueuedCompletionStatus(m_hIocp, &dwNumberOfBytes, &dwKey, (OVERLAPPED**)&pOV, INFINITE);
		if (!bRet)
		{
			continue;
		}

		switch (pOV->m_rt)
		{
		case RT_RECV:
		{
			RecvRequest* pRR = (RecvRequest*)pOV;

			//继续收数据
			OnRecv(*(Package*)pRR->m_buf);
			PostRecv(pRR->m_sock);

			break;
		}
		case RT_SEND:
		{

			break;
		}
		default:
			break;
		}

		delete pOV;

		/*Package pkg;
		sockaddr_in si;
		int nLen = sizeof(sockaddr_in);
		int nRet = recvfrom(m_sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&si, &nLen);*/

		//if (pkg.m_chFlag & ACK)
		//{
		//	lock_guard<recursive_mutex> lck(m_mtxForSend);
		//	m_mpSend.erase(pkg.m_nSeq);		// 重发移除
		//	continue;
		//}

		//if (pkg.m_chFlag & PSH)
		//{
		//	// 校验
		//	if (pkg.m_nCheck == CCrc32::crc32((unsigned char*)pkg.m_buf, pkg.m_nlen))
		//	{
		//		continue;	// 损毁的包不做处理，等重发
		//	}
		//	Package pkgAck(ACK, pkg.m_nSeq);
		//	sendto(m_sock, (char*)&pkgAck, sizeof(pkgAck), 0, (sockaddr*)&m_siDst, sizeof(m_siDst));

		//	lock_guard<recursive_mutex> lck(m_mtxForRecv);
		//	m_mpRecv[pkg.m_nSeq] = pkg;
		//}
	}
}

void CUMTSocket::HandleThreadProc()
{
	while (m_bRunning)
	{
		this_thread::sleep_for(chrono::microseconds(1));

		// 包内容写入缓冲区
		lock_guard<recursive_mutex> lck(m_mtxForRecv);
		while (m_mpRecv.find(m_nNextRecvSeq) != m_mpRecv.end())		// 向缓冲区写入下一个包
		{
			lock_guard<recursive_mutex> lck(m_mtxForBuf);
			m_buf.Write(m_mpRecv[m_nNextRecvSeq].m_buf, m_mpRecv[m_nNextRecvSeq].m_nlen);
			m_mpRecv.erase(m_nNextRecvSeq);
			++m_nNextRecvSeq;
		}
	}
}

void CUMTSocket::OnRecv(Package& pkg)
{
	if (pkg.m_chFlag & ACK)
	{
		lock_guard<recursive_mutex> lck(m_mtxForSend);
		m_mpSend.erase(pkg.m_nSeq);		// 重发移除
		return;
	}

	if (pkg.m_chFlag & PSH)
	{
		// 校验
		if (pkg.m_nCheck != CCrc32::crc32((unsigned char*)pkg.m_buf, pkg.m_nlen))
		{
			return;						// 损毁的包不做处理，等重发
		}

		Package pkgAck(ACK, pkg.m_nSeq);
		PostSend(m_sock, (uint8_t*)&pkgAck, m_siDst);	// 告诉对方已收到
		//sendto(m_sock, (char*)&pkgAck, sizeof(pkgAck), 0, (sockaddr*)&m_siDst, sizeof(m_siDst));

		lock_guard<recursive_mutex> lck(m_mtxForRecv);
		m_mpRecv[pkg.m_nSeq] = pkg;		// 写进map里方便排序和查找
	}
}
