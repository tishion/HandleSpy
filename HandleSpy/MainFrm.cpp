#include "StdAfx.h"
#include "MainFrm.h"
#include "AboutDlg.h"
#include "StackDlg.h"
#include "FuncCallDlg.h"
#include "LeakedFunCallDlg.h"
#include "Api.h"
#include "SymbolHandler.h"
#include "ProgressDlg.h"

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;

	return m_ChartView.PreTranslateMessage(pMsg) || m_ApiListView.PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnIdle()
{
	UIUpdateToolBar();
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	TCHAR szWindowName[_MAX_PATH];
	ZeroMemory(szWindowName, sizeof(szWindowName));
	LoadString(ModuleHelper::GetResourceInstance(), GetWndClassInfo().m_uCommonResourceID, szWindowName, _countof(szWindowName));

	m_strWindowName = szWindowName;
	
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	m_CmdBar.AttachMenu(GetMenu());
	m_CmdBar.LoadImages(IDR_MAINFRAME);
	SetMenu(NULL);

	HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
	m_ToolBar.Attach(hWndToolBar);

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(hWndCmdBar);
	AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

	m_hWndClient = m_splitter.Create(m_hWnd, rcDefault, NULL, 
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	CreateChartView();
	CreateStackListView();

	UpdateLayout();

	m_splitter.SetSplitterPanes(m_ChartView, m_ApiListView);

	CRect rcClient;
	GetClientRect(rcClient);
	m_splitter.SetSplitterPos((rcClient.bottom - rcClient.top) / 4);
	
	UIAddToolBar(hWndToolBar);
	UISetCheck(ID_VIEW_TOOLBAR, TRUE);
	UIEnable(ID_EDIT_OPTIONS, FALSE);	
	
	m_bOpened = FALSE;
	m_bSaved = FALSE;
	UIEnable(ID_FILE_SAVE, FALSE);
	UIEnable(ID_FILE_CLOSE, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	bHandled = FALSE;
	return 1;
}

// Command Message Handler
LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (FALSE == CloseConfirmation())
	{
		return 0;
	}

	PostQuitMessage(0);
	return 0;
}

LRESULT CMainFrame::OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (FALSE == CloseConfirmation())
	{
		return 0;
	}

	CFileDialog dlg(TRUE, NULL, NULL, OFN_SHAREAWARE, _T("HSL Files(*.hsl)\0*.hsl\0All Files (*.*)\0*.*||"));

	TCHAR tszFilePathName[_MAX_PATH];
	ZeroMemory(tszFilePathName, sizeof(tszFilePathName));

	dlg.m_ofn.lpstrFile = tszFilePathName;
	dlg.m_ofn.nMaxFile = _countof(tszFilePathName);
	if( dlg.DoModal() == IDOK )
	{
		m_Progress.Go(m_hWnd, _T("正在处理数据..."), 5);
		CStackStorage::GetInstance()->ReadFromFile(tszFilePathName);
		CStackStorage::GetInstance()->Open();

		if (FALSE == CloseConfirmation())
		{
			return 0;
		}

		{// 更新UI
			SetWindowNameSuffix(tszFilePathName);
			m_bSaved = TRUE;
			UIEnable(ID_FILE_SAVE, FALSE);
			m_bOpened = TRUE;
			UIEnable(ID_FILE_CLOSE, TRUE);
		}

		m_Progress.StepIt(NULL);
		FillCountTimeArray();

		m_Progress.StepIt(NULL);
		FillStackListView();

		m_Progress.StepIt(NULL);
		FillModInfoArray();

		m_Progress.StepIt(_T("绘制线条图表..."));
		m_ChartView.DrawLineChart(&m_CountArray);

		m_Progress.StepIt(_T("加载符号文件..."));
		CSymbolHandler::GetInstance()->ClearAllSymbols();
		CSymbolHandler::GetInstance()->LoadSymbols(m_ModInfo);

		m_Progress.StepIt(NULL);
		m_Progress.Stop();
	}

	return 0;
}

