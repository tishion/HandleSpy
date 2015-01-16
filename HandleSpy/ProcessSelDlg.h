#pragma once

class CProcessSelDlg : public CDialogImpl<CProcessSelDlg>
{
public:
	enum {IDD = IDD_DIALOG_PROCESS};
	CProcessSelDlg(void);
	~CProcessSelDlg(void);

public:
	BEGIN_MSG_MAP(CProcessSelDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDOK, BN_CLICKED, OnBnClickedOk)
		COMMAND_HANDLER(IDC_BUTTON_REFRESH, BN_CLICKED, OnBnClickedRefresh)
		COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnBnClickedCancel)
		NOTIFY_HANDLER(IDC_PROCESSLIST, NM_DBLCLK, OnNmDblClickProcesslist)
		NOTIFY_CODE_HANDLER(HDN_ITEMCLICK, OnHdnItemClickProcesslist) 
	END_MSG_MAP()

public:
	BOOL OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnBnClickedOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnNMDblclkProcessList(int /*idCtrl*/, LPNMHDR /*pNMHDR*/, BOOL& /*bHandled*/);
	LRESULT OnNmDblClickProcesslist(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnHdnItemClickProcesslist(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);

protected:
	void ReFreshProcessList();
	void GetSelectedProcessID();
	static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

private:
	CListViewCtrl	m_listProcess;
	PDWORD			m_pdwPidReceiver;

	static BOOL		s_Order[2];
	static INT		s_SortColumn;
};