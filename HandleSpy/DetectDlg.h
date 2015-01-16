#pragma once
#include "atlwin.h"
#include "Detector.h"
#include "ChartView.h"
#include "CountAndTimeTypeDefine.h"

class CDetectDlg :
	public CDialogImpl<CDetectDlg>
{
public:
	enum { IDD = IDD_DIALOG_DETECT };

	BEGIN_MSG_MAP(CAboutDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_HANDLER(ID_STOP, BN_CLICKED, OnBnClickedStop)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(HSMSG_TARGETPROCESSEXIT, OnTargetProcessExit)
	END_MSG_MAP()

	CDetectDlg(void);

	void SetArrayPointer(std::vector<COUNT_TIME>* p);

// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnBnClickedStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnTargetProcessExit(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

private:
	CChartView			m_chart;
	CDetector			m_detector;

	std::vector<COUNT_TIME>* m_pArray;
};
