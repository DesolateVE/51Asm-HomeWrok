// CNetViewDlg.cpp: 实现文件
//

#include "pch.h"
#include "ARK_User.h"
#include "afxdialogex.h"
#include "CNetViewDlg.h"
#include <vector>

#pragma comment(lib, "Iphlpapi.lib")

#define ARK_NETWORK_IPV6	6
#define ARK_NETWORK_TCP		0
#define ARK_NETWORK_UDP		1

typedef struct _ARK_NETWORK_ENDPOINT_ITEM {
	ULONG ip_ver;
	ULONG tran_ver;
	WCHAR protocol[8];
	union {
		ULONG local_addr;
		UCHAR local_addr6[16];
	} u0;
	union {
		ULONG remote_addr;
		UCHAR remote_addr6[16];
	} u1;
	ULONG local_port;
	ULONG remote_port;
	WCHAR local[64];
	WCHAR remote[64];
	ULONG state;
	WCHAR readable_state[32];
	ULONG pid;
} ARK_NETWORK_ENDPOINT_ITEM, * PARK_NETWORK_ENDPOINT_ITEM;

PWCHAR GetReadableState(int state)
{
	switch (state) {
	case MIB_TCP_STATE_CLOSED:
		return L"CLOSED";
	case MIB_TCP_STATE_LISTEN:
		return L"LISTENING";
	case MIB_TCP_STATE_ESTAB:
		return L"ESTABLISHED";
	case MIB_TCP_STATE_SYN_SENT:
		return L"SYN_SENT";
	case MIB_TCP_STATE_SYN_RCVD:
		return L"SYN_RECV";
	case MIB_TCP_STATE_FIN_WAIT1:
		return L"FIN_WAIT1";
	case MIB_TCP_STATE_FIN_WAIT2:
		return L"FIN_WAIT2";
	case MIB_TCP_STATE_CLOSE_WAIT:
		return L"CLOSE_WAIT";
	case MIB_TCP_STATE_CLOSING:
		return L"CLOSING";
	case MIB_TCP_STATE_LAST_ACK:
		return L"LAST_ACK";
	case MIB_TCP_STATE_TIME_WAIT:
		return L"TIME_WAIT";
	case MIB_TCP_STATE_DELETE_TCB:
		return L"DELETE_TCB";
	default:
		return L"UNKNOWN";
	}
}

// CNetViewDlg 对话框

IMPLEMENT_DYNAMIC(CNetViewDlg, CDialogEx)

CNetViewDlg::CNetViewDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NORMAL_LIST, pParent)
{

}

CNetViewDlg::~CNetViewDlg()
{
}

void CNetViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CNetViewDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CNetViewDlg 消息处理程序


