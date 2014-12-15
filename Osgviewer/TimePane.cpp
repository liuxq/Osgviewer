#include "stdafx.h"
#include "commonData.h"
#include "Resource.h"
#include "MainFrm.h"

#include "bvh.h"
#include "fbx.h"
#include "c3d.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

extern list<vector<sDataBufferEngine>> framedatas;

TimePane::TimePane()
{
	isRecord = false;
	m_startRecord = -1;
	m_endRecord = -1;
	curFrameIndex = 0;

	m_logFont.lfHeight			= -13;
	m_logFont.lfWidth			= 0;
	m_logFont.lfEscapement		= 0;
	m_logFont.lfOrientation		= 0;
	m_logFont.lfWeight			= 400;
	m_logFont.lfItalic			= FALSE;
	m_logFont.lfUnderline		= FALSE;
	m_logFont.lfStrikeOut		= FALSE;
	m_logFont.lfCharSet			= ANSI_CHARSET;
	m_logFont.lfOutPrecision	= OUT_DEFAULT_PRECIS;
	m_logFont.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
	m_logFont.lfQuality			= PROOF_QUALITY;
	m_logFont.lfPitchAndFamily	= DEFAULT_PITCH;
	lstrcpyW(m_logFont.lfFaceName,_T("Ariel"));

	m_zoomFont.lfHeight			= -13;
	m_zoomFont.lfWidth			= 0;
	m_zoomFont.lfEscapement		= 0;
	m_zoomFont.lfOrientation		= 0;
	m_zoomFont.lfWeight			= 400;
	m_zoomFont.lfItalic			= FALSE;
	m_zoomFont.lfUnderline		= FALSE;
	m_zoomFont.lfStrikeOut		= FALSE;
	m_zoomFont.lfCharSet			= ANSI_CHARSET;
	m_zoomFont.lfOutPrecision	= OUT_DEFAULT_PRECIS;
	m_zoomFont.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
	m_zoomFont.lfQuality			= PROOF_QUALITY;
	m_zoomFont.lfPitchAndFamily	= DEFAULT_PITCH;
	lstrcpyW(m_zoomFont.lfFaceName,_T("Ariel"));

	m_dValuePrPixel = 0.0f;

	m_font.CreateFontIndirect(&m_zoomFont);
}

TimePane::~TimePane()
{
}

BEGIN_MESSAGE_MAP(TimePane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
END_MESSAGE_MAP()

int TimePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	return 0;
}
void TimePane::calculate()
{
	vector<sDataBufferEngine>* pFrameData = getCurFrame(curFrameIndex);
	if (!pFrameData)
		return;
	vector<sDataBufferEngine>& framedata = *pFrameData;
	frameSize = framedata.size()-1;
	m_dValuePrPixel   = ((double)(frameSize) / (double)m_plotRect.Width());
	if (isRecord)
	{
		m_endRecord = frameSize;
	}
	Invalidate(FALSE);
}
void TimePane::startRecord()
{
	isRecord = true;
	m_startRecord = m_endRecord = frameSize;
}
void TimePane::stopRecord()
{
	if (isRecord)
	{
		m_endRecord = frameSize;
		isRecord = false;

		//开始存储
		CString epathtype;//存储
		GetPrivateProfileString(_T("Export"),_T("ExportType"),NULL,epathtype.GetBuffer(100),100,_T(".\\option.ini"));
		int curSel = _ttoi(epathtype);

		CString path;//存储
		GetPrivateProfileString(_T("Export"),_T("ExportPath"),NULL,path.GetBuffer(100),100,_T(".\\option.ini"));

		CString name; //获取系统时间
		CTime tm;
		tm = CTime::GetCurrentTime();
		name = tm.Format(_T("%Y.%m.%d_%X"));
		name.Replace(':','_');

		path.ReleaseBuffer();

		if (name != _T(""))
		{
			if (curSel == 0)
			{
				CString pathname = path + name + _T(".bvh");
				char chr[80];
				WideCharToMultiByte( CP_ACP, 0, pathname.GetBuffer(0), -1,  
					chr, sizeof(chr), NULL, NULL );
				BVH bv;
				bv.bvhWrite(chr, m_startRecord, m_endRecord, true);
			}
			else if (curSel == 1)
			{
				CString pathname = path + name + _T(".bvh");
				char chr[80];
				WideCharToMultiByte( CP_ACP, 0, pathname.GetBuffer(0), -1,  
					chr, sizeof(chr), NULL, NULL );
				BVH bv;
				bv.bvhWrite(chr, m_startRecord, m_endRecord, false);
			}
			else if (curSel == 2)
			{
				CString pathname = path + name + _T(".c3d");
				char chr[80];
				WideCharToMultiByte( CP_ACP, 0, pathname.GetBuffer(0), -1,  
					chr, sizeof(chr), NULL, NULL );
				C3D c3d_;
				c3d_.saveC3DFile(chr, m_startRecord, m_endRecord);
			}
			else if (curSel == 3)
			{
				CString pathname = path + name + _T(".fbx");
				char chr[80];
				WideCharToMultiByte( CP_ACP, 0, pathname.GetBuffer(0), -1,  
					chr, sizeof(chr), NULL, NULL );
				FBX fbx_;
				fbx_.fbxWrite(chr, m_startRecord, m_endRecord);
			}

		}
	}
}
void TimePane::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CMemDC  pdc(dc,this);  // non flickering painting
	CDC& curDc = pdc.GetDC();
	COLORREF color = RGB(255,255,255);
	CBrush brushctlBkColor(color);
	curDc.FillRect(m_viewRect,&brushctlBkColor);
	COLORREF color1 = RGB(100,100,100);
	CBrush brushctlBkColor1(color1);
	CRect borderRect = m_viewRect;
	borderRect.bottom = borderRect.top + 2;
	curDc.FillRect(borderRect,&brushctlBkColor1);

	CFont *oFont = curDc.SelectObject(&m_font);

	double xGrid = 0.0;
	int offset = 5;
	for (int sx = 0; sx < m_plotRect.Width(); sx += 10)
	{
		int x = m_plotRect.left + sx;

		offset = 3;
		if (sx % 50 == 0)
		{
			offset = 5;
			char b[100];
			sprintf(b, "%.0f", xGrid + 0);
			CSize z = curDc.GetTextExtent(CString(b));
			curDc.DrawText(CString(b), CRect(x-z.cx/2, m_plotRect.top+4, x+z.cx/2, m_plotRect.top+4+z.cy), DT_RIGHT|DT_BOTTOM);
			xGrid += m_dValuePrPixel * 50;
		}
		curDc.MoveTo(CPoint(x,m_plotRect.top));
		curDc.LineTo(CPoint(x,m_plotRect.top+offset));
	}
	if (m_startRecord != -1)
	{
		COLORREF color = RGB(100,100,100);
		CBrush brushctlBkColor(color);
		CRect recordRect = m_viewRect;
		recordRect.top += 14;
		recordRect.bottom = recordRect.top + 10;
		recordRect.left = m_startRecord / m_dValuePrPixel;
		recordRect.right = m_endRecord / m_dValuePrPixel;
		curDc.FillRect(recordRect,&brushctlBkColor);
	}
	curDc.SelectObject(oFont);
}
void TimePane::OnContextMenu(CWnd* pWnd, CPoint point)
{
	
	/*CMenu menu;
	menu.LoadMenu(IDR_MENU_CHART);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

	if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
	return;

	((CFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
	UpdateDialogControls(this, FALSE);*/
	
}
void TimePane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	//m_chart.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	GetClientRect(m_viewRect);	
	m_plotRect = m_viewRect;
	m_plotRect.left += 10;
	m_plotRect.right -= 10;
	m_plotRect.top += 2;
	m_plotRect.bottom -= 10;

	CClientDC dc(this);
	
	/*CSize z = dc.GetTextExtent(CString("A"));
	double m_dzoom = ((double)m_viewRect.Width()/(double)z.cy) * 0.02;
	m_zoomFont.lfWidth = (int)(m_logFont.lfWidth * m_dzoom);
	m_zoomFont.lfHeight = (int)(m_logFont.lfHeight * m_dzoom);*/
	m_zoomFont.lfWidth = 6;
	m_zoomFont.lfHeight = 12;
	m_font.Detach();
	m_font.CreateFontIndirect(&m_zoomFont);
	
	CFont *oFont = dc.SelectObject(&m_font);
	//	SetFont(&m_font);
	CSize z = dc.GetTextExtent(CString("A"));
	m_TextHeight = z.cy;
	dc.SelectObject(oFont);
}