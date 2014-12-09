// XTreeCtrl.h : header file
#ifndef AFX_XTREECTRL_H
#define AFX_XTREECTRL_H

class CXTreeCtrl : public CTreeCtrl
{
	// Construction
public:
	 CXTreeCtrl();
	  // Attributes
public:
	 // Operations
public:
	 // Overrides
	 // ClassWizard generated virtual function overrides
	 //{{AFX_VIRTUAL(CXTreeCtrl)
	 //}}AFX_VIRTUAL
	 // Implementation
public:
	virtual ~CXTreeCtrl();
	// Generated message map functions
protected:
	
	CPoint m_HoverPoint; //鼠标位置

	DWORD m_dwDragStart; //按下鼠标左键那一刻的时间
	BOOL m_bDragging; //标识是否正在拖动过程中
	CImageList* m_pDragImage; //拖动时显示的图象列表
	HTREEITEM m_hItemDragS; //被拖动的标签
	//{{AFX_MSG(CXTreeCtrl)
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
									//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
};
#endif 
