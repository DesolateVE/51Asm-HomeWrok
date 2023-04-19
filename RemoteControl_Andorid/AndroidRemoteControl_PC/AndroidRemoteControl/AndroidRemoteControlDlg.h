
// AndroidRemoteControlDlg.h: 头文件
//

#pragma once

#include "AndroidWnd.h"
#include "AndroidConsoleDlg.h"

// CAndroidRemoteControlDlg 对话框
class CAndroidRemoteControlDlg : public CDialogEx
{
	// 构造
public:
	CAndroidRemoteControlDlg(CWnd* pParent = nullptr);	// 标准构造函数
	virtual ~CAndroidRemoteControlDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ANDROIDREMOTECONTROL_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


	// 实现
protected:
	HICON					m_hIcon;
	CEdit					m_EditIP;
	CEdit					m_EditHost;
	CEdit					m_EditPort;
	CStatic					m_StaticDest;
	COLORREF				m_TextColor;
	CAndroidWnd				m_AndroidWnd;
	CAndroidConsoleDlg		m_AndroidControlDlg;

	hv::TcpServer			m_TcpSrv;
	hv::SocketChannelPtr	m_Channel;
	BOOL					m_bInit;
	BOOL					m_bClose;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()

private:
	static const COLORREF rgbRed = 0x000000FF;
	static const COLORREF rgbGreen = 0x0000FF00;
	void InitSystemInfo();
	void InitSocketInfo();

public:
	afx_msg void OnBnClickedPortOn();
	afx_msg void OnBnClickedRemoteControl();
	afx_msg void OnBnClickedShowWindow();
	afx_msg void OnClose();
	afx_msg void OnBnClickedShellOn();
};