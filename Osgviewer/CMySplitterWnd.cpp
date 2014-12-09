#include "stdafx.h"
#include "CMySplitterWnd.h"


void CMySplitterWnd::hideHSplider()
{
	m_cy = m_cySplitterGap;
	m_cySplitterGap = 1;
}
void CMySplitterWnd::hideVSplider()
{
	m_cx = m_cxSplitterGap;
	m_cxSplitterGap = 1;
}
void CMySplitterWnd::ShowHSplider()
{
	m_cySplitterGap = m_cy;
}
void CMySplitterWnd::ShowVSplider()
{
	m_cxSplitterGap = m_cx;
}