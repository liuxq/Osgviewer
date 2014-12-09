//////////////////////////////////////////////////////////// CXTreeCtrl
#include "stdafx.h"
//#include "DragTree.h"
#include "CXTreeCtrl.h"
#include "MainFrm.h"


#define DRAG_DELAY 60
CXTreeCtrl::CXTreeCtrl()
{
	m_bDragging = false;
} 
CXTreeCtrl::~CXTreeCtrl()
{}

BEGIN_MESSAGE_MAP(CXTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CXTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CXTreeCtrl::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

	m_hItemDragS = pNMTreeView->itemNew.hItem;
	//得到用于拖动时显示的图象列表
	
	m_bDragging = true;

	SetCapture();
	::SetCursor((HCURSOR)::LoadCursor(NULL, IDC_HAND)); // 鼠标设置为手状表示可以移动窗口
}

void CXTreeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	//HTREEITEM hItem;
	//UINT flags;

	m_HoverPoint = point;
	if( m_bDragging )
	{
		CPoint pt = point;
		::SetCursor((HCURSOR)::LoadCursor(NULL, IDC_HAND)); // 鼠标设置为手状表示可以移动窗口
	}
	CTreeCtrl::OnMouseMove(nFlags, point);
}

void CXTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CTreeCtrl::OnLButtonUp(nFlags, point);
	if( m_bDragging )
	{
		m_bDragging = FALSE;
		ReleaseCapture();
		SelectDropTarget( NULL );
		::SetCursor((HCURSOR)::LoadCursor(NULL, IDC_ARROW));

		CPoint tmp = point;
		ClientToScreen(&tmp);
		CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		RECT lineRect;
		pMain->getOutPutWnd().GetWindowRect(&lineRect);
		if(PtInRect(&lineRect,tmp))
		{
			HTREEITEM trItem = GetSelectedItem();
			/*UINT uFlag = 0;
			trItem = HitTest( point,&uFlag );*/
			CString dataAttribute = GetItemText(trItem);
			CString dataName = GetItemText(GetParentItem(trItem));
			pMain->getOutPutWnd().updateDataName(tmp,dataName,dataAttribute);
		}
	}
}


void CXTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) //处理无意拖曳
{
	m_dwDragStart = GetTickCount();

	HTREEITEM trItem = 0;
	UINT uFlag = 0;
	trItem = HitTest( point,&uFlag );
	if( trItem )
	{
		SelectItem( trItem );
		//Expand( trItem,TVE_EXPAND );
	}
	CTreeCtrl::OnLButtonDown(nFlags, point);
}
void CXTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	HTREEITEM trItem = 0;
	UINT uFlag = 0;
	trItem = HitTest( point,&uFlag );
	CString dataAttribute = GetItemText(trItem);
	CString dataName = GetItemText(GetParentItem(trItem));
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	pMain->getOutPutWnd().getChar1()->updateDataName(dataName,dataAttribute);
}