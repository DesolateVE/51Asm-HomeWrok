// CAndroidWnd.cpp: 实现文件
//

#include "pch.h"
#include "AndroidRemoteControl.h"
#include "AndroidWnd.h"
#include "SocketPackage.h"


// CAndroidWnd

IMPLEMENT_DYNAMIC(CAndroidWnd, CWnd)

CAndroidWnd::CAndroidWnd()
{
    m_bControl = FALSE;
}

CAndroidWnd::~CAndroidWnd()
{
}


BEGIN_MESSAGE_MAP(CAndroidWnd, CWnd)
    ON_WM_CLOSE()
    ON_WM_CHAR()
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()



// CAndroidWnd 消息处理程序

void CAndroidWnd::RefreshView(void* pMemData, long len)
{
    IStream* pStream;
    CImage image;

    if (CreateStreamOnHGlobal(NULL, TRUE, &pStream) == S_OK)
    {
        pStream->Write(pMemData, len, NULL);
        if (SUCCEEDED(image.Load(pStream))) {
            CDC* pDC = GetDC();
            CRect rect;
            GetClientRect(rect);
            image.StretchBlt(pDC->m_hDC, rect);
            ReleaseDC(pDC);
            pStream->Release();
        }
    }


    //HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, len);
    //void* pData = GlobalLock(hGlobal);
    //memcpy(pData, pMemData, len);
    //GlobalUnlock(hGlobal);

    //IStream* pStream = NULL;
    //if (CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) == S_OK)
    //{
    //    CImage image;
    //    if (SUCCEEDED(image.Load(pStream)))
    //    {
    //        CDC* pDC = GetDC();
    //        CRect rect;
    //        GetClientRect(rect);
    //        image.StretchBlt(pDC->m_hDC, rect);
    //        image.Destroy();
    //        ReleaseDC(pDC);
    //    }
    //    pStream->Release();
    //}
    //GlobalFree(hGlobal);
}

void CAndroidWnd::ResetWindow(CRect rect)
{
    CRect WindowRect;
    CRect ClientRect;
    GetWindowRect(WindowRect);
    GetClientRect(ClientRect);

    UINT PaddingWidth = WindowRect.Width() - ClientRect.Width();
    UINT PaddingHeight = WindowRect.Height() - ClientRect.Height();

    rect.right += PaddingWidth;
    rect.bottom += PaddingHeight;

    MoveWindow(rect);
    CenterWindow();
    ShowWindow(SW_SHOW);
}


void CAndroidWnd::OnClose()
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    std::shared_ptr<CPackage> pPackage(new CPackage(PackageType::REQUEST_WINDOW_SHOW_OFF, nullptr, 0));
    m_Channel->write(pPackage->m_Buffer, pPackage->m_Length);

    CWnd::OnClose();
}


void CAndroidWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    if (!m_bControl)
    {
        return CWnd::OnLButtonDown(nFlags, point);
    }

    CStringA ShellCmd;
    ShellCmd.Format("input tap %d %d\n", point.x, point.y);
    std::shared_ptr<CPackage> pPackage(new CPackage(PackageType::STREAM_INPUT_CTRL, ShellCmd.GetBuffer(), ShellCmd.GetLength()));
    m_Channel->write(pPackage->m_Buffer, pPackage->m_Length);


    CWnd::OnLButtonDown(nFlags, point);
}





void CAndroidWnd::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    if (!m_bControl)
    {
        return CWnd::OnChar(nChar, nRepCnt, nFlags);
    }

    UINT LinuxKey = nChar;
    CStringA ShellCmd = "input keyevent";
    const char* szCapitalChange = "input keyevent 115";
    bool flag = false;

    if (nChar >= 65 && nChar <= 90)                // A ~ Z
    {
        flag = true;
        LinuxKey -= 36;
        ShellCmd.Format("%s\n%s %d\n%s\n", szCapitalChange, ShellCmd, LinuxKey, szCapitalChange);
    }
    else if (nChar >= 48 && nChar <= 57)        // 0 ~ 9
    {
        LinuxKey -= 41;
    }
    else if (nChar >= 97 && nChar <= 122)        // a ~ z
    {
        LinuxKey -= 68;
    }
    else if (nChar == 32)                        // SPACE
    {
        LinuxKey = 62;
    }
    else if (nChar == 13)                        // ENTER
    {
        LinuxKey = 66;
    }
    else if (nChar == 8)                        // BS
    {
        LinuxKey = 67;
    }
    else
    {
        return CWnd::OnChar(nChar, nRepCnt, nFlags);
    }

    if (!flag)
    {
        ShellCmd.Format("%s %d\n", ShellCmd, LinuxKey);
    }

    std::shared_ptr<CPackage> pPackage(new CPackage(PackageType::STREAM_INPUT_CTRL, ShellCmd.GetBuffer(), ShellCmd.GetLength()));
    m_Channel->write(pPackage->m_Buffer, pPackage->m_Length);

    CWnd::OnChar(nChar, nRepCnt, nFlags);
}


void CAndroidWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    if (!m_bControl)
    {
        return CWnd::OnRButtonDown(nFlags, point);
    }

    CStringA ShellCmd;
    ShellCmd.Format("input swipe %d %d %d %d 500\n", point.x, point.y, point.x, point.y);
    std::shared_ptr<CPackage> pPackage(new CPackage(PackageType::STREAM_INPUT_CTRL, ShellCmd.GetBuffer(), ShellCmd.GetLength()));
    m_Channel->write(pPackage->m_Buffer, pPackage->m_Length);

    CWnd::OnRButtonDown(nFlags, point);
}
