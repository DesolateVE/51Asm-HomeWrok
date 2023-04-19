#pragma once


// CAndroidWnd

class CAndroidWnd : public CWnd
{
	DECLARE_DYNAMIC(CAndroidWnd)

public:
	CAndroidWnd();
	virtual ~CAndroidWnd();

public:
	hv::SocketChannelPtr	m_Channel;
	BOOL					m_bControl;

protected:
	DECLARE_MESSAGE_MAP()


public:
	void RefreshView(void* pMemData, long  len);
	void ResetWindow(CRect rect);
	afx_msg void OnClose();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};