LRESULT CMainFrame::OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CFileDialog dlg(FALSE, _T("wrl"), NULL, OFN_SHAREAWARE | OFN_OVERWRITEPROMPT, _T("HSL Files(*.hsl)\0*.hsl"));

	TCHAR tszFilePathName[_MAX_PATH];
	ZeroMemory(tszFilePathName, sizeof(tszFilePathName));
	_stprintf_s(tszFilePathName, m_strFileName, _countof(tszFilePathName));

	dlg.m_ofn.lpstrFile = tszFilePathName;
	dlg.m_ofn.nMaxFile = _countof(tszFilePathName);
	if(IDOK == dlg.DoModal())
	{
		CStackStorage::GetInstance()->WriteToFile(tszFilePathName);

		{// 更新UI
			SetWindowNameSuffix(tszFilePathName);
			//m_bOpened = TRUE;
			//UIEnable(ID_FILE_CLOSE, TRUE);
			m_bSaved = TRUE;
			UIEnable(ID_FILE_SAVE, FALSE);
		}
	}

	return 0;
}

LRESULT CMainFrame::OnFileClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (FALSE == CloseConfirmation())
	{
		return 0;
	}

	{// 更新UI
		CleanAndClose();
		SetWindowNameSuffix();
		m_bOpened  = FALSE;
		UIEnable(ID_FILE_CLOSE, FALSE);
		m_bSaved = FALSE;
		UIEnable(ID_FILE_SAVE, FALSE);
	}

	return 0;
}

LRESULT CMainFrame::OnProcessList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DWORD dwPid = 0;
	CProcessSelDlg ProcessDlg;

	if (IDOK == ProcessDlg.DoModal(m_hWnd, (LPARAM)&dwPid))
	{
		if (FALSE == CloseConfirmation())
		{
			return 0;
		}

		if (TRUE == m_bOpened)
		{
			CleanAndClose();
			SetWindowNameSuffix();
			m_bOpened = FALSE;
			UIEnable(ID_FILE_CLOSE, FALSE);
			m_bSaved = FALSE;
			UIEnable(ID_FILE_SAVE, FALSE);
		}

		if (TRUE == DoDetect(dwPid))
		{
			CStackStorage::GetInstance()->GenerateStorageFileName(m_strFileName);

			{// 更新UI
				SetWindowNameSuffix(m_strFileName);
				m_bOpened = TRUE;
				m_bSaved = FALSE;
				UIEnable(ID_FILE_SAVE, TRUE);
				UIEnable(ID_FILE_CLOSE, TRUE);
			}
		}
	}

	return 0;
}

LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	static BOOL bNew = TRUE;				// initially visible
	bNew = !bNew;
	UINT uBandID = ATL_IDW_BAND_FIRST + 1;	// toolbar is second added band
	CReBarCtrl rebar = m_hWndToolBar;
	int nBandIndex = rebar.IdToIndex(uBandID);
	rebar.ShowBand(nBandIndex, bNew);
	UISetCheck(ID_VIEW_TOOLBAR, bNew);
	UpdateLayout();

	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainFrame::OnShowSelectedFuncs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DWORD dwFirst = 0;
	DWORD dwLast = 0;

	m_ChartView.GetSelectedIndexRange(dwFirst, dwLast);
	
	DWORD dwBeginTime = m_CountArray[dwFirst].dwTime;
	DWORD dwEndTime = m_CountArray[dwLast].dwTime;

	DWORD dwBeginPos = 0;
	DWORD dwEndPos = 0;

	if (false == m_CallStackArray.empty())
	{

		for (UINT i=0; i<m_CallStackArray.size(); i++)
		{
			if (m_CallStackArray[i]->dwTimeStamp >= dwBeginTime)
			{
				dwBeginPos = i;
				break;
			}
		}

		dwEndPos = m_CallStackArray.size() - 1;

		for (UINT i=dwBeginPos; i<m_CallStackArray.size(); i++)
		{
			if (m_CallStackArray[i]->dwTimeStamp > dwEndTime)
			{
				dwEndPos = i - 1;
				break;
			}
		}
	}

	//CFuncCallDlg dlg(dwBeginPos, dwEndPos);
	CLeakedFunCallDlg dlg(dwBeginPos, dwEndPos);
	dlg.DoModal(m_hWnd, (LPARAM)&m_CallStackArray);

	return 0;
}