BOOL CNetViewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect ListCtrlRect;
	m_ListCtrl.GetClientRect(ListCtrlRect);
	DWORD dwWidth = ListCtrlRect.Width() / 10;

	ULONG nNum = 0;
	m_ListCtrl.InsertColumn(nNum++, L"协议", LVCFMT_LEFT, dwWidth * 1);
	m_ListCtrl.InsertColumn(nNum++, L"本地地址", LVCFMT_LEFT, dwWidth * 2);
	m_ListCtrl.InsertColumn(nNum++, L"外部地址", LVCFMT_LEFT, dwWidth * 2);
	m_ListCtrl.InsertColumn(nNum++, L"状态", LVCFMT_LEFT, dwWidth * 3);
	m_ListCtrl.InsertColumn(nNum++, L"进程ID", LVCFMT_LEFT, ListCtrlRect.Width() - dwWidth * 8);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CNetViewDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码
	if (!bShow)
	{
		return;
	}

	m_ListCtrl.DeleteAllItems();

	std::vector<ARK_NETWORK_ENDPOINT_ITEM> items;

	typedef PWSTR(NTAPI* __RtlIpv6AddressToStringW)(
		_In_ const struct in6_addr* Addr,
		_Out_writes_(46) PWSTR* S
		);
	auto pRtlIpv6AddressToStringW = (__RtlIpv6AddressToStringW)
		GetProcAddress(GetModuleHandle(L"ntdll.dll"), "RtlIpv6AddressToStringW");
	if (!pRtlIpv6AddressToStringW) return;

	DWORD size = 0;
	PMIB_TCP6TABLE_OWNER_PID tcp = NULL;
	if (GetExtendedTcpTable(tcp, &size, TRUE, AF_INET6, TCP_TABLE_OWNER_PID_ALL, 0) != ERROR_INSUFFICIENT_BUFFER)
		return;

	tcp = (MIB_TCP6TABLE_OWNER_PID*)new char[size];
	if (GetExtendedTcpTable(tcp, &size, TRUE, AF_INET6, TCP_TABLE_OWNER_PID_ALL, 0) != NO_ERROR) {
		delete[] tcp;
		return;
	}
	int nums = (int)tcp->dwNumEntries;
	for (int i = 0; i < nums; i++) {
		ARK_NETWORK_ENDPOINT_ITEM endpoint;
		endpoint.ip_ver = ARK_NETWORK_IPV6;
		endpoint.tran_ver = ARK_NETWORK_TCP;
		wcscpy_s(endpoint.protocol, L"TCP6");
		RtlCopyMemory(&endpoint.u0.local_addr6, &tcp->table[i].ucLocalAddr, 16);
		RtlCopyMemory(&endpoint.u1.remote_addr6, &tcp->table[i].ucRemoteAddr, 16);
		endpoint.local_port = tcp->table[i].dwLocalPort;
		endpoint.remote_port = tcp->table[i].dwRemotePort;
		endpoint.state = tcp->table[i].dwState;
		endpoint.pid = tcp->table[i].dwOwningPid;
		wcscpy_s(endpoint.readable_state, GetReadableState(endpoint.state));

		WCHAR str[64] = { 0 };
		pRtlIpv6AddressToStringW((in6_addr*)&endpoint.u0.local_addr6, (wchar_t**)&str);
		swprintf_s(endpoint.local, L"[%s]:%d", &str, htons(endpoint.local_port));
		if (endpoint.state == MIB_TCP_STATE_LISTEN) {
			wcscpy_s(endpoint.remote, L"[::]:0");
		}
		else {
			RtlZeroMemory(&str, sizeof(str));
			pRtlIpv6AddressToStringW((in6_addr*)&endpoint.u1.remote_addr6, (wchar_t**)&str);
			swprintf_s(endpoint.remote, L"[%s]:%d", &str, htons(endpoint.remote_port));
		}
		items.push_back(endpoint);
	}
	delete[] tcp;

	size = 0;
	PMIB_UDP6TABLE_OWNER_PID udp = NULL;
	if (GetExtendedUdpTable(udp, &size, TRUE, AF_INET6, UDP_TABLE_OWNER_PID, 0) != ERROR_INSUFFICIENT_BUFFER)
		return;

	udp = (MIB_UDP6TABLE_OWNER_PID*)new char[size];
	if (GetExtendedUdpTable(udp, &size, TRUE, AF_INET6, UDP_TABLE_OWNER_PID, 0) != NO_ERROR) {
		delete[] udp;
		return;
	}
	nums = (int)udp->dwNumEntries;
	for (int i = 0; i < nums; i++) {
		ARK_NETWORK_ENDPOINT_ITEM endpoint;
		endpoint.ip_ver = ARK_NETWORK_IPV6;
		endpoint.tran_ver = ARK_NETWORK_UDP;
		wcscpy_s(endpoint.protocol, L"UDP6");
		RtlCopyMemory(&endpoint.u0.local_addr6, &udp->table[i].ucLocalAddr, 16);
		endpoint.local_port = udp->table[i].dwLocalPort;
		endpoint.pid = udp->table[i].dwOwningPid;
		WCHAR str[64] = { 0 };
		pRtlIpv6AddressToStringW((in6_addr*)&endpoint.u0.local_addr6, (wchar_t**)&str);
		swprintf_s(endpoint.local, L"[%s]:%d", &str, htons(endpoint.local_port));
		wcscpy_s(endpoint.remote, L"*:*");
		items.push_back(endpoint);
	}
	delete[] udp;


	UINT nNum = 0;
	CString szTemp;
	for (auto& var : items)
	{
		UINT nCol = 1;
		m_ListCtrl.InsertItem(nNum, var.protocol);
		m_ListCtrl.SetItemText(nNum, nCol++, var.local);
		m_ListCtrl.SetItemText(nNum, nCol++, var.remote);
		m_ListCtrl.SetItemText(nNum, nCol++, var.tran_ver == ARK_NETWORK_TCP ? var.readable_state : L"");
		szTemp.Format(L"%d", var.pid);
		m_ListCtrl.SetItemText(nNum, nCol++, szTemp);

		nNum++;
	}

}
