#include "StdAfx.h"
#include "ProgressDlg.h"

CProgressDlg::CProgressDlg(void)
{
	m_hTread = NULL;
	m_OkEvent = NULL;
}

CProgressDlg::~CProgressDlg(void)
{
	m_hTread = NULL;
	m_OkEvent = NULL;
}

LRESULT CProgressDlg::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	LPCREATESTRUCT pCreateStruct = (LPCREATESTRUCT)lParam;

	CRect rcClient(0, 0, pCreateStruct->cx, pCreateStruct->cy);

	SetWindowText(m_Text);

	CRect rcProgress;
	rcProgress.left = rcClient.left + 5;
	rcProgress.right = rcClient.right - 5;
	rcProgress.top = rcClient.top + 20;
	rcProgress.bottom = rcProgress.top + 40;
	m_Progress.Create(m_hWnd, rcProgress, NULL, WS_CHILD | WS_VISIBLE | PBS_SMOOTH);

	m_Progress.SetRange(0, m_Range);
	m_Progress.SetStep(1);

	return 0;
}

void CProgressDlg::Go(HWND hWndParent, LPCTSTR lpText, DWORD dwRange)
{
	m_hWndParent = hWndParent;
	m_Text = lpText;
	m_Range = dwRange;

	::EnableWindow(m_hWndParent, FALSE);
	
	DWORD dwTid = 0;
	m_OkEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hTread = ::CreateThread(NULL, 0, ThreadProc, this, 0, &dwTid);
	::WaitForSingleObject(m_OkEvent, INFINITE);
	::CloseHandle(m_OkEvent);
}

void CProgressDlg::StepIt(LPCTSTR lpText)
{
	SendMessage(PROGRESS_MSG_STEPIT, (WPARAM)lpText, 0);
}

void CProgressDlg::Stop()
{
	PostMessage(PROGRESS_MSG_STOP, 0, 0);
	::WaitForSingleObject(m_hTread, INFINITE);
	::CloseHandle(m_hTread);

	::EnableWindow(m_hWndParent, TRUE);
	::SetForegroundWindow(m_hWndParent);

	m_hTread = NULL;
	m_Range = 0;
	m_Text = _T("");
	m_hWnd = NULL;
	m_Progress.m_hWnd = NULL;
}

DWORD WINAPI CProgressDlg::ThreadProc(LPVOID lpVoid)
{
	CProgressDlg* p = (CProgressDlg*)lpVoid;
	CRect rc(0, 0, 500, 50);
	p->Create(NULL, rc, NULL, WS_POPUP|WS_VISIBLE);
	p->CenterWindow(p->m_hWndParent);
	p->ShowWindow(SW_SHOW);

	::SetEvent(p->m_OkEvent);
	
	CMessageLoop msgLoop;
	int nRet = msgLoop.Run();

	return 0;
}

LRESULT CProgressDlg::OnStepIt(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	LPCTSTR lpText = (LPCTSTR)wParam;
	m_Progress.StepIt();
	if (NULL != lpText)
	{
		SetWindowText(lpText);
	}

	return 0;
}

LRESULT CProgressDlg::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DestroyWindow();
	return 0;
}

LRESULT CProgressDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	PostQuitMessage(0);
	return 0;
}
