#include "StdAfx.h"
#include "LeakedFunCallDlg.h"
#include "Api.h"
#include "ApiIndex.h"
#include "NtApiIndex.h"

CLeakedFunCallDlg::CLeakedFunCallDlg(DWORD dwBegin, DWORD dwEnd)
{
	m_BeginPos = dwBegin;
	m_EndPos = dwEnd;
}

CLeakedFunCallDlg::~CLeakedFunCallDlg(void)
{

}

#define BUTTON_PADDING	3
BOOL CLeakedFunCallDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	m_treeLeaked = GetDlgItem(IDC_TREE_LEAKED);
	m_treeAll = GetDlgItem(IDC_TREE_ALL);
	m_btnAll = GetDlgItem(IDC_BUTTON_ALL);
	m_btnLeaked = GetDlgItem(IDC_BUTTON_LEAKED);
	m_btnQuit = GetDlgItem(IDCANCEL);

	CRect rcButton;
	m_btnLeaked.GetClientRect(rcButton);

	m_sizeButton.cx = rcButton.Width();
	m_sizeButton.cy = rcButton.Height();
	m_dwHeightOfButtonsPane = rcButton.Height() + 2 * BUTTON_PADDING;

	CRect rcClient;
	GetClientRect(rcClient);

	CRect rcTreeCtrl = rcClient;
	rcTreeCtrl.bottom = rcTreeCtrl.bottom - m_dwHeightOfButtonsPane;
	m_treeAll.MoveWindow(rcTreeCtrl);
	m_treeLeaked.MoveWindow(rcTreeCtrl);

	rcButton = rcTreeCtrl;
	rcButton.top = rcTreeCtrl.bottom + BUTTON_PADDING;
	rcButton.bottom = rcButton.top + m_sizeButton.cy;

	rcButton.left += BUTTON_PADDING;
	rcButton.right = rcButton.left + m_sizeButton.cx;
	m_btnLeaked.MoveWindow(rcButton);

	rcButton.left = rcButton.right + BUTTON_PADDING;
	rcButton.right = rcButton.left + m_sizeButton.cx;
	m_btnAll.MoveWindow(rcButton);

	rcButton.right = rcTreeCtrl.right - BUTTON_PADDING;
	rcButton.left = rcButton.right - m_sizeButton.cx;
	m_btnQuit.MoveWindow(rcButton);

	m_treeAll.DeleteAllItems();
	m_treeLeaked.DeleteAllItems();

	m_treeAll.ShowWindow(SW_HIDE);
	m_treeLeaked.ShowWindow(SW_SHOW);

	CString strTemp;
	std::vector<CALL_STACK*>* pVec = (std::vector<CALL_STACK*>*)lParam;

	if ((NULL != pVec) && (false == pVec->empty()))
	{
		for (UINT i=m_BeginPos; i<=m_EndPos; i++)
		{
			m_vecAllCall.push_back(pVec->at(i));
		}
	}

	ShowLeaked();

	CenterWindow(GetParent());

	return TRUE;
}

LRESULT CLeakedFunCallDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

