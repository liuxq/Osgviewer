#ifndef DYNAMICLINE_H
#define DYNAMICLINE_H

#include "commonData.h"

class axis
{
public:
	CString szTitle;
	double	minrange;
	double	maxrange;

	double	m_dValuePrPixel;

	axis()
	{
		szTitle	= "Title";
		minrange	= 0.0;
		maxrange	= 2000.0;

		m_dValuePrPixel = 1;
	};
};

//*******************************************************************************************************/
//* legend
//*******************************************************************************************************/
class legend
{
public:
	BOOL		m_bIAmInUse;
	COLORREF	m_color;		// legend color code
	int			m_istyle;
	CString		m_szTitle;		// legend title

	legend(){
		m_bIAmInUse = FALSE;
		m_color		= 0;
		m_istyle	= PS_SOLID;
		m_szTitle	= "";
	}
};

#define MAXLEGENDS	10

class DynamicLine: public CWnd
{

public:
	int currentStep;
	int dataIndex;
	int curFrameIndex;
	AttributeType currentAttr;
	double miny;
	double maxy;
	int stepIndex;
	bool isSolved;
	int X_showLength;


	BOOL		m_bctlBorder;		// control border
	BOOL		m_bplotBorder;		// plot border
	BOOL		m_blegendBorder;	// legend border
	BOOL		m_bPrimaryLegend;	// primary legend
	BOOL		m_bSecondaryLegend;	// secondary legend
	BOOL		m_bAxisLY;			// left axis
	BOOL		m_bAxisRY;			// right axis
	BOOL		m_bAxisBX;			// bottom axis
	BOOL		m_bAutoScrollX;		// automatic x range scrolling
	BOOL		m_bSimMode;			// simulate data input

	double scaleY;

	COLORREF	m_ctlBkColor;		// control background color
	COLORREF	m_plotBkColor;		// plot bacground color
	COLORREF	m_legendBkColor;	// legend background color
	COLORREF	m_gridColor;		// grid line color

	CRect		m_ctlRect;			// control rect
	CRect		m_clientRect;		// ctlRect - borderspace
	CRect		m_plotRect;			// clientRect - margins
	CRect		m_legendRect;		// any rect within clientRect
	CRect		m_axisLYRect;		// Left axisi rect
	CRect		m_axisRYRect;		// right y axis
	CRect		m_axisBXRect;		// bottom x axis

	int			m_iMleft;			// left margin
	int			m_iMright;			// right margin
	int			m_iMtop;			// top margin
	int			m_iMbottom;			// bottom margin

	CFont		m_font;
	LOGFONT		m_logFont;
	LOGFONT		m_zoomFont;
	double		m_dzoom;
	int			m_TextHeight;

	axis		m_leftaxis;			// left axis
	axis		m_rightaxis;		// right axis
	axis		m_stepaxis;

	legend		m_primarylegends[MAXLEGENDS];
	legend		m_secondarylegends[MAXLEGENDS];

	CPoint		*pLineArray;		// pre-calculated when new data are entered into the system
	long		lArraySize;			// current size of pLineArray

	CString m_dataName;
	CString m_dataAttribute;

public:
	void updateChart();
	void updateDataName(CString data, CString attr);

public:
	BOOL Create(DWORD dwstyle, CRect &rect, CWnd *pParent, UINT id);
	void MoveWindow(CRect &Rect);

	DynamicLine();
	void setData();
	void setAttribute(int dataIndex, AttributeType stepIndex);
	void setCurFrameIndex(int curIndex){curFrameIndex = curIndex;};

	void calculate();

	void ComputeRects(BOOL bInitialization);
	void SetLegend(int l, int style, COLORREF color, const char *text);

	void DrawBasic(CDC * dc);
	void drawWhenSolving(CDC* dc);
	void DrawGrid(CDC * dc);
	void DrawYAxisGrid(CDC * dc);
	void DrawXAxisGrid(CDC * dc);
	void DrawLegendShadow(CDC * dc);
	void DrawLegend(CDC * dc);

protected:
	//{{AFX_MSG(clPlot)
	afx_msg void OnPaint();
	//}}AFX_MSG
	BOOL OnEraseBkgnd(CDC* pDC) ;
	DECLARE_MESSAGE_MAP()
};

#endif