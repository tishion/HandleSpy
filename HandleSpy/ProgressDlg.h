#pragma once
#include "atlwin.h"

#define PROGRESS_MSG_STEPIT		(WM_USER+0x100)
#define PROGRESS_MSG_STOP		(WM_USER+0x101)

class CProgressDlg :
	public CWindowImpl<CProgressDlg, CWindow, CWinTraits<WS_OVERLAPPED | WS_CAPTION, WS_EX_TOOLWINDOW | WS_EX_WINDOWEDGE | WS_EX_TOPMOST>>
{
public:
	CProgressDlg(void);
	~CProgressDlg(void);

	void Go(HWND hWndParent, LPCTSTR lpText, DWORD dwRange);
	void StepIt(LPCTSTR lpText = NULL);
	void Stop();

	BEGIN_MSG_MAP(CProcessSelDlg)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(PROGRESS_MSG_STEPIT, OnStepIt)
		MESSAGE_HANDLER(PROGRESS_MSG_STOP, OnClose)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnStepIt(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

protected:
	static DWORD WINAPI ThreadProc(LPVOID lpVoid);

private:
	CProgressBarCtrl	m_Progress;
	HANDLE				m_hTread;

	HWND				m_hWndParent;
	DWORD				m_Range;
	CString				m_Text;

	HANDLE				m_OkEvent;
};