void CLeakedFunCallDlg::ShowLeaked()
{
	if (m_treeLeaked.GetCount() == 0)
	{
		CString strTemp;
		StringT strFrameName;
		StringT strSourceFileName;

		if (true == m_vecLeakedCall.empty())
		{
			std::vector<CALL_STACK*> VecTempSrc = m_vecAllCall;

			AdvanceFilter(VecTempSrc, m_vecLeakedCall);
		}

		m_Progress.Go(m_hWnd, _T("数据处理中..."), m_vecLeakedCall.size());

		HTREEITEM htiFunc = NULL;
		HTREEITEM htiFrame = NULL;
		DWORD dwLineNumber = 0;

		for (UINT i=0; i<m_vecLeakedCall.size(); i++)
		{
			m_Progress.StepIt(NULL);

			// + 0000|000000000|FunctionName|[0x00000000] 
			strTemp.Format(_T("%03d | %09d | %s | [0x%08X]"), 
				i+1, m_vecLeakedCall[i]->dwTimeStamp, 
				CApi::GetNameByIndex(m_vecLeakedCall[i]->Type),
				m_vecLeakedCall[i]->Handle);
			htiFunc = m_treeLeaked.InsertItem(strTemp, TVI_ROOT, TVI_LAST);

			for (UINT j=0; j<m_vecLeakedCall[i]->nFrameCount; j++)
			{
				strFrameName = CSymbolHandler::GetInstance()->FrameNameFromAddr(m_vecLeakedCall[i]->frame[j]);

				strTemp.Format(_T("%03d | %s"), j+1, strFrameName.c_str());
				htiFrame = m_treeLeaked.InsertItem(strTemp, htiFunc, TVI_LAST);

				if (TRUE == CSymbolHandler::GetInstance()->GetSourceFileAndLineNumber(m_vecLeakedCall[i]->frame[j], strSourceFileName, dwLineNumber))
				{
					strTemp.Format(_T("%s [%04d]"), strSourceFileName.c_str(), dwLineNumber);
					m_treeLeaked.InsertItem(strTemp, htiFrame, TVI_LAST);
				}
				//m_treeLeaked.Expand(htiFrame);
			}
			//m_treeLeaked.Expand(htiFunc);
		}

		m_treeLeaked.EnsureVisible(m_treeLeaked.GetRootItem());

		m_Progress.Stop();
	}
}

void CLeakedFunCallDlg::ShowAll()
{
	if (m_treeAll.GetCount() == 0)
	{
		CString strTemp;
		StringT strFrameName;
		StringT strSourceFileName;

		m_Progress.Go(m_hWnd, _T("数据处理中..."), m_vecAllCall.size());

		HTREEITEM htiFunc = NULL;
		HTREEITEM htiFrame = NULL;
		DWORD dwLineNumber = 0;

		for (UINT i=0; i<m_vecAllCall.size(); i++)
		{
			m_Progress.StepIt();

			// + 0000|000000000|FunctionName|[0x00000000] 
			strTemp.Format(_T("%03d | %09d | %s | [0x%08X]"), 
				i+1, m_vecAllCall[i]->dwTimeStamp, 
				CApi::GetNameByIndex(m_vecAllCall[i]->Type),
				m_vecAllCall[i]->Handle);
			htiFunc = m_treeAll.InsertItem(strTemp, TVI_ROOT, TVI_LAST);

			for (UINT j=0; j<m_vecAllCall[i]->nFrameCount; j++)
			{
				strFrameName = CSymbolHandler::GetInstance()->FrameNameFromAddr(m_vecAllCall[i]->frame[j]);

				strTemp.Format(_T("%03d | %s"), j+1, strFrameName.c_str());
				htiFrame = m_treeAll.InsertItem(strTemp, htiFunc, TVI_LAST);

				if (TRUE == CSymbolHandler::GetInstance()->GetSourceFileAndLineNumber(m_vecAllCall[i]->frame[j], strSourceFileName, dwLineNumber))
				{
					strTemp.Format(_T("%s [%04d]"), strSourceFileName.c_str(), dwLineNumber);
					m_treeAll.InsertItem(strTemp, htiFrame, TVI_LAST);
				}
				//m_treeAll.Expand(htiFrame);
			}
			//m_treeAll.Expand(htiFunc);
		}

		m_treeAll.EnsureVisible(m_treeAll.GetRootItem());
		
		m_Progress.Stop();
	}
}


void CLeakedFunCallDlg::AdvanceFilter(std::vector<CALL_STACK*>& vecSrc, std::vector<CALL_STACK*>& vecDst)
{
	m_Progress.Go(m_hWnd, _T("正在过滤..."), vecSrc.size());

	for (UINT i=0; i<vecSrc.size(); i++)
	{
		m_Progress.StepIt();

		if (NULL == vecSrc[i])
		{
			continue;
		}

		if (vecSrc[i]->Type > Index_Close_End)
		{// 如果是打开操作，则执行判断
			for (UINT j=i+1; j<vecSrc.size(); j++)
			{// 向后查找第一个句柄值相同的 关闭操作

				if (NULL == vecSrc[j])
				{//  空指针直接跳过
					continue;
				}

				if (vecSrc[j]->Handle == vecSrc[i]->Handle)
				{// 句柄值相同，判断是否为关闭操作
					if (vecSrc[j]->Type < Index_Close_End && vecSrc[j]->Type > Index_Close_Begin)
					{// 是关闭操作，找到匹配的过滤项，赋值NULL
						vecSrc[i] = NULL;
						vecSrc[j] = NULL;

						break;
					}
					else
					{// 不是关闭操作，这里是什么原因？一个句柄没有被关闭，却有被再次打开了？？？
						//[//无论如何，没有找到关闭，到此为止，不再继续搜寻关闭项，因为找到的关闭项很可能关闭的是这次打开的句柄。]这种处理方法有问题！！
						//同一个句柄值被打开两次，那就记作打开，并且向后找关闭操作
						vecSrc[j] = NULL;
					}
				}
			}
		}
		else
		{// 是关闭操作，说明该操之前没有与之匹配的打开操作，赋值NULL
			vecSrc[i] = NULL;
		}
	}

	m_Progress.Stop();

	for (UINT i=0; i<vecSrc.size(); i++)
	{
		if (NULL == vecSrc[i])
		{
			continue;
		}

		vecDst.push_back(vecSrc[i]);
	}

	return ;
}

LRESULT CLeakedFunCallDlg::OnBnClickedButtonLeaked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_treeAll.ShowWindow(SW_HIDE);
	
	ShowLeaked();

	m_treeLeaked.ShowWindow(SW_SHOW);

	return 0;
}

LRESULT CLeakedFunCallDlg::OnBnClickedButtonAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_treeLeaked.ShowWindow(SW_HIDE);

	ShowAll();

	m_treeAll.ShowWindow(SW_SHOW);

	return 0;
}

LRESULT CLeakedFunCallDlg::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CRect rcClient;
	GetClientRect(rcClient);

	CRect rcTreeCtrl = rcClient;
	rcTreeCtrl.bottom = rcTreeCtrl.bottom - m_dwHeightOfButtonsPane;
	m_treeAll.MoveWindow(rcTreeCtrl);
	m_treeLeaked.MoveWindow(rcTreeCtrl);

	CRect rcButton = rcTreeCtrl;
	rcButton.top = rcTreeCtrl.bottom + BUTTON_PADDING;
	rcButton.bottom = rcButton.top + m_sizeButton.cy;

	rcButton.left += BUTTON_PADDING;
	rcButton.right = rcButton.left + m_sizeButton.cx;
	m_btnLeaked.MoveWindow(rcButton);

	rcButton.left = rcButton.right + BUTTON_PADDING;
	rcButton.right = rcButton.left + m_sizeButton.cx;
	m_btnAll.MoveWindow(rcButton);

	rcButton.right = rcTreeCtrl.right - BUTTON_PADDING;
	rcButton.left = rcButton.right - m_sizeButton.cx;
	m_btnQuit.MoveWindow(rcButton);

	return 0;
}

