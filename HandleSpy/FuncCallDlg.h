#pragma once

#include "CallStackTypeDefine.h"
#include "StackDlg.h"
#include "ProgressDlg.h"

class CFuncCallDlg : public CDialogImpl<CFuncCallDlg>
{
public:
	enum {IDD = IDD_DIALOG_FUNCCALL};
	CFuncCallDlg(DWORD dwBegin, DWORD dwEnd);
	~CFuncCallDlg(void);

public:
	BEGIN_MSG_MAP(CFuncCallDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		NOTIFY_CODE_HANDLER(NM_DBLCLK, OnNmDbClick)
		COMMAND_HANDLER(IDC_BUTTON_SHOWALL, BN_CLICKED, OnBnClickedButtonShowall)
		COMMAND_HANDLER(IDC_BUTTON_ONLYLEAK, BN_CLICKED, OnBnClickedButtonOnlyleak)
		COMMAND_HANDLER(IDC_BUTTON_SMARTFILTER, BN_CLICKED, OnBnClickedButtonSmartfilter)
	END_MSG_MAP()

	void ShowAll();
	void ShowLeak();

	void ShowStackDlg(CALL_STACK* pCs, LPCTSTR lpText);
	void ReleaseAllStackDlg();

	void AdvanceFilter(std::vector<CALL_STACK*>& vecSrc, std::vector<CALL_STACK*>& vecDst);
	BOOL SimpleFilter(const std::vector<CALL_STACK*>& vecSrc, std::vector<CALL_STACK*>& vecDst);

public:
	BOOL OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnNmDbClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonShowall(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonOnlyleak(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonSmartfilter(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
	CListViewCtrl	m_listFuncCall;
	DWORD			m_BeginPos;
	DWORD			m_EndPos;
	std::vector<CALL_STACK*>	m_AllSrcVec;
	std::vector<CALL_STACK*>	m_AdvanceFilterVec;
	std::vector<CALL_STACK*>	m_SimpleFilterVec;

	std::map<CALL_STACK*, CStackDlg*>		m_CallstackDlgMap;

	CProgressDlg	m_Progress;
};