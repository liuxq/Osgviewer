#pragma once


#include "DynamicLine.h"
class COutputWnd : public CDockablePane
{
// Construction
public:
	COutputWnd();

	//void UpdateFonts();

// Attributes
protected:
	DynamicLine m_chart1;
	DynamicLine m_chart2;
	bool isDoubleWin;

public:
	void updateChart();
	void setDoubleWin(bool isDoubleW);
	DynamicLine* getChar1()
	{
		return &m_chart1;
	}
	DynamicLine* getChar2()
	{
		return &m_chart2;
	}
	void updateDataName(CPoint p, CString data, CString attr);
// Implementation
public:
	virtual ~COutputWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnWindowOnewindow();
	afx_msg void OnWindowTwowindow();


	DECLARE_MESSAGE_MAP()
};
