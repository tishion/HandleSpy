#include "stdafx.h"
#include "FuncCallDlg.h"
#include "Api.h"
#include "ApiIndex.h"
#include "NtApiIndex.h"

CFuncCallDlg::CFuncCallDlg(DWORD dwBegin, DWORD dwEnd)
{
	m_BeginPos = dwBegin;
	m_EndPos = dwEnd;
}

CFuncCallDlg::~CFuncCallDlg(void)
{
	ReleaseAllStackDlg();
}

void CFuncCallDlg::ShowStackDlg(CALL_STACK* pCs, LPCTSTR lpText)
{
	std::map<CALL_STACK*, CStackDlg*>::iterator it;
	
	it = m_CallstackDlgMap.find(pCs);
	if (it == m_CallstackDlgMap.end())
	{
		m_CallstackDlgMap[pCs] = NULL;
	}

	it = m_CallstackDlgMap.find(pCs);

	if (NULL == it->second || FALSE == it->second->IsWindow())
	{
		it->second = new CStackDlg;
		it->second->Create(m_hWnd, (LPARAM)pCs);
		it->second->SetWindowText(lpText);
	}
	else
	{
		it->second->SetActiveWindow();
	}

	it->second->ShowWindow(SW_NORMAL);
}

void CFuncCallDlg::ReleaseAllStackDlg()
{
	std::map<CALL_STACK*, CStackDlg*>::iterator it;

	for (it=m_CallstackDlgMap.begin(); it!=m_CallstackDlgMap.end(); it++)
	{
		if (NULL != it->second && TRUE == it->second->IsWindow())
		{
			delete it->second;
		}
	}
}

BOOL CFuncCallDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	m_listFuncCall = GetDlgItem(IDC_LIST_FUNC_CALL);

	m_listFuncCall.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);
	m_listFuncCall.InsertColumn(0, _T("序号"), LVCFMT_LEFT, 60);
	m_listFuncCall.InsertColumn(1, _T("调用时间"), LVCFMT_LEFT, 80);
	m_listFuncCall.InsertColumn(2, _T("函数名称"), LVCFMT_LEFT, 200);	
	m_listFuncCall.InsertColumn(3, _T("句柄值"), LVCFMT_LEFT, 80);

	CString strTemp;
	std::vector<CALL_STACK*>* pVec = (std::vector<CALL_STACK*>*)lParam;

	if ((NULL != pVec) && (false == pVec->empty()))
	{
		for (UINT i=m_BeginPos; i<=m_EndPos; i++)
		{
			m_AllSrcVec.push_back(pVec->at(i));
		}
	}

	GetDlgItem(IDC_BUTTON_ONLYLEAK).EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SHOWALL).EnableWindow(TRUE);
	ShowLeak();

	CenterWindow(GetParent());

	return TRUE;
}

LRESULT CFuncCallDlg::OnNmDbClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	POINT pt = { 0, 0 };
	GetCursorPos(&pt);
	POINT ptClient = pt;
	if(pnmh->hwndFrom != NULL)
	{
		::ScreenToClient(pnmh->hwndFrom, &ptClient);
	}

	if(pnmh->hwndFrom == m_listFuncCall.m_hWnd)
	{
		LVHITTESTINFO lvhti = { 0 };
		lvhti.pt = ptClient;
		m_listFuncCall.HitTest(&lvhti);

		if (lvhti.iItem != -1)
		{
			CString strFuncName;
			CString strFuncTime;
			CString strWindowText = _T("调用栈：");

			if (0 != m_listFuncCall.GetItemText(lvhti.iItem, 1, strFuncTime))
			{
				strWindowText += strFuncTime;
			}
			strWindowText += _T("_");
			if (0 != m_listFuncCall.GetItemText(lvhti.iItem, 2, strFuncName))
			{
				strWindowText += strFuncName;
			}

			PCALL_STACK pCs = (PCALL_STACK)m_listFuncCall.GetItemData(lvhti.iItem);

			ShowStackDlg(pCs, strWindowText);
		}
	}

	return 0;
}

LRESULT CFuncCallDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}


LRESULT CFuncCallDlg::OnBnClickedCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(IDCANCEL);

	return 0;
}

LRESULT CFuncCallDlg::OnBnClickedButtonShowall(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	GetDlgItem(IDC_BUTTON_SHOWALL).EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ONLYLEAK).EnableWindow(TRUE);
	
	ShowAll();

	return 0;
}

LRESULT CFuncCallDlg::OnBnClickedButtonOnlyleak(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	GetDlgItem(IDC_BUTTON_ONLYLEAK).EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SHOWALL).EnableWindow(TRUE);
	
	ShowLeak();

	return 0;
}

void CFuncCallDlg::ShowAll()
{
	m_Progress.Go(m_hWnd, _T("正在执行操作..."), m_AllSrcVec.size());

	m_listFuncCall.DeleteAllItems();

	CString strTemp;

	for (UINT i=0; i<m_AllSrcVec.size(); i++)
	{
		strTemp.Format(_T("%d"), i+1);
		m_listFuncCall.InsertItem(i, strTemp);

		strTemp.Format(_T("%d"), m_AllSrcVec[i]->dwTimeStamp);
		m_listFuncCall.SetItemText(i, 1, strTemp);

		m_listFuncCall.SetItemText(i, 2, CApi::GetNameByIndex(m_AllSrcVec[i]->Type));

		strTemp.Format(_T("0x%08X"), m_AllSrcVec[i]->Handle);
		m_listFuncCall.SetItemText(i, 3, strTemp);

		m_listFuncCall.SetItemData(i, (DWORD_PTR)(m_AllSrcVec[i]));

		m_Progress.StepIt(NULL);
	}

	m_Progress.Stop();
}

void CFuncCallDlg::ShowLeak()
{
	CString strTemp;

	if (true == m_AdvanceFilterVec.empty())
	{
		std::vector<CALL_STACK*> VecTempSrc = m_AllSrcVec;

		AdvanceFilter(VecTempSrc, m_AdvanceFilterVec);
	}

	m_Progress.Go(m_hWnd, _T("正在执行操作..."), m_AdvanceFilterVec.size());

	m_listFuncCall.DeleteAllItems();

	for (UINT i=0; i<m_AdvanceFilterVec.size(); i++)
	{
		m_Progress.StepIt(NULL);

		strTemp.Format(_T("%d"), i+1);
		m_listFuncCall.InsertItem(i, strTemp);

		strTemp.Format(_T("%d"), m_AdvanceFilterVec[i]->dwTimeStamp);
		m_listFuncCall.SetItemText(i, 1, strTemp);

		m_listFuncCall.SetItemText(i, 2, CApi::GetNameByIndex(m_AdvanceFilterVec[i]->Type));

		strTemp.Format(_T("0x%08X"), m_AdvanceFilterVec[i]->Handle);
		m_listFuncCall.SetItemText(i, 3, strTemp);

		m_listFuncCall.SetItemData(i, (DWORD_PTR)(m_AdvanceFilterVec[i]));
	}

	m_Progress.Stop();
}

void CFuncCallDlg::AdvanceFilter(std::vector<CALL_STACK*>& vecSrc, std::vector<CALL_STACK*>& vecDst)
{
	m_Progress.Go(m_hWnd, _T("正在过滤..."), vecSrc.size());
	
	for (UINT i=0; i<vecSrc.size(); i++)
	{
		m_Progress.StepIt(NULL);

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
					if (vecSrc[j]->Type > Index_Close_Begin && vecSrc[j]->Type < Index_Close_End)
					{// 是关闭操作，找到匹配的过滤项，赋值NULL
						vecSrc[i] = NULL;
						vecSrc[j] = NULL;

						break;
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


/*
 * 去掉vecSrc中符合下列条件的项
 * 1、相邻的两项
 * 2、前一个为打开句柄操作，后一个为关闭句柄操作
 * 3、两次操作的句柄值相等
 * 将去掉上述项的数据保存在vecDst中
 */
BOOL CFuncCallDlg::SimpleFilter(const std::vector<CALL_STACK*>& vecSrc, std::vector<CALL_STACK*>& vecDst)
{
	if (1 == vecSrc.size())
	{
		vecDst.push_back(vecSrc[0]);
	}

	for (UINT i=0; i<vecSrc.size(); )
	{
		if (i+1 == vecSrc.size())
		{// i最多只能是最后一个，所以i+1最多只能是size
			vecDst.push_back(vecSrc[i]);
			break;
		}

		if (vecSrc[i]->Type > Index_Close_End && vecSrc[i+1]->Type <= Index_Close_End)
		{// 符合，进入判断句柄值是否相等，无论句柄值是否相等，都后移两位
			if (vecSrc[i]->Handle != vecSrc[i+1]->Handle)
			{// 句柄值不相等，保留这两个
				vecDst.push_back(vecSrc[i]);
				vecDst.push_back(vecSrc[i+1]);
				i = i + 2;
			}
			else
			{// 句柄值相等，过滤这两个
				i = i + 2;					//后移两位
			}
		}
		else
		{// 不符合，保留第一个，后移一位，继续判断
			vecDst.push_back(vecSrc[i]);
			i = i + 1;
		}
	}

	return (vecSrc.size() != vecDst.size());
}

LRESULT CFuncCallDlg::OnBnClickedButtonSmartfilter(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	INT nIndex = 0;

	CString strTemp;

	m_Progress.Go(m_hWnd, _T("正在执行过滤操作..."), 2);

	m_listFuncCall.DeleteAllItems();

	if (true == m_SimpleFilterVec.empty())
	{
		std::vector<CALL_STACK*> VecTempSrc = m_AllSrcVec;

		while (TRUE == SimpleFilter(VecTempSrc, m_SimpleFilterVec))
		{
			VecTempSrc = m_SimpleFilterVec;
			m_SimpleFilterVec.clear();
		}
	}

	m_Progress.StepIt(NULL);

	for (UINT i=0; i<m_SimpleFilterVec.size(); i++)
	{
		strTemp.Format(_T("%d"), i+1);
		m_listFuncCall.InsertItem(i, strTemp);

		strTemp.Format(_T("%d"), m_SimpleFilterVec[i]->dwTimeStamp);
		m_listFuncCall.SetItemText(i, 1, strTemp);

		m_listFuncCall.SetItemText(i, 2, CApi::GetNameByIndex(m_SimpleFilterVec[i]->Type));

		strTemp.Format(_T("0x%08X"), m_SimpleFilterVec[i]->Handle);
		m_listFuncCall.SetItemText(i, 3, strTemp);

		m_listFuncCall.SetItemData(i, (DWORD_PTR)(m_SimpleFilterVec[i]));
	}

	m_Progress.StepIt(NULL);

	m_Progress.Stop();

	return 0;
}