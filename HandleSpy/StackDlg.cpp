#include "StdAfx.h"
#include "StackDlg.h"
#include "SymbolHandler.h"

CStackDlg::CStackDlg(void)
{
}

CStackDlg::~CStackDlg(void)
{
	m_StackList.Detach();
	if (IsWindow())
	{
		DestroyWindow();
	}
	delete this;
}

LRESULT CStackDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	TCHAR wcsListString[_MAX_PATH];
	StringT strFrame;
	
	PCALL_STACK pCs = (PCALL_STACK)lParam;
	m_StackList.Attach(GetDlgItem(IDC_LIST_STACK));

	m_Progress.Go(m_hWnd, _T("ÕýÔÚ½âÎö·ûºÅ..."), pCs->nFrameCount);
	for (UINT i=0; i<pCs->nFrameCount; i++)
	{
		m_Progress.StepIt(NULL);
		strFrame = CSymbolHandler::GetInstance()->FrameNameFromAddr(pCs->frame[i]);
		::ZeroMemory(wcsListString, sizeof(wcsListString));
		_stprintf_s(wcsListString, _countof(wcsListString), _T("%03d | %s"), i+1, strFrame.c_str());
		m_StackList.InsertString(-1, wcsListString);
	}
	m_Progress.Stop();

	return 0;
}

LRESULT CStackDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DestroyWindow();
	return 0;
}
LRESULT CStackDlg::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CRect rcClient;
	GetClientRect(rcClient);

	m_StackList.MoveWindow(rcClient);

	return 0;
}
