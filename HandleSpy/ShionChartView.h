/************************************************************************\
 * File		:	ShionChartView.h
 * Module	:	ShionChartView
 * Created	:	shiontian @ 2013-01-05
 * Description:
 * 功能丰富的静态图表控件
 *
\************************************************************************/
#pragma once
#include "atlwin.h"
#include "atlscrl.h"
#include "CountAndTimeTypeDefine.h"

class CShionChartView :
	public CScrollWindowImpl<CShionChartView>
{
public:
	DECLARE_WND_CLASS(_T("SHIONT_CHART_VIEW_CONTROL"))

	BOOL PreTranslateMessage(MSG* pMsg);

	typedef enum {
		SelectStatus_SelectNone,
		SelectStatus_Selecting,
		SelectStatus_Selected	
	}SelectStatus;

	BEGIN_MSG_MAP(CChartView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_RBUTTONUP, OnRButtonUp)
		CHAIN_MSG_MAP(CScrollWindowImpl<CShionChartView>)
	END_MSG_MAP()

	CShionChartView(void);
	~CShionChartView(void);

	void ResetLineChart();
	void DrawLineChart(std::vector<COUNT_TIME>* p);
	void DoPaint(CDCHandle /*dc*/);
	void GetSelectedIndexRange(DWORD& dwBegin, DWORD& dwEnd);
	BOOL IsSelected();

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);	
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnRButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
	CDC				m_ImageDC;
	CDC				m_CanvasDC;
	Pen*			m_pChartLinePen;
	DWORD			m_CanvasWidth;
	DWORD			m_CanvasHeight;
	CRect			m_CanvasRect;

	Pen*			m_pSelectLinePen;
	SolidBrush*		m_pSelectRectBrush;
	SelectStatus	m_SelectStatus;
	LONG			m_SelectBeginX;
	LONG			m_SelectEndX;

	CBitmap			m_BitMap;
	CBrush			m_CanvasBrush;

	CToolTipCtrl	m_ToolTip;

	std::vector<COUNT_TIME>* m_pVec;
	DWORD		m_dwMaxCount;
	DWORD		m_dwMinCount;
};
