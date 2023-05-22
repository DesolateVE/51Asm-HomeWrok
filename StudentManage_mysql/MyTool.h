#pragma once
#include <tchar.h>
#include <strsafe.h>
#include <windows.h>

#ifdef MFC_TAB_HELP
// ͷ�ļ�
#define DECLARE_DIALOG_INIT(tab_name) \
private: \
	CTabCtrl&			m_veTabCtrl = tab_name; \
	CArray<CDialogEx*> 	m_aryDialog; 
#define DECLARE_DIALOG_CLASS(dlg_class) \
	dlg_class m_tab##dlg_class; 
// �Ի����ʼ������
#define BEGIN_DIALOG_CREATE() \
	CRect rcTab; \
	m_veTabCtrl.GetClientRect(rcTab); \
	rcTab.left = rcTab.left + 4; \
	rcTab.top = rcTab.top + 25; \
	rcTab.right = rcTab.right - 4; \
	rcTab.bottom = rcTab.bottom - 4; \
	int nItem = 0; 
#define ON_DIALOG_CREATE(dlg_class, dlg_id , tab_title) \
	m_veTabCtrl.InsertItem(nItem++, ##tab_title); \
	m_tab##dlg_class.Create(dlg_id, &m_veTabCtrl); \
	m_aryDialog.Add(&m_tab##dlg_class); 
#define END_DIALOG_CREATE() \
	for (int i = 0; i < nItem; i++) \
	{ \
		m_aryDialog[i]->MoveWindow(rcTab); \
		m_aryDialog[i]->SetBackgroundColor(0xF9F9F9); \
		m_aryDialog[i]->ShowWindow(SW_HIDE); \
	} \
	m_aryDialog[0]->ShowWindow(SW_SHOW); 
#define ON_DIALOG_CHANGE() \
	for (int i = 0; i < m_aryDialog.GetCount(); i++) \
	{ \
		m_aryDialog[i]->ShowWindow(SW_HIDE); \
	} \
	int n = m_veTabCtrl.GetCurSel(); \
	m_aryDialog[n]->ShowWindow(SW_SHOW);
#endif


void Printf_Debug(LPCTSTR format, ...);
void ErrorExit(LPCTSTR lpszFunction);