LRESULT CLeakedFunCallDlg::OnNMRclickTree(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	POINT pt = { 0, 0 };
	GetCursorPos(&pt);
	POINT ptClient = pt;
	if(pNMHDR->hwndFrom != NULL)
	{
		::ScreenToClient(pNMHDR->hwndFrom, &ptClient);
	}

	if(pNMHDR->hwndFrom == m_treeAll.m_hWnd || pNMHDR->hwndFrom == m_treeLeaked)
	{
		CTreeViewCtrl tvCtrl = pNMHDR->hwndFrom;

		TVHITTESTINFO tvhti = { 0 };
		tvhti.pt = ptClient;
		tvCtrl.HitTest(&tvhti);

		if ((tvhti.flags & (TVHT_ONITEM | TVHT_ONITEMINDENT | TVHT_ONITEMRIGHT)) != 0)
		{
			tvCtrl.Select(tvhti.hItem, TVGN_CARET);

			CMenu menu; 
			menu.LoadMenu(IDR_MENU_POPUP); 
			CMenuHandle MenuPopup = menu.GetSubMenu(1); 
			MenuPopup.TrackPopupMenu(TPM_RIGHTBUTTON | TPM_VERPOSANIMATION | TPM_VERTICAL, 
				pt.x, pt.y, m_hWnd);
		}
	}

	return 0;
}

LRESULT CLeakedFunCallDlg::OnFuncExpandAllStacks(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CTreeViewCtrl tvCtrl;

	if (TRUE == m_treeAll.IsWindowVisible())
	{
		tvCtrl = m_treeAll;
	}
	else if (TRUE == m_treeLeaked.IsWindowVisible())
	{
		tvCtrl = m_treeLeaked;
	}
	else
	{
		return 0;
	}

	HTREEITEM htiFun = tvCtrl.GetRootItem();

	while (NULL != htiFun)
	{
		tvCtrl.Expand(htiFun);
		htiFun = tvCtrl.GetNextSiblingItem(htiFun);
	}

	tvCtrl.EnsureVisible(tvCtrl.GetRootItem());

	return 0;
}

LRESULT CLeakedFunCallDlg::OnFuncCollapseAllStacks(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CTreeViewCtrl tvCtrl;

	if (TRUE == m_treeAll.IsWindowVisible())
	{
		tvCtrl = m_treeAll;
	}
	else if (TRUE == m_treeLeaked.IsWindowVisible())
	{
		tvCtrl = m_treeLeaked;
	}
	else
	{
		return 0;
	}

	HTREEITEM htiFun = tvCtrl.GetRootItem();

	while (NULL != htiFun)
	{
		tvCtrl.Expand(htiFun, TVE_COLLAPSE);
		htiFun = tvCtrl.GetNextSiblingItem(htiFun);
	}

	tvCtrl.EnsureVisible(tvCtrl.GetRootItem());

	return 0;
}

LRESULT CLeakedFunCallDlg::OnFuncExpandAllSource(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CTreeViewCtrl tvCtrl;

	if (TRUE == m_treeAll.IsWindowVisible())
	{
		tvCtrl = m_treeAll;
	}
	else if (TRUE == m_treeLeaked.IsWindowVisible())
	{
		tvCtrl = m_treeLeaked;
	}
	else
	{
		return 0;
	}

	HTREEITEM htiFun = tvCtrl.GetRootItem();
	HTREEITEM htiFrame = NULL;

	while (NULL != htiFun)
	{
		if (TRUE == tvCtrl.ItemHasChildren(htiFun))
		{
			htiFrame = tvCtrl.GetChildItem(htiFun);

			while (NULL != htiFrame)
			{
				tvCtrl.Expand(htiFrame);
				htiFrame = tvCtrl.GetNextSiblingItem(htiFrame);
			}
		}
		htiFun = tvCtrl.GetNextSiblingItem(htiFun);
	}

	tvCtrl.EnsureVisible(tvCtrl.GetRootItem());

	return 0;
}

