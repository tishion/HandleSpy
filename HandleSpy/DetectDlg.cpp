#include "StdAfx.h"
#include <MMSystem.h>
#pragma comment(lib, "winmm.lib")

#include "DetectDlg.h"


#define ID_TIMER_GETHANDLECOUNT	0x1000
#define CHART_UPDATE_ECLIPSE	1000

CDetectDlg::CDetectDlg(void)
{
	m_pArray = NULL;
}

LRESULT CDetectDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	DWORD dwPid = (DWORD)lParam;
	DWORD dwInitCount = 0;
	if (dwPid <= 0)
	{
		EndDialog(IDCANCEL);
		return 0;
	}

	m_chart.SubclassWindow(GetDlgItem(IDC_STATIC_CHARTFRAME).m_hWnd);
	
	if (TRUE == (FALSE == m_detector.Init(dwPid, m_hWnd)) || (FALSE == m_detector.Start()))
	{
		EndDialog(IDCANCEL);
		return 0;
	}
	
	m_detector.GetHandleCount(&dwInitCount);
	m_chart.SetRangeAndFirstValue(dwInitCount-500, dwInitCount+500, dwInitCount);
	SetTimer(ID_TIMER_GETHANDLECOUNT, CHART_UPDATE_ECLIPSE);
	
	CenterWindow(GetParent());
	return 0;
}

void CDetectDlg::SetArrayPointer(std::vector<COUNT_TIME>* p)
{
	if (NULL != p)
	{
		m_pArray = p;
		m_pArray->clear();
	}
}

LRESULT CDetectDlg::OnBnClickedStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_detector.Stop();
	EndDialog(IDOK);
	return 0;
}

LRESULT CDetectDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int nRet = AtlMessageBox(m_hWnd,
		_T("确认要停止检测吗？"),
		_T("提示"), 
		MB_OKCANCEL | MB_ICONQUESTION);
	
	if (nRet != IDOK)
	{
		return 0;
	}
	m_detector.Stop();
	EndDialog(IDOK);
	return 0;
}

LRESULT CDetectDlg::OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	COUNT_TIME ct;
	ct.dwCount = 0;
	ct.dwTime = 0;
	UINT uPeriod = 1;

	if (FALSE == m_detector.GetHandleCount(&ct.dwCount))
	{
		AtlTrace(_T("Failed at m_Worker.GetHandleCount(&dwCount).\r\n"));
		return 0;
	}

	timeBeginPeriod(uPeriod);
	ct.dwTime = timeGetTime();
	timeEndPeriod(uPeriod);

	if (NULL != m_pArray)
	{
		m_pArray->push_back(ct);
	}
	
	m_chart.Update(ct.dwCount);

	return 0;
}

LRESULT CDetectDlg::OnTargetProcessExit(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	KillTimer(ID_TIMER_GETHANDLECOUNT);
	AtlMessageBox(m_hWnd, _T("目标进程已经退出！"));

	return 0;
}