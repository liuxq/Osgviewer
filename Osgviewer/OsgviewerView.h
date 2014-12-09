
// OsgviewerView.h : interface of the COsgviewerView class
//

#pragma once
#include "MFC_OSG.h"

class COutputWnd;

class COsgviewerView : public CView
{
protected: // create from serialization only
	COsgviewerView();
	DECLARE_DYNCREATE(COsgviewerView)

// Attributes
public:
	COsgviewerDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual void OnInitialUpdate();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	cOSG* getOSG()
	{
		return mOSG;
	}
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~COsgviewerView();
	bool isConnected;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    cOSG* mOSG;
    HANDLE mThreadHandle;
	int iFollow;
	bool bShowFloor, bShowShadow, bShowLable;
	bool bIn;
// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnContorlFollow();
	afx_msg void OnUpdateContorlFollow(CCmdUI *pCmdUI);
	afx_msg void OnContorlUsercontrol();
	afx_msg void OnUpdateContorlUsercontrol(CCmdUI *pCmdUI);
	afx_msg void OnContorlFollowTop();
	afx_msg void OnUpdateContorlFollowTop(CCmdUI *pCmdUI);
	afx_msg void OnContorlFollowRight();
	afx_msg void OnUpdateContorlFollowRight(CCmdUI *pCmdUI);
	afx_msg void OnScenecontrolShowf();
	afx_msg void OnUpdateScenecontrolShowf(CCmdUI *pCmdUI);
	afx_msg void OnScenecontrolShowshadow();
	afx_msg void OnUpdateScenecontrolShowshadow(CCmdUI *pCmdUI);
	afx_msg void OnScenecontrolShowlable();
	afx_msg void OnUpdateScenecontrolShowlable(CCmdUI *pCmdUI);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg void OnScenecontrolAutoscaleskeleton();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // debug version in OsgviewerView.cpp
inline COsgviewerDoc* COsgviewerView::GetDocument() const
   { return reinterpret_cast<COsgviewerDoc*>(m_pDocument); }
#endif