LRESULT CLeakedFunCallDlg::OnFuncCollapseAllSource(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CTreeViewCtrl tvCtrl;

	if (TRUE == m_treeAll.IsWindowVisible())
	{
		tvCtrl = m_treeAll;
	}
	else if (TRUE == m_treeLeaked.IsWindowVisible())
	{
		tvCtrl = m_treeLeaked;
	}
	else
	{
		return 0;
	}

	HTREEITEM htiFun = tvCtrl.GetRootItem();
	HTREEITEM htiFrame = NULL;

	while (NULL != htiFun)
	{
		if (TRUE == tvCtrl.ItemHasChildren(htiFun))
		{
			htiFrame = tvCtrl.GetChildItem(htiFun);

			while (NULL != htiFrame)
			{
				tvCtrl.Expand(htiFrame, TVE_COLLAPSE);
				htiFrame = tvCtrl.GetNextSiblingItem(htiFrame);
			}
		}
		htiFun = tvCtrl.GetNextSiblingItem(htiFun);
	}

	tvCtrl.EnsureVisible(tvCtrl.GetRootItem());

	return 0;
}

LRESULT CLeakedFunCallDlg::OnFuncCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CTreeViewCtrl tvCtrl;

	if (TRUE == m_treeAll.IsWindowVisible())
	{
		tvCtrl = m_treeAll;
	}
	else if (TRUE == m_treeLeaked.IsWindowVisible())
	{
		tvCtrl = m_treeLeaked;
	}
	else
	{
		return 0;
	}

	HTREEITEM htiSelected = tvCtrl.GetSelectedItem();
	if (NULL == htiSelected)
	{
		return FALSE;
	}
	
	CString strSelectedText;

	if (FALSE == tvCtrl.GetItemText(htiSelected, strSelectedText))
	{
		return FALSE;
	}

	if (FALSE == (TRUE == OpenClipboard() && TRUE == EmptyClipboard()))
	{
		return FALSE;
	}

	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (strSelectedText.GetLength() + 1) * sizeof(TCHAR)); 
	if (NULL == hglbCopy) 
	{ 
		CloseClipboard(); 
		return FALSE; 
	} 

	LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hglbCopy);

	RtlCopyMemory(lptstrCopy, strSelectedText.GetString(), strSelectedText.GetLength() * sizeof(TCHAR)); 
	lptstrCopy[strSelectedText.GetLength()] = (TCHAR)0;

	GlobalUnlock(hglbCopy); 

	SetClipboardData(CF_UNICODETEXT, hglbCopy); 

	CloseClipboard(); 

	return 0;
}

LRESULT CLeakedFunCallDlg::OnFuncCopystack(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CTreeViewCtrl tvCtrl;

	if (TRUE == m_treeAll.IsWindowVisible())
	{
		tvCtrl = m_treeAll;
	}
	else if (TRUE == m_treeLeaked.IsWindowVisible())
	{
		tvCtrl = m_treeLeaked;
	}
	else
	{
		return 0;
	}

	HTREEITEM htiSelected = tvCtrl.GetSelectedItem();
	if (NULL == htiSelected)
	{
		return FALSE;
	}

	HTREEITEM htiParent = NULL;
	HTREEITEM htiRootParent = tvCtrl.GetParentItem(htiSelected);
	
	while (NULL != htiRootParent)
	{
		htiParent = htiRootParent;
		htiRootParent = tvCtrl.GetParentItem(htiRootParent);
	}

	CString strStackText;
	CString strTemp;

	if (FALSE == tvCtrl.GetItemText(htiParent, strTemp))
	{
		return FALSE;
	}

	//strStackText.Append(strTemp);
	//strStackText.Append(_T("\r\n"));

	//if (TRUE == tvCtrl.ItemHasChildren(htiParent))
	//{
	//	HTREEITEM hti = tvCtrl.GetChildItem(htiParent);

	//	while (NULL != hti)
	//	{
	//		tvCtrl.GetItemText(hti, strTemp);
	//		strStackText.Append(_T("\t"));
	//		strStackText.Append(strTemp);
	//	}



	//}
	//

	if (FALSE == (TRUE == OpenClipboard() && TRUE == EmptyClipboard()))
	{
		return FALSE;
	}

	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (strStackText.GetLength() + 1) * sizeof(TCHAR)); 
	if (NULL == hglbCopy) 
	{ 
		CloseClipboard(); 
		return FALSE; 
	} 

	LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hglbCopy);

	RtlCopyMemory(lptstrCopy, strStackText.GetString(), strStackText.GetLength() * sizeof(TCHAR)); 
	lptstrCopy[strStackText.GetLength()] = (TCHAR)0;

	GlobalUnlock(hglbCopy); 

	SetClipboardData(CF_UNICODETEXT, hglbCopy); 

	CloseClipboard(); 

	return 0;
}

