#pragma once
#include "CallStackTypeDefine.h"
#include "atlwin.h"

class CApiListView :
	public CWindowImpl<CApiListView, CListViewCtrl>
{
public:
public:
	DECLARE_WND_SUPERCLASS(NULL, CListViewCtrl::GetWndClassName())

	BOOL PreTranslateMessage(MSG* pMsg);

	CApiListView(void);
	~CApiListView(void);

	BEGIN_MSG_MAP(CApiListView)
	END_MSG_MAP()


	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
};
