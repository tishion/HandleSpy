#pragma once
#include "atlwin.h"
#include "CallStackTypeDefine.h"
#include "ProgressDlg.h"

class CStackDlg :
	public CDialogImpl<CStackDlg>
{
public:

	enum { IDD = IDD_DIALOG_STACK };

	BEGIN_MSG_MAP(CStackDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
	END_MSG_MAP()

	CStackDlg(void);
	~CStackDlg(void);

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
private:
	CListBox	m_StackList;
	CProgressDlg	m_Progress;
};