LRESULT CLeakedFunCallDlg::OnFuncExport2txt(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	HTREEITEM htiFun = m_treeLeaked.GetRootItem();
	HTREEITEM hFrame = NULL;
	HTREEITEM hSource = NULL;

	CString strLeakedList;
	CString strTemp;

	while (NULL != htiFun)
	{
		m_treeLeaked.GetItemText(htiFun, strTemp);
		strLeakedList.Append(strTemp);
		strLeakedList.Append(_T("\r\n"));

		hFrame = m_treeLeaked.GetChildItem(htiFun);

		while (NULL != hFrame)
		{
			strLeakedList.Append(_T("\t"));
			m_treeLeaked.GetItemText(hFrame, strTemp);
			strLeakedList.Append(strTemp);
			strLeakedList.Append(_T("\r\n"));

			hSource = m_treeLeaked.GetChildItem(hFrame);

			while (NULL != hSource)
			{
				strLeakedList.Append(_T("\t\t"));
				m_treeLeaked.GetItemText(hSource, strTemp);
				strLeakedList.Append(strTemp);
				strLeakedList.Append(_T("\r\n"));

				hSource = m_treeLeaked.GetNextSiblingItem(hSource);
			}

			hFrame = m_treeLeaked.GetNextSiblingItem(hFrame);
		}
		
		htiFun = m_treeLeaked.GetNextSiblingItem(htiFun);
	}

	int i = strLeakedList.GetLength();

	return 0;
}

LRESULT CLeakedFunCallDlg::OnFuncCopyallleaked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	HTREEITEM htiFun = m_treeLeaked.GetRootItem();
	HTREEITEM hFrame = NULL;
	HTREEITEM hSource = NULL;

	CString strLeakedList;
	CString strTemp;

	while (NULL != htiFun)
	{
		m_treeLeaked.GetItemText(htiFun, strTemp);
		strLeakedList.Append(strTemp);
		strLeakedList.Append(_T("\r\n"));

		hFrame = m_treeLeaked.GetChildItem(htiFun);

		while (NULL != hFrame)
		{
			strLeakedList.Append(_T("\t"));
			m_treeLeaked.GetItemText(hFrame, strTemp);
			strLeakedList.Append(strTemp);
			

			hSource = m_treeLeaked.GetChildItem(hFrame);

			if (NULL != hSource)
			{
				strLeakedList.Append(_T("\t[SourceFile]"));
				m_treeLeaked.GetItemText(hSource, strTemp);
				strLeakedList.Append(strTemp);
			}

			strLeakedList.Append(_T("\r\n"));

			hFrame = m_treeLeaked.GetNextSiblingItem(hFrame);
		}

		htiFun = m_treeLeaked.GetNextSiblingItem(htiFun);
	}

	if (FALSE == (TRUE == OpenClipboard() && TRUE == EmptyClipboard()))
	{
		return FALSE;
	}

	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (strLeakedList.GetLength() + 1) * sizeof(TCHAR)); 
	if (NULL == hglbCopy) 
	{ 
		CloseClipboard(); 
		return FALSE; 
	} 

	LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hglbCopy);

	RtlCopyMemory(lptstrCopy, strLeakedList.GetString(), strLeakedList.GetLength() * sizeof(TCHAR)); 
	lptstrCopy[strLeakedList.GetLength()] = (TCHAR)0;

	GlobalUnlock(hglbCopy); 

	SetClipboardData(CF_UNICODETEXT, hglbCopy); 

	CloseClipboard(); 

	return 0;
}
