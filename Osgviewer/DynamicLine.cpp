#include "stdafx.h"
#include "DynamicLine.h"
#include "DeviceBuffer.h"
#include "MainFrm.h"


#define MIN_DOUBLE -100000000
#define MAX_DOUBLE 100000000

CString dataNames[VMSENS_CHANNEL_NUM_ENG] = {_T("hip"),_T("hip1"),_T("hip2"),_T("hip3"),_T("chest"),_T("neck"),_T("head"),_T(""),
	_T("lCollar"),_T("lShldr"),_T("lForeArm"),_T("lHand"),_T(""),
	_T("rCollar"),_T("rShldr"),_T("rForeArm"),_T("rHand"),_T(""),
	_T("lThigh"),_T("lShin"),_T("lFoot"),_T("lFoot1"),_T(""),
	_T("rThigh"),_T("rShin"),_T("rFoot"),_T("rFoot1"),_T(""),
};
CString dataAttributes[6] = {_T("Xposition"),_T("Yposition"),_T("Zposition"),_T("Yrotation"),_T("Zrotation"),_T("Xrotation")};


BEGIN_MESSAGE_MAP(DynamicLine, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()

list<vector<sDataBufferEngine>> framedatas;
extern CCriticalSection g_cs;

DynamicLine::DynamicLine()
{
	//this->framedatas = NULL;
	this->stepIndex = 0;
	this->currentAttr = AttributeType::Pos;
	this->isSolved = false;
	this->curFrameIndex = 0;
	X_showLength = 10;

	m_ctlBkColor		= RGB(255,255,255);
	m_plotBkColor		= RGB(255,255,255);
	m_legendBkColor		= RGB(255,255,255);
	m_gridColor			= RGB(127,127,127);
	m_bctlBorder		= TRUE;
	m_bplotBorder		= TRUE;
	m_blegendBorder		= TRUE;
	m_bPrimaryLegend	= FALSE;
	m_bSecondaryLegend	= FALSE;
	m_bAxisLY			= TRUE;
	m_bAxisRY			= TRUE;
	m_bAxisBX			= TRUE;
	m_bAutoScrollX		= FALSE;
	m_bSimMode			= FALSE;

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

	m_font.CreateFontIndirect(&m_zoomFont);

}
void DynamicLine::updateDataName(CString dataName, CString dataAttribute)
{
	if (dataName == "Persons")
	{
		curFrameIndex = _ttoi(dataAttribute) - 1;

		CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		pMain->curFrameIndex = curFrameIndex;
		return;
	}
	if (dataAttribute == "Persons")
	{
		curFrameIndex = 0;
		return;
	}

	if (m_dataName == dataName && m_dataAttribute == dataAttribute)
	{
		return;
	}
	m_dataName = dataName;
	m_dataAttribute = dataAttribute;

	int iDataName = 0;
	while(dataNames[iDataName] != dataName && iDataName < VMSENS_CHANNEL_NUM_ENG) iDataName++;
	if (iDataName == VMSENS_CHANNEL_NUM_ENG)
		iDataName = 0;

	AttributeType attr;

	if (dataAttribute == L"Position")
	{
		attr = AttributeType::Pos;
	}
	else if (dataAttribute == L"Rotation")
	{
		attr = AttributeType::Rot;
	}
	else if (dataAttribute == L"Accel")
	{
		attr = AttributeType::Accel;
	}
	else if (dataAttribute == L"RotateRate")
	{
		attr = AttributeType::RotateRate;
	}
	else
	{
		attr = AttributeType::Pos;
	}

	COLORREF colors[3] = {RGB(255,0,0),RGB(0,255,0),RGB(0,0,255)};
	CString xyz[3] = {L"X", L"Y", L"Z"};
	for (int i = 0; i < 3; i++)
	{
		CString legend = dataName + " " + dataAttribute + xyz[i];
		char chr[50];
		WideCharToMultiByte( CP_ACP, 0, legend.GetBuffer(0), -1,  
			chr, sizeof(chr), NULL, NULL );

		this->SetLegend(i, PS_SOLID, colors[i], chr);
	}

	this->setAttribute(iDataName, attr);

	updateChart();
}
void DynamicLine::updateChart()
{
	this->setData();
}

BOOL DynamicLine::Create(DWORD dwstyle, CRect &rect, CWnd *pParent, UINT id)
{
	DWORD style = dwstyle & (~WS_BORDER);
	if(dwstyle & WS_BORDER)
		m_bctlBorder=TRUE;
	else
		m_bctlBorder=FALSE;

	if(!CWnd::Create(NULL, _T(""), style, rect, pParent, id, NULL))
		return FALSE;


	m_ctlRect = rect;
	/*pParent->ClientToScreen(m_ctlRect);
	ScreenToClient(m_ctlRect);*/

	ComputeRects(TRUE);

	CString legend = dataNames[0] + " " + dataAttributes[0];
	char chr[50];
	WideCharToMultiByte( CP_ACP, 0, legend.GetBuffer(0), -1,  
		chr, sizeof(chr), NULL, NULL );

	this->SetLegend(0, PS_SOLID, RGB(255,0,0), chr);
	this->SetLegend(1, PS_SOLID, RGB(0,255,0), chr);
	this->SetLegend(2, PS_SOLID, RGB(0,0,255), chr);
	this->setAttribute(0, AttributeType::Pos);

	return TRUE;
}

void DynamicLine::MoveWindow(CRect & Rect)
{
	m_ctlRect = Rect;
	m_ctlRect.right -= m_ctlRect.left;
	m_ctlRect.left = 0;
	/*GetParent()->ClientToScreen(m_ctlRect);
	ScreenToClient(m_ctlRect);*/
	ComputeRects(TRUE);
	CWnd::MoveWindow(Rect);
}
void DynamicLine::ComputeRects(BOOL bInitialization)
{
	// adjust the client rect for borders

	//GetClientRect(m_ctlRect);
	CClientDC dc(this);
	int w = 0;
	int n=0;
	CSize z=dc.GetTextExtent(CString("A"));
	//	m_TextHeight = z.cy;

	m_dzoom = ((double)m_ctlRect.Height()/(double)z.cy) / 20.0;

	m_zoomFont.lfWidth = (int)(m_logFont.lfWidth * m_dzoom);
	m_zoomFont.lfHeight = (int)(m_logFont.lfHeight * m_dzoom);
	m_font.Detach();
	m_font.CreateFontIndirect(&m_zoomFont);
	CFont *oFont = dc.SelectObject(&m_font);
	//	SetFont(&m_font);
	z=dc.GetTextExtent(CString(_T("A")));
	m_TextHeight = z.cy;
	if(m_bctlBorder){
		m_clientRect.left = m_ctlRect.left+2;
		m_clientRect.right = m_ctlRect.right-2;
		m_clientRect.top = m_ctlRect.top+2;
		m_clientRect.bottom = m_ctlRect.bottom-2;
	}else{
		m_clientRect = m_ctlRect;
	}

	if(bInitialization)
	{
		m_iMtop = m_iMbottom = m_clientRect.Height()/10;
		m_iMleft = m_iMright = m_clientRect.Width()/10;
	}

	// compute plot rect.
	m_plotRect.left = m_clientRect.left + m_iMleft;
	m_plotRect.right = m_clientRect.right - m_iMright;
	m_plotRect.top = m_clientRect.top + m_iMtop;
	m_plotRect.bottom = m_clientRect.bottom - m_iMbottom;

	// compute default legend possition

	if(bInitialization)
	{
		m_legendRect.left = m_plotRect.left + (m_iMleft/5);
		m_legendRect.right = m_plotRect.left + (m_plotRect.Width()/5);
		m_legendRect.top = m_plotRect.top - (m_iMtop/2);
		m_legendRect.bottom = m_plotRect.top + (m_iMtop);
		int w = 0;
		int n=0;
		for(int x = 0; x< MAXLEGENDS;x++){
			if(m_primarylegends[x].m_bIAmInUse){
				n++;
				z=dc.GetTextExtent(CString(m_primarylegends[x].m_szTitle));
				if(z.cx > w )
					w=z.cx;
				//				m_TextHeight = z.cy;
			}
		}

		m_legendRect.right = m_legendRect.left + 10+(2*m_TextHeight) + w;
		m_legendRect.bottom = m_legendRect.top + 10 + (m_TextHeight*n);

		/*m_legendRect.right = m_legendRect.left + 40 + w;
		m_legendRect.bottom = m_legendRect.top + 10 + (m_TextHeight*n);*/
	}

	// compute left axis area
	m_axisLYRect.left = m_clientRect.left + (m_iMleft/5);
	m_axisLYRect.right = m_plotRect.left;
	m_axisLYRect.top = m_plotRect.top;
	m_axisLYRect.bottom = m_plotRect.bottom;

	// compute right axis area
	m_axisRYRect.left =  m_plotRect.left;
	m_axisRYRect.right = m_clientRect.right - (m_iMright/5);
	m_axisRYRect.top = m_plotRect.top;
	m_axisRYRect.bottom = m_plotRect.bottom;

	// compute bottom axis area
	m_axisBXRect.left = m_plotRect.left;
	m_axisBXRect.right = m_plotRect.right;
	m_axisBXRect.top = m_plotRect.bottom;
	m_axisBXRect.bottom = m_clientRect.bottom - (m_iMbottom/5);

	//	if(bInitialization)
	//	{
	//m_timeaxis.m_dSecondsPrPixel = ((double)(m_timeaxis.m_maxtime.GetTime() - m_timeaxis.m_mintime.GetTime())) / (double)m_plotRect.Width();
	
	m_leftaxis.maxrange = 2000.0;
	m_leftaxis.minrange = 200.0;
	m_rightaxis.maxrange = 2000.0;
	m_rightaxis.minrange = 200.0;
	m_stepaxis.maxrange = 100;
	m_stepaxis.minrange = 0;

	m_leftaxis.m_dValuePrPixel   = ((double)(m_leftaxis.maxrange- m_leftaxis.minrange) / (double)m_plotRect.Height());
	m_rightaxis.m_dValuePrPixel   = ((double)(m_rightaxis.maxrange- m_rightaxis.minrange) / (double)m_plotRect.Height());
	m_stepaxis.m_dValuePrPixel   = ((double)(m_stepaxis.maxrange- m_stepaxis.minrange) / (double)m_plotRect.Width());
	//	}
	dc.SelectObject(oFont);
}

void DynamicLine::setData()
{
	//this->framedatas = framedatas;
	calculate();
}
void DynamicLine::setAttribute(int dataIndex, AttributeType curAttr)
{
	this->dataIndex = dataIndex;
	this->currentAttr = curAttr;
}

void DynamicLine::calculate()
{
	int XaxisCount = 2000;
	g_cs.Lock();
	vector<sDataBufferEngine>* pFrameData = getCurFrame(curFrameIndex);
	if (!pFrameData)
	{
		g_cs.Unlock();
		return;
	}
	vector<sDataBufferEngine>& framedata = *pFrameData;
	if (framedata.empty())
	{
		g_cs.Unlock();
		return;
	}
	miny = MAX_DOUBLE;
	maxy = MIN_DOUBLE;
	int starti = framedata.size() - XaxisCount;
	if (starti < 0)
		starti = 0;
	if (currentAttr == Pos)
	{
		for (int i = starti; i < framedata.size(); i++)
		{
			for (int j = 0;j < 3; j++)
			{
				double tmp = framedata.at(i).mChannel[dataIndex].mT[j];
				if (tmp < MIN_DOUBLE || tmp > MAX_DOUBLE) continue;
				if (miny > tmp) miny = tmp;
				if (maxy < tmp) maxy = tmp;
			}
		}
	}
	else if (currentAttr == Rot)
	{
		for (int i = starti; i < framedata.size(); i++)
		{
			for (int j = 0;j < 3; j++)
			{
				double tmp = framedata.at(i).mChannel[dataIndex].mR[j];
				if (tmp < MIN_DOUBLE || tmp > MAX_DOUBLE) continue;
				if (miny > tmp) miny = tmp;
				if (maxy < tmp) maxy = tmp;
			}
		}
	}else if (currentAttr == Accel)
	{
		for (int i = starti; i < framedata.size(); i++)
		{
			for (int j = 0;j < 3; j++)
			{
				double tmp = framedata.at(i).mChannel[dataIndex].mAcc[j];
				if (tmp < MIN_DOUBLE || tmp > MAX_DOUBLE) continue;
				if (miny > tmp) miny = tmp;
				if (maxy < tmp) maxy = tmp;
			}
		}
	}else if (currentAttr == RotateRate)
	{
		for (int i = starti; i < framedata.size(); i++)
		{
			for (int j = 0;j < 3; j++)
			{
				double tmp = framedata.at(i).mChannel[dataIndex].mGyro[j];
				if (tmp < MIN_DOUBLE || tmp > MAX_DOUBLE) continue;
				if (miny > tmp) miny = tmp;
				if (maxy < tmp) maxy = tmp;
			}
		}
	}
	
	if (miny == maxy)
		maxy = miny +1;

	m_leftaxis.maxrange = maxy;
	m_leftaxis.minrange = miny;
	m_rightaxis.maxrange = maxy;
	m_rightaxis.minrange = miny;
	m_stepaxis.maxrange = framedata.size()-1;
	m_stepaxis.minrange = starti;
	g_cs.Unlock();

	m_leftaxis.m_dValuePrPixel   = ((double)(m_leftaxis.maxrange- m_leftaxis.minrange) / (double)m_plotRect.Height());
	m_rightaxis.m_dValuePrPixel   = ((double)(m_rightaxis.maxrange- m_rightaxis.minrange) / (double)m_plotRect.Height());
	m_stepaxis.m_dValuePrPixel   = ((double)(m_stepaxis.maxrange- m_stepaxis.minrange) / (double)m_plotRect.Width());

	
	Invalidate(FALSE);
}

void DynamicLine::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CMemDC  pdc(dc,this);  // non flickering painting
	CDC& curDc = pdc.GetDC();

	CFont *oFont = curDc.SelectObject(&m_font);
	
	g_cs.Lock();

	DrawBasic(&curDc);
	DrawGrid(&curDc);
	drawWhenSolving(&curDc);
	DrawLegend(&curDc);
	
	g_cs.Unlock();
	
	curDc.SelectObject(oFont);

	// Do not call CWnd::OnPaint() for painting messages
}
BOOL DynamicLine::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}
void DynamicLine::DrawBasic(CDC * dc)
{
	CBrush brushctlBkColor(m_ctlBkColor);
	dc->FillRect(m_ctlRect,&brushctlBkColor);
	if(m_bctlBorder)
	{
		dc->DrawEdge(m_ctlRect,BDR_SUNKENINNER|BDR_SUNKENOUTER, BF_RECT); 
	}
	dc->Rectangle(m_plotRect);
	DrawLegendShadow(dc);
}

