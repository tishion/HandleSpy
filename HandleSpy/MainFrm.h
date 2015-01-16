// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "atlctrlw.h"
#include "atlsplit.h"
#include "ChartView.h"
#include "ProcessSelDlg.h"
#include "DetectDlg.h"
#include "StackStorage.h"
#include "ShionChartView.h"
#include "ApiListView.h"
#include "ProgressDlg.h"

class CMainFrame : 
	public CFrameWindowImpl<CMainFrame>, 
	public CUpdateUI<CMainFrame>,
	public CMessageFilter, 
	public CIdleHandler
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_FILE_SAVE, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
		//UPDATE_ELEMENT(ID_FILE_SAVE, UPDUI_TOOLBAR)

		UPDATE_ELEMENT(ID_FILE_CLOSE, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)

		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_EDIT_OPTIONS, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_FILE_PROCESSLIST, OnProcessList)
		COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(ID_FILE_SAVE, OnFileSave)
		COMMAND_ID_HANDLER(ID_FILE_OPEN, OnFileOpen)
		COMMAND_ID_HANDLER(ID_SHOW_SELECTED_FUNCS, OnShowSelectedFuncs)
		NOTIFY_CODE_HANDLER(NM_DBLCLK, OnNmDbClick)
		NOTIFY_CODE_HANDLER(NM_RCLICK, OnNmRClick)
		COMMAND_ID_HANDLER(ID_FILE_CLOSE, OnFileClose)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()

	void AppendCountData();

	void CreateChartView();
	void CreateStackListView();

	BOOL DoDetect(DWORD dwPid);
	void FillStackListView();
	void FillCountTimeArray();
	void FillModInfoArray();

	BOOL CloseConfirmation();
	void CleanAndClose();

	void SetWindowNameSuffix(LPCTSTR suffix = NULL);

private:
	CToolBarCtrl				m_ToolBar;
	CCommandBarCtrl				m_CmdBar;
	CHorSplitterWindow			m_splitter;
	CShionChartView				m_ChartView;
	CApiListView				m_ApiListView;
	CProgressDlg				m_Progress;

	DWORD						m_MaxCount;
	DWORD						m_MinCount;

	BOOL						m_bSaved;
	BOOL						m_bOpened;

	std::vector<COUNT_TIME>		m_CountArray;
	std::vector<CALL_STACK*>	m_CallStackArray;
	std::vector<MOD_INFO*>		m_ModInfo;
	CString						m_strFileName;
	CString						m_strWindowName;

public:

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

	// Command Message Handler
	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnProcessList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnShowSelectedFuncs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnNmDbClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnNmRClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnFileClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};