LRESULT CMainFrame::OnNmDbClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	POINT pt = { 0, 0 };
	GetCursorPos(&pt);
	POINT ptClient = pt;
	if(pnmh->hwndFrom != NULL)
	{
		::ScreenToClient(pnmh->hwndFrom, &ptClient);
	}

	if(pnmh->hwndFrom == m_ApiListView.m_hWnd)
	{
		LVHITTESTINFO lvhti = { 0 };
		lvhti.pt = ptClient;
		m_ApiListView.HitTest(&lvhti);
		
		if (lvhti.iItem != -1)
		{
			CString strFuncNmae;
			CString strWindowText = _T("调用栈：");
			CStackDlg* pDlg = NULL;

			pDlg = (CStackDlg*)(m_ApiListView.GetItemData(lvhti.iItem));

			if (NULL != pDlg && TRUE == pDlg->IsWindow())
			{
				pDlg->SetActiveWindow();
				pDlg->ShowWindow(SW_NORMAL);
			}
			else
			{
				if (0 != m_ApiListView.GetItemText(lvhti.iItem, 1, strFuncNmae))
				{
					strWindowText += strFuncNmae;
				}

				pDlg = new CStackDlg;
				pDlg->Create(m_hWnd, (LPARAM)m_CallStackArray.at(lvhti.iItem));
				pDlg->SetWindowText(strWindowText);
				pDlg->ShowWindow(SW_NORMAL);
				m_ApiListView.SetItemData(lvhti.iItem, (DWORD_PTR)pDlg);
			}
		}
	}

	return 0;
}

LRESULT CMainFrame::OnNmRClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	POINT pt = { 0, 0 };
	GetCursorPos(&pt);
	POINT ptClient = pt;
	if(pnmh->hwndFrom != NULL)
	{
		::ScreenToClient(pnmh->hwndFrom, &ptClient);
	}

	if(pnmh->hwndFrom == m_ChartView.m_hWnd)
	{
		if (TRUE == m_ChartView.IsSelected())
		{
			CMenu menu; 
			menu.LoadMenu(IDR_MENU_POPUP); 
			CMenuHandle MenuPopup = menu.GetSubMenu(0); 
			MenuPopup.TrackPopupMenu(TPM_RIGHTBUTTON | TPM_VERPOSANIMATION | TPM_VERTICAL, 
				pt.x, pt.y, m_hWnd);
		}
	}

	return 0;
}

LRESULT CMainFrame::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (FALSE == CloseConfirmation())
	{
		return 0;
	}

	DestroyWindow();

	return 0;
}

BOOL CMainFrame::CloseConfirmation()
{
	if (TRUE == m_bOpened && FALSE == m_bSaved)
	{
		if (IDCANCEL == AtlMessageBox(m_hWnd, 
			_T("当前日志文件未保存，确定关闭？"), 
			IDR_MAINFRAME, MB_OKCANCEL|MB_ICONQUESTION))
		{
			return FALSE;
		}
	}

	return TRUE;
}

void CMainFrame::CreateChartView()
{
	ATLASSERT(::IsWindow(m_hWndClient));

	m_ChartView.Create(m_hWndClient, rcDefault, NULL, 
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, WS_EX_STATICEDGE);

	return ;
}

