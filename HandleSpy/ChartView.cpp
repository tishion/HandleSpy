#include "StdAfx.h"
#include <atlmisc.h>
#include "ChartView.h"

#define MAX_TIME_TO_STORE	(5*60*1000)		/*ms*/
#define ONE_MOVE_OFFSET_PX	4

CChartView::CChartView(void)
{
	m_nCurVal = 0;
	m_nLastVal = 0;
	m_dwUpperBound = 100;
	m_dwLowerBound = 0;
	m_pLinePen = NULL;
}

CChartView::~CChartView(void)
{
	if (NULL != m_pLinePen)
	{
		delete m_pLinePen;
		m_pLinePen = NULL;
	}
}

BOOL CChartView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

LRESULT CChartView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{

	return 0;
}

LRESULT CChartView::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{

	return 0;
}

LRESULT CChartView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);

	CRect rcClient;
	GetClientRect(rcClient);

	CString str;
	str.Format(_T("句柄数量:%d"), m_nCurVal);

	if (TRUE == m_MemDC.IsNull())
	{
		m_rcTextout.left = 10;
		m_rcTextout.top = 10;
		m_rcTextout.right = 110;
		m_rcTextout.bottom = 30;
		//初次绘制， 初始化m_MemDC, m_LinePen等
		m_pLinePen = new Pen(Color(255, 255, 0, 0), 1);
		m_HatchBrush.CreateHatchBrush(HS_HORIZONTAL, RGB(0, 128, 0));
		m_Font.Attach((HFONT)GetStockObject(ANSI_FIXED_FONT));

		CClientDC dcClient(m_hWnd);
		m_MemDC.CreateCompatibleDC(dcClient);

		CBitmap BitMap;
		BitMap.CreateCompatibleBitmap(dcClient, rcClient.Width(), rcClient.Height());
		m_MemDC.SelectBitmap(BitMap);

		m_MemDC.SetBkColor(RGB(0, 0, 0));
		CBrush CrossHatchBrush;
		CrossHatchBrush.CreateHatchBrush(HS_CROSS, RGB(0, 128, 0));
		m_MemDC.FillRect(rcClient, CrossHatchBrush);
	}

	dc.BitBlt(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), m_MemDC, 0, 0, SRCCOPY);	

	HFONT hOldFont = dc.SelectFont(m_Font);
	dc.SetTextColor(RGB(255, 0, 0));
	dc.TextOut(10, 10, str);
	dc.SelectFont(hOldFont);

	return 0;
}

void CChartView::SetRangeAndFirstValue(DWORD low, DWORD up, DWORD initVal)
{
	m_dwUpperBound = up;
	m_dwLowerBound = low;

	m_nCurVal = initVal;
	m_nLastVal = initVal;
}

void CChartView::Update(DWORD val)
{
	m_nLastVal = m_nCurVal;
	m_nCurVal = val;

	InternalUpdate();

	Invalidate(FALSE);
}

void CChartView::InternalUpdate()
{
	CRect rcClient;
	GetClientRect(rcClient);

	if (FALSE == m_MemDC.IsNull())
	{
		// 原图左移
		m_MemDC.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), m_MemDC, ONE_MOVE_OFFSET_PX, 0, SRCCOPY);
		
		CRect rcNew = rcClient;
		rcNew.left = rcNew.right - ONE_MOVE_OFFSET_PX;

		Graphics graphics(m_MemDC.m_hDC);
		graphics.SetSmoothingMode(SmoothingModeAntiAlias);

		// 右侧新出现的区域绘制横线
		m_MemDC.FillRect(rcNew, m_HatchBrush);

		PointF ptOld;
		ptOld.X = (float)(rcNew.left - 1);	//修正偏移,防止断线
		ptOld.Y = ((((float)(m_dwUpperBound - m_nLastVal))/(float)(m_dwUpperBound - m_dwLowerBound))
			* (float)rcClient.Height());

		PointF ptNew;
		ptNew.X = (float)(rcNew.right - 1);	//修正偏移,防止断线
		ptNew.Y = ((((float)(m_dwUpperBound - m_nCurVal))/(float)(m_dwUpperBound - m_dwLowerBound))
			* (float)rcClient.Height());

		graphics.DrawLine(m_pLinePen, ptOld, ptNew);

		static BOOL bDrawVertical = TRUE;
		bDrawVertical = !bDrawVertical;
		if (bDrawVertical)
		{
			Pen pen(Color(255, 0, 128, 0), 1);
			graphics.DrawLine(&pen, (int)ptNew.X, rcNew.top, (int)ptNew.X, rcNew.bottom);
		}
	}
}
