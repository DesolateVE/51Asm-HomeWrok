#pragma once


// CFindIcon

class CFindIcon : public CStatic
{
	DECLARE_DYNAMIC(CFindIcon)

public:
	CFindIcon();
	virtual ~CFindIcon();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