void CMainFrame::CreateStackListView()
{
	ATLASSERT(::IsWindow(m_hWndClient));

	m_ApiListView.Create(m_hWndClient, rcDefault, NULL, 
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS , WS_EX_STATICEDGE);

	m_ApiListView.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_UNDERLINEHOT);

	m_ApiListView.InsertColumn(0, _T("时间"), LVCFMT_LEFT, 80);
	m_ApiListView.InsertColumn(1, _T("函数"), LVCFMT_LEFT, 200);
	m_ApiListView.InsertColumn(2, _T("句柄值"), LVCFMT_LEFT, 80);
	m_ApiListView.InsertColumn(3, _T("关闭函数"), LVCFMT_LEFT, 200);

	return ;
}

void CMainFrame::SetWindowNameSuffix(LPCTSTR suffix)
{
	if (NULL == suffix)
	{
		SetWindowText(m_strWindowName);
	}

	CString strNewWindowName;
	strNewWindowName = m_strWindowName + _T(" - ") + suffix;
	SetWindowText(strNewWindowName);
}

void CMainFrame::CleanAndClose()
{
	m_Progress.Go(m_hWnd, _T("正在关闭..."), 5);

	CStackStorage::GetInstance()->Close();
	CSymbolHandler::GetInstance()->ClearAllSymbols();
	m_Progress.StepIt();

	m_ApiListView.DeleteAllItems();
	m_Progress.StepIt();

	m_ChartView.ResetLineChart();
	m_Progress.StepIt();

	m_CountArray.clear();
	m_Progress.StepIt();

	m_CallStackArray.clear();
	m_Progress.StepIt();

	m_ModInfo.clear();
	m_Progress.StepIt();

	m_Progress.Stop();
}

void CMainFrame::AppendCountData()
{
	CStackStorage::GetInstance()->SetExtendedData(m_CountArray);

	return ;
}

BOOL CMainFrame::DoDetect(DWORD dwPid)
{
	CDetectDlg DetectDlg;

	DetectDlg.SetArrayPointer(&m_CountArray);
	if (IDOK == DetectDlg.DoModal(m_hWnd, (LPARAM)dwPid))
	{
		m_Progress.Go(m_hWnd, _T("正在处理数据..."), 4);
		AppendCountData();

		m_Progress.StepIt(NULL);
		FillStackListView();

		m_Progress.StepIt(NULL);
		FillModInfoArray();

		m_Progress.StepIt(_T("绘制线条图表..."));
		m_ChartView.DrawLineChart(&m_CountArray);

		m_Progress.StepIt(_T("加载符号文件..."));
		CSymbolHandler::GetInstance()->ClearAllSymbols();
		CSymbolHandler::GetInstance()->LoadSymbols(m_ModInfo);

		m_Progress.StepIt(NULL);
		m_Progress.Stop();

		return TRUE;
	}

	return FALSE;
}

void CMainFrame::FillModInfoArray()
{
	m_ModInfo.clear();
	CStackStorage::GetInstance()->GetModInfoData(m_ModInfo);
}

void CMainFrame::FillStackListView()
{
	m_CallStackArray.clear();
	CStackStorage::GetInstance()->GetCallStackData(m_CallStackArray);

	m_ApiListView.DeleteAllItems();

	CString strTemp;
	for (UINT i=0; i<m_CallStackArray.size(); i++)
	{
		strTemp.Format(_T("%d"), m_CallStackArray[i]->dwTimeStamp);
		m_ApiListView.InsertItem(i, strTemp);

		m_ApiListView.SetItemText(i, 1, CApi::GetNameByIndex(m_CallStackArray[i]->Type));

		strTemp.Format(_T("0x%08X"), m_CallStackArray[i]->Handle);
		m_ApiListView.SetItemText(i, 2, strTemp);

		m_ApiListView.SetItemData(i, NULL);
	}

	return ;
}

void CMainFrame::FillCountTimeArray()
{
	m_CountArray.clear();
	CStackStorage::GetInstance()->GetExtendedData(m_CountArray);
}
