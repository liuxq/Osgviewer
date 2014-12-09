#pragma once
#include "afxext.h"

class CMySplitterWnd : public CSplitterWnd
{
	//DECLARE_DYNAMIC(CMySplitterWnd)

	// Construction
public:
	//CMySplitterWnd();

	// Overrides
	//virtual void OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rect);
	void hideHSplider();
	void hideVSplider();
	void ShowHSplider();
	void ShowVSplider();
	
	int m_cx;
	int m_cy;
	// Implementation
public:
	//virtual ~CMySplitterWnd();

protected:

	//{{AFX_MSG(CSplitterWndEx)
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};