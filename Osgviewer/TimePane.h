#pragma once

class TimePane :public CDockablePane
{
// Construction
public:
	TimePane();

// Attributes
protected:
	CRect m_viewRect;
	CRect m_plotRect;
	int frameSize;
	double m_dValuePrPixel;

	CFont		m_font;
	LOGFONT		m_logFont;
	LOGFONT		m_zoomFont;
	int m_TextHeight;
public:
	int isRecord;
	int m_startRecord;

	int m_endRecord;
	int curFrameIndex;
public:
	void calculate();
	void startRecord();
	void stopRecord();
	void setCurFrameIndex(int curIndex){curFrameIndex = curIndex;};
// Implementation
public:
	virtual ~TimePane();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnWindowOnewindow();
	afx_msg void OnWindowTwowindow();
	afx_msg void OnPaint();


	DECLARE_MESSAGE_MAP()
};