void DynamicLine::DrawGrid(CDC * dc)
{
	DrawXAxisGrid(dc);
	DrawYAxisGrid(dc);
}

//*******************************************************************************************************/
//*******************************************************************************************************/
void DynamicLine::DrawYAxisGrid(CDC * dc)
{

	double yGrid = m_leftaxis.minrange;
	double delta = 25.0 + (long)(((m_leftaxis.m_dValuePrPixel)))*25;
	//double delta = /*25.0 + */m_leftaxis.m_dValuePrPixel*25;
	if((long)delta%50 != 0 && delta > 20.0)
		delta +=25;
	double d10 = delta / 5.0;
	// todo: delta switch
	long diff = ((long)yGrid)%((long)delta);
	yGrid = yGrid - diff;
	CPen *old, pen(PS_SOLID, 1, m_gridColor);
	CPen stick(PS_SOLID,0,RGB(0,0,0));
	CPen mline(PS_SOLID,0,RGB(192,192,192));

	for( long sy = 5; sy < m_plotRect.Height(); sy += 5)
	{
		int off=3;
		if( sy%25 == 0 ){
			off=5;
		}
			int y = (int)(m_plotRect.bottom - sy);
			old = dc->SelectObject(&stick); 
			dc->MoveTo(CPoint(m_plotRect.left,y));
			dc->LineTo(CPoint(m_plotRect.left-off,y));
			dc->MoveTo(CPoint(m_plotRect.right,y));
			dc->LineTo(CPoint(m_plotRect.right+off,y));
			dc->SelectObject(old);

			old = dc->SelectObject(&mline);
			dc->MoveTo(CPoint(m_plotRect.left+1,y));
			dc->LineTo(CPoint(m_plotRect.right-1,y));
			dc->SelectObject(old);
	}
	double ydelta = m_leftaxis.maxrange - m_leftaxis.minrange;
	yGrid = m_leftaxis.minrange;
	old = dc->SelectObject(&pen);
	for (int sy = 0; sy < m_plotRect.Height(); sy += 25)
	{
		
		int y = (int)(m_plotRect.bottom - sy);
		dc->MoveTo(CPoint(m_plotRect.left+1,y));
		dc->LineTo(CPoint(m_plotRect.right-1,y));

		char b[1000];
		sprintf(b, "%.2f", yGrid);
		dc->DrawText(CString(b), CRect(m_clientRect.left, y-m_TextHeight/2,m_plotRect.left-5,y+m_TextHeight/2), DT_RIGHT|DT_BOTTOM);

		sprintf(b, "%.2f", yGrid);
		dc->DrawText(CString(b), CRect(m_plotRect.right+5, y-m_TextHeight/2,m_clientRect.right,y+m_TextHeight/2), DT_LEFT|DT_BOTTOM);

		yGrid += m_leftaxis.m_dValuePrPixel * 25;

	}
	dc->SelectObject(old);
	/*
	while(yGrid <= m_leftaxis.maxrange)
	{
		double yy = m_plotRect.bottom - ((yGrid-m_leftaxis.minrange)/m_leftaxis.m_dValuePrPixel);
		int y = (int)yy;
		if(yGrid > m_leftaxis.minrange && yGrid<m_leftaxis.maxrange){
			dc->MoveTo(CPoint(m_plotRect.left+1,y));
			dc->LineTo(CPoint(m_plotRect.right-1,y));
		}

		char b[100];
		sprintf(b, "%.0f", yGrid);
		dc->DrawText(CString(b), CRect(m_clientRect.left, y-m_TextHeight/2,m_plotRect.left-5,y+m_TextHeight/2), DT_RIGHT|DT_BOTTOM);

		double yr = (m_plotRect.bottom - yy) * m_rightaxis.m_dValuePrPixel + m_rightaxis.minrange;
		sprintf(b, "%.0f", yr);
		dc->DrawText(CString(b), CRect(m_plotRect.right+5, y-m_TextHeight/2,m_clientRect.right,y+m_TextHeight/2), DT_LEFT|DT_BOTTOM);

		yGrid += delta;
	}
	dc->SelectObject(old);*/
}

//*******************************************************************************************************/
//*******************************************************************************************************/
void DynamicLine::DrawXAxisGrid(CDC * dc)
{
	double xGrid = 0.0;
	for (int sx = 0; sx < m_plotRect.Width(); sx += 50)
	{

		int x = m_plotRect.left + sx;
		dc->MoveTo(CPoint(x,m_plotRect.bottom));
		dc->LineTo(CPoint(x,m_plotRect.bottom+5));

		char b[100];
		sprintf(b, "%.0f", xGrid + m_stepaxis.minrange);
		CSize z = dc->GetTextExtent(CString(b));
		
		dc->DrawText(CString(b), CRect(x-z.cx/2, m_plotRect.bottom+4, x+z.cx/2, m_plotRect.bottom+4+m_TextHeight), DT_RIGHT|DT_BOTTOM);
		xGrid += m_stepaxis.m_dValuePrPixel * 50;

	}
	//long yGrid = m_timeaxis.m_mintime.GetTime();
	//long delta = (long)(10.0 + (long)(((m_timeaxis.m_dSecondsPrPixel)))*10);
	//long d10 = (long)(delta / 10.0);
	//// todo: delta switch
	//long diff = ((long)yGrid)%((long)delta);
	//yGrid = yGrid - diff;
	//CPen *old, pen(PS_SOLID, 1, m_gridColor);
	//CPen stick(PS_SOLID,0,RGB(0,0,0));
	//CPen mline(PS_SOLID,0,RGB(192,192,192));

	//for( long sx = m_stepaxis.minrange - diff; sx < m_timeaxis.m_maxtime.GetTime(); sx+=d10)
	//{
	//	int off=3;
	//	if((long)sx%(long)delta == 0){
	//		off=5;
	//	}
	//	if(sx > m_timeaxis.m_mintime.GetTime()){
	//		int x = (int)(m_plotRect.left + ((sx-m_timeaxis.m_mintime.GetTime())/m_timeaxis.m_dSecondsPrPixel));
	//		//			int x = m_plotRect.right - ((sx-m_timeaxis.m_mintime.GetTime())/m_timeaxis.m_lSecondsPrPixel);
	//		old = dc->SelectObject(&stick); 
	//		dc->MoveTo(CPoint(x,m_plotRect.bottom));
	//		dc->LineTo(CPoint(x+off,m_plotRect.bottom));
	//		dc->SelectObject(old);

	//		old = dc->SelectObject(&mline);
	//		dc->MoveTo(CPoint(x,m_plotRect.bottom-1));
	//		dc->LineTo(CPoint(x,m_plotRect.top+1));
	//		dc->SelectObject(old);
	//	}
	//}

	//old = dc->SelectObject(&pen);
	//while(yGrid <= m_timeaxis.m_maxtime.GetTime())
	//{
	//	int x = (int)(m_plotRect.left + ((yGrid-m_timeaxis.m_mintime.GetTime())/m_timeaxis.m_dSecondsPrPixel));

	//	if(yGrid > m_timeaxis.m_mintime.GetTime() && yGrid<m_timeaxis.m_maxtime.GetTime()){
	//		dc->MoveTo(CPoint(x,m_plotRect.bottom-1));
	//		dc->LineTo(CPoint(x,m_plotRect.top+1));
	//	}

	//	//		char b[100];
	//	//		sprintf(b, "%.0f", yGrid);
	//	//		dc->DrawText(b, CRect(m_clientRect.left, y-m_TextHeight/2,m_plotRect.left-5,y+m_TextHeight/2), DT_RIGHT|DT_BOTTOM);

	//	yGrid += delta;
	//}
	//dc->SelectObject(old);
}

//*******************************************************************************************************/
//*******************************************************************************************************/
void DynamicLine::DrawLegendShadow(CDC * dc)
{
	if(m_blegendBorder){
		CPen pen(PS_SOLID, 1, RGB(127,127,127));
		CPen *oPen = dc->SelectObject(&pen);
		CBrush *oBrush , brush(RGB(127,127,127));
		oBrush = dc->SelectObject(&brush);
		dc->Rectangle(CRect(m_legendRect.left+5,m_legendRect.top+5,m_legendRect.right+5, m_legendRect.bottom+5));
		dc->SelectObject(oBrush);
		dc->SelectObject(oPen);
	}
}
//*******************************************************************************************************/
//*******************************************************************************************************/
void DynamicLine::SetLegend(int l, int style, COLORREF color, const char *text)
{
	m_primarylegends[l].m_bIAmInUse	= TRUE;
	m_primarylegends[l].m_color		= color;
	m_primarylegends[l].m_istyle		= style;
	m_primarylegends[l].m_szTitle		= text;
	CClientDC dc(this);
	CFont *oFont = dc.SelectObject(&m_font);
	int w = 0;
	int n=0;
	for(int x = 0; x< MAXLEGENDS;x++){
		if(m_primarylegends[x].m_bIAmInUse){
			n++;
			CSize z=dc.GetTextExtent(CString(m_primarylegends[x].m_szTitle));
			if(z.cx > w )
				w=z.cx;
			//			m_TextHeight = z.cy;
		}
	}

	m_legendRect.right = m_legendRect.left + 10+(2*m_TextHeight) + w;
	m_legendRect.bottom = m_legendRect.top + 10 + (m_TextHeight*n);
	dc.SelectObject(oFont);
}
//*******************************************************************************************************/
//*******************************************************************************************************/
void DynamicLine::DrawLegend(CDC * dc)
{
	if(m_blegendBorder)
	{
		CPen pen(PS_SOLID, 1, RGB(0,0,0));
		CPen *oPen = dc->SelectObject(&pen);
		CBrush *oBrush , brush(m_legendBkColor);
		oBrush = dc->SelectObject(&brush);
		dc->Rectangle(m_legendRect);
		dc->SelectObject(oBrush);
		dc->SelectObject(oPen);
	}
	int y = m_legendRect.top + 5;
	int dx = m_legendRect.left + (2*m_TextHeight);
	int mh = m_TextHeight/2;
	for(int x = 0; x< MAXLEGENDS;x++)
	{
		if(m_primarylegends[x].m_bIAmInUse)
		{
			CRect lRect(	dx + 5, y, m_legendRect.right - 5, y + m_TextHeight);
			CPen pen(m_primarylegends[x].m_istyle, 1, m_primarylegends[x].m_color);
			CPen *oPen = dc->SelectObject(&pen);
			dc->MoveTo(CPoint(m_legendRect.left+5, y + mh));
			dc->LineTo(CPoint(dx, y+mh));
			dc->SelectObject(oPen);

			dc->DrawText(m_primarylegends[x].m_szTitle, lRect, DT_LEFT);
			y+=m_TextHeight+1;
		}
	}
}

void DynamicLine::drawWhenSolving(CDC* dc)
{
	vector<sDataBufferEngine>* pFrameData = getCurFrame(curFrameIndex);
	if (!pFrameData)
		return;
	vector<sDataBufferEngine>& framedata = *pFrameData;
	if (framedata.empty())
		return;
	CPoint p;
	
	//g_cs.Lock();
	int sz = framedata.size();
	CPoint* pLineArray = new CPoint[sz];

	for (int j = 0; j < 3; j++)
	{
		CPen pen(m_primarylegends[j].m_istyle, 1, m_primarylegends[j].m_color);
		CPen *oPen = dc->SelectObject(&pen);
		int ly = 0;
		for (int i = 0; i < sz; i++)
		{
			/*if(framedata.at(i).empty())
			{
			return;
			}*/

			double attrValue;
			if (currentAttr == Pos)
			{
				attrValue = framedata.at(i).mChannel[dataIndex].mT[j];
			}
			else if (currentAttr == Rot)
			{
				attrValue = framedata.at(i).mChannel[dataIndex].mR[j];
			}
			else if (currentAttr == Accel)
			{
				attrValue = framedata.at(i).mChannel[dataIndex].mAcc[j];
			}
			else if (currentAttr == RotateRate)
			{
				attrValue = framedata.at(i).mChannel[dataIndex].mGyro[j];
			}

			p.x = (int)(m_plotRect.left + ((i-m_stepaxis.minrange)/m_stepaxis.m_dValuePrPixel));
			p.y = (int)(m_plotRect.bottom - ((attrValue-m_leftaxis.minrange)/m_leftaxis.m_dValuePrPixel));

			if((ly == 0 || p.x != pLineArray[ly].x || p.y != pLineArray[ly].y)
				&& (p.x >= m_plotRect.left && p.x <= m_plotRect.right))
			{
				pLineArray[ly].x = p.x;
				pLineArray[ly].y = p.y;
				ly++;
			}
		}
		//g_cs.Unlock();
		if(ly > 0)
		{
			dc->Polyline(pLineArray, ly);
		}
		dc->SelectObject(oPen);
	}
	

	delete[] pLineArray;
}
