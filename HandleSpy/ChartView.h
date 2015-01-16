/************************************************************************\
 * File		:	ChartView.h
 * Module	:	Chart
 * Created	:	shiontian @ 2013-01-05
 * Description:
 * 功能简单的动态图表控件
 *
\************************************************************************/
#pragma once
#include "atlwin.h"
#include "atlctrls.h"

class CChartView :
	public CWindowImpl<CChartView>
{
public:
	DECLARE_WND_CLASS(_T("CHART_VIEW_CONTROL"))
	
	BOOL PreTranslateMessage(MSG* pMsg);
	
	BEGIN_MSG_MAP(CChartView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	END_MSG_MAP()

	CChartView(void);
	~CChartView(void);

	//void InitChart();
	void SetRangeAndFirstValue(DWORD low, DWORD up, DWORD initVal);

	void Update(DWORD val);

protected:
	void InternalUpdate();

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
	CDC			m_MemDC;
	CBrush		m_HatchBrush;
	Pen*		m_pLinePen;
	

	// Point
	DWORD		m_nLastVal;
	DWORD		m_nCurVal;

	DWORD		m_dwUpperBound;
	DWORD		m_dwLowerBound;

	CFont		m_Font;

	CRect		m_rcTextout;

};
