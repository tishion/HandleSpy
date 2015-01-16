#pragma once

#include "CallStackTypeDefine.h"
#include "StackDlg.h"
#include "ProgressDlg.h"
#include "SymbolHandler.h"

class CLeakedFunCallDlg : public CDialogImpl<CLeakedFunCallDlg>
{
public:
	enum {IDD = IDD_DIALOG_SELECTEDFUNCALL};
	CLeakedFunCallDlg(DWORD dwBegin, DWORD dwEnd);
	~CLeakedFunCallDlg(void);
	
	void ShowLeaked();
	void ShowAll();
	void AdvanceFilter(std::vector<CALL_STACK*>& vecSrc, std::vector<CALL_STACK*>& vecDst);

	BEGIN_MSG_MAP(CLeakedFunCallDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		//NOTIFY_CODE_HANDLER(NM_DBLCLK, OnNmDbClick)
		COMMAND_HANDLER(IDC_BUTTON_LEAKED, BN_CLICKED, OnBnClickedButtonLeaked)
		COMMAND_HANDLER(IDC_BUTTON_ALL, BN_CLICKED, OnBnClickedButtonAll)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		NOTIFY_CODE_HANDLER(NM_RCLICK, OnNMRclickTree)
		COMMAND_ID_HANDLER(ID_FUNC_COPY, OnFuncCopy)
		//COMMAND_ID_HANDLER(ID_FUNC_COPYSTACK, OnFuncCopystack)
		COMMAND_ID_HANDLER(ID_FUNC_EXPAND_ALL_STACKS, OnFuncExpandAllStacks)
		COMMAND_ID_HANDLER(ID_FUNC_COLLAPSE_ALL_STACKS, OnFuncCollapseAllStacks)
		COMMAND_ID_HANDLER(ID_FUNC_EXPAND_ALL_SOURCE, OnFuncExpandAllSource)
		COMMAND_ID_HANDLER(ID_FUNC_COLLAPSE_ALL_SOURCE, OnFuncCollapseAllSource)
		COMMAND_ID_HANDLER(ID_FUNC_EXPORT2TXT, OnFuncExport2txt)
		COMMAND_ID_HANDLER(ID_FUNC_COPYALLLEAKED, OnFuncCopyallleaked)
	END_MSG_MAP()

	BOOL OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonLeaked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnNMRclickTree(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnFuncCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFuncCopystack(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFuncExpandAllStacks(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFuncCollapseAllStacks(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFuncExpandAllSource(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFuncCollapseAllSource(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFuncExport2txt(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFuncCopyallleaked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
	DWORD			m_dwHeightOfButtonsPane;
	CSize			m_sizeButton;

	CProgressDlg	m_Progress;
	CTreeViewCtrl	m_treeLeaked;
	CTreeViewCtrl	m_treeAll;
	CButton			m_btnLeaked;
	CButton			m_btnAll;
	CButton			m_btnQuit;
	DWORD			m_BeginPos;
	DWORD			m_EndPos;

	std::vector<CALL_STACK*>	m_vecAllCall;
	std::vector<CALL_STACK*>	m_vecLeakedCall;
};
