
#include "stdafx.h"
#include "MainFrm.h"
#include "ClassView.h"
#include "Resource.h"
#include "Osgviewer.h"
#include <string>
#include <fstream>
#include <stack>

#include <osgDB/DatabasePager>
#include <osgDB/Registry>
#include <osgDB/ReadFile>

#include "commonData.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClassView::CClassView()
{
	//m_nCurrSort = ID_SORTING_GROUPBYTYPE;
}

CClassView::~CClassView()
{
}

BEGIN_MESSAGE_MAP(CClassView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CLASS_ADD_MEMBER_FUNCTION, OnClassAddMemberFunction)
	ON_COMMAND(ID_CLASS_ADD_MEMBER_VARIABLE, OnClassAddMemberVariable)
	ON_COMMAND(ID_CLASS_DEFINITION, OnClassDefinition)
	ON_COMMAND(ID_CLASS_PROPERTIES, OnClassProperties)
	ON_COMMAND(ID_NEW_FOLDER, OnNewFolder)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassView message handlers

int CClassView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create views:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS/* | WS_CLIPSIBLINGS | WS_CLIPCHILDREN*/;

	if (!m_wndClassView.Create(dwViewStyle, rectDummy, this, 2))
	{
		TRACE0("Failed to create Class View\n");
		return -1;      // fail to create
	}

	// Fill in some static tree view data (dummy code, nothing magic here)
	FillClassView();
	AdjustLayout();

	return 0;
}

void CClassView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}
void CClassView::ResetPerson()
{
	int frameSize = 0;
	g_cs.Lock();
		frameSize = framedatas.size();
	g_cs.Unlock();


	if (m_wndClassView.ItemHasChildren(personSets))
	{
		HTREEITEM hChild = m_wndClassView.GetChildItem(personSets);
		while(hChild)
		{
			HTREEITEM deleteChild = hChild;
			hChild = m_wndClassView.GetNextItem(hChild,TVGN_NEXT); 
			m_wndClassView.DeleteItem(deleteChild);
		}
	}

	for (int i = 0; i < frameSize; i++)
	{
		CString str;
		str.Format(L"%d",i + 1);
		m_wndClassView.InsertItem(str, personSets);
	}
	

	Invalidate(true);
}
void CClassView::FillClassView()
{
	HTREEITEM hRoot, hClass;

	ifstream inputTPose("TPose.avm");
	string type, tittle, tmp;
	stack<string> titles;
	stack<HTREEITEM> items;
	personSets = m_wndClassView.InsertItem(L"Persons", TVI_ROOT);
	m_wndClassView.SetItemState(personSets, TVIS_BOLD, TVIS_BOLD);
	while(inputTPose >> type)
	{
		if (type == "ROOT" || type == "JOINT" || type == "End")
		{
			inputTPose >> tmp;
			titles.push(tmp);
			inputTPose >> tmp;
			inputTPose >> tmp >> tmp >> tmp >> tmp;
			string tittle = titles.top();
			CString tt = CString(tittle.c_str());

			if (type == "ROOT")
			{
				hRoot = m_wndClassView.InsertItem(tt, TVI_ROOT);
				m_wndClassView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD); 
				m_wndClassView.Expand(hRoot, TVE_EXPAND);
				items.push(hRoot);
			}
			else if (type == "JOINT" || type == "End")
			{
				hClass = items.top();
				hClass = m_wndClassView.InsertItem(tt, 1,1,hClass);
				items.push(hClass);
			}
			if (type != "End")
			{
				hClass = items.top();
				inputTPose >> type;
				int channelNum = 0;
				inputTPose >> channelNum;
				for (int i = 0; i < channelNum; i++)
				{
					inputTPose >> tmp;
					
				}
				//if (channelNum == 3)
				{
					m_wndClassView.InsertItem(L"Position", 1,1,hClass);
					m_wndClassView.InsertItem(L"Rotation", 1,1,hClass);
					m_wndClassView.InsertItem(L"Accel", 1,1,hClass);
					m_wndClassView.InsertItem(L"RotateRate", 1,1,hClass);
				}
				//CString t(tmp.c_str());
				
			}			
		}

		if (type == "}")
		{
			titles.pop();
			items.pop();
		}
	}
}

void CClassView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndClassView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// Select clicked item:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	/*CMenu menu;
	menu.LoadMenu(IDR_POPUP_SORT);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}*/
}

void CClassView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	/*int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);*/
	int cyTlb = 0;
	m_wndClassView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CClassView::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

//void CClassView::OnSort(UINT id)
//{
//	if (m_nCurrSort == id)
//	{
//		return;
//	}
//
//	m_nCurrSort = id;
//
//	CClassViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CClassViewMenuButton, m_wndToolBar.GetButton(0));
//
//	if (pButton != NULL)
//	{
//		pButton->SetImage(GetCmdMgr()->GetCmdImage(id));
//		m_wndToolBar.Invalidate();
//		m_wndToolBar.UpdateWindow();
//	}
//}

//void CClassView::OnUpdateSort(CCmdUI* pCmdUI)
//{
//	pCmdUI->SetCheck(pCmdUI->m_nID == m_nCurrSort);
//}

void CClassView::OnClassAddMemberFunction()
{
	AfxMessageBox(_T("Add member function..."));
}

void CClassView::OnClassAddMemberVariable()
{
	// TODO: Add your command handler code here
}

void CClassView::OnClassDefinition()
{
	// TODO: Add your command handler code here
}

void CClassView::OnClassProperties()
{
	// TODO: Add your command handler code here
}

void CClassView::OnNewFolder()
{
	AfxMessageBox(_T("New Folder..."));
}

void CClassView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndClassView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CClassView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndClassView.SetFocus();
}

//void CClassView::OnChangeVisualStyle()
//{
//	m_ClassViewImages.DeleteImageList();
//
//	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_CLASS_VIEW_24 : IDB_CLASS_VIEW;
//
//	CBitmap bmp;
//	if (!bmp.LoadBitmap(uiBmpId))
//	{
//		TRACE(_T("Can't load bitmap: %x\n"), uiBmpId);
//		ASSERT(FALSE);
//		return;
//	}
//
//	BITMAP bmpObj;
//	bmp.GetBitmap(&bmpObj);
//
//	UINT nFlags = ILC_MASK;
//
//	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;
//
//	m_ClassViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
//	m_ClassViewImages.Add(&bmp, RGB(255, 0, 0));
//
//	m_wndClassView.SetImageList(&m_ClassViewImages, TVSIL_NORMAL);
//
//	m_wndToolBar.CleanUpLockedImages();
//	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_SORT_24 : IDR_SORT, 0, 0, TRUE /* Locked */);
//}
