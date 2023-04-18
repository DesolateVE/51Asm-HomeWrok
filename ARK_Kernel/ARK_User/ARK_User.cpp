
// ARK_User.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "ARK_User.h"
#include "ARK_UserDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CARKUserApp

BEGIN_MESSAGE_MAP(CARKUserApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CARKUserApp 构造

CARKUserApp::CARKUserApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


CARKUserApp::~CARKUserApp()
{
}

// 唯一的 CARKUserApp 对象

CARKUserApp theApp;

// CARKUserApp 初始化

BOOL CARKUserApp::InitInstance()
{
	CWinApp::InitInstance();

	CShellManager* pShellManager = new CShellManager;

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	// 加载驱动
	TCHAR szSysFile[MAX_PATH];
	TCHAR szSvcLnkName[] = L"KrnlHW64";
	GetFullPathName(L".\\ARK_Kernel.sys", sizeof(szSysFile), szSysFile, NULL);

	if (!theDriver.Install(szSysFile, szSvcLnkName, szSvcLnkName))
	{
		MessageBox(NULL, L"驱动安装失败", 0, 0);
		return FALSE;
	}
	if (!theDriver.Start())
	{
		MessageBox(NULL, L"驱动启动失败", 0, 0);
		return FALSE;
	}
	if (!theDriver.Open(SYMBOL_PATH))
	{
		MessageBox(NULL, L"驱动打开失败", 0, 0);
		return FALSE;
	}

	// 创建对话框
	CARKUserDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// 卸载驱动
	theDriver.Close();
	theDriver.Stop();
	theDriver.Remove();


	if (pShellManager != nullptr)
	{
		delete pShellManager;
}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

