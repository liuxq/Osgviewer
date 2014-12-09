
#include "stdafx.h"

#include "OsgviewerDoc.h"
#include "OsgviewerView.h"

#include "OutputWnd.h"
#include "Resource.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

COutputWnd::COutputWnd()
{
	isDoubleWin = false;
}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CHARTOPTION_ONEWINDOW, &COutputWnd::OnWindowOnewindow)
	ON_COMMAND(ID_CHARTOPTION_TWOWINDOW, &COutputWnd::OnWindowTwowindow)
	//ON_MESSAGE(WM_MY_MESSAGE, OnMyMessage)
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create tabs window:
	CRect rc, rc1, rc2;
	GetClientRect(rc);
	rc1 = rc2 = rc;
	rc1.right = rc.Width()/2;
	rc2.left = rc.Width()/2;
	m_chart1.Create(WS_CHILD|WS_VISIBLE|WS_BORDER,rc1,this,12000);
	m_chart2.Create(WS_CHILD|WS_VISIBLE|WS_BORDER,rc2,this,12001);
	m_chart2.ShowWindow(FALSE);
	//updateChart();

	return 0;
}
void COutputWnd::setDoubleWin(bool isDoubleW)
{
	if (isDoubleWin != isDoubleW)
	{
		if (isDoubleW)
		{
			m_chart2.ShowWindow(TRUE);
		}else
		{
			m_chart2.ShowWindow(FALSE);
		}
		isDoubleWin = isDoubleW;

		CRect rc, rc1, rc2;
		GetClientRect(rc);
		rc1 = rc2 = rc;
		rc1.right = rc.Width()/2;
		rc2.left = rc.Width()/2;

		if (isDoubleWin)
		{
			m_chart1.MoveWindow(rc1);
			m_chart2.MoveWindow(rc2);
		}
		else
		{
			m_chart1.MoveWindow(rc);
		}
	}
}
void COutputWnd::OnWindowOnewindow()
{
	setDoubleWin(false);
}
void COutputWnd::OnWindowTwowindow()
{
	setDoubleWin(true);
}
void COutputWnd::updateDataName(CPoint p, CString data, CString attr)
{
	if (!isDoubleWin)
	{
		m_chart1.updateDataName(data,attr);
	}
	else
	{
		CRect rc;
		GetClientRect(rc);
		ScreenToClient(&p);
		if (p.x < rc.Width()/2)
		{
			m_chart1.updateDataName(data,attr);
		}else
		{
			m_chart2.updateDataName(data,attr);
		}
	}
}
void COutputWnd::OnContextMenu(CWnd* pWnd, CPoint point)
{
	
	CMenu menu;
	menu.LoadMenu(IDR_MENU_CHART);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

	if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
		return;

	((CFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
	UpdateDialogControls(this, FALSE);
	
}
void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	//m_chart.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	CRect rc, rc1, rc2;
	GetClientRect(rc);
	rc1 = rc2 = rc;
	rc1.right = rc.Width()/2;
	rc2.left = rc.Width()/2;

	if (isDoubleWin)
	{
		m_chart1.MoveWindow(rc1);
		m_chart2.MoveWindow(rc2);
	}
	else
	{
		m_chart1.MoveWindow(rc);
	}
	//m_chart1.updateChart();
	//m_chart2.updateChart();
	
}

void COutputWnd::updateChart()
{
	m_chart1.updateChart();
	if (isDoubleWin) m_chart2.updateChart();
}