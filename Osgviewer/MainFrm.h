
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "ClassView.h"
#include "OutputWnd.h"
#include "CMySplitterWnd.h"
#include "TimePane.h"

class CMainFrame : public CFrameWndEx
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	COutputWnd& getOutPutWnd(){ return m_wndOutput;}
	TimePane& getTimePane(){return m_TimePane;}
	CClassView& getClassView(){return m_wndClassView;}
// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
	
#endif

protected:  // control bar embedded members
	CMFCMenuBar       m_wndMenuBar;
	CClassView        m_wndClassView;
	COutputWnd        m_wndOutput;
	CMySplitterWnd    m_wndSplitter;
	TimePane		  m_TimePane;

public:
	CEdit			  m_editExportPath;
	int iShowCurve;
	int iShowData;

	int curFrameIndex;

// Generated message map functions

private: 
	void convertWindowNum(int toWinNum);
	void splitRow();
	void splitColumn();
	void mergeRow();
	void mergeColumn();
	// Generated message map functions
protected:
	afx_msg void OnWindowOnewindow();
	afx_msg void OnWindowTwowindow();
	afx_msg void OnWindowFourwindow();
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//afx_msg void OnWindowManager();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);

	afx_msg void OnButtonCurve();
	afx_msg void OnButtonData();
	afx_msg void OnButtonAutoscale();
	DECLARE_MESSAGE_MAP()

	afx_msg void OnDestroy();
	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:
	CMFCToolBar       m_wndToolBar;

};


