
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Osgviewer.h"

#include "MainFrm.h"
#include "OsgviewerDoc.h"
#include "OsgviewerView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern int windowNum_;

extern CCriticalSection g_csForView;
extern COsgviewerView* viewSet[4];

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_WINDOW_ONEWINDOW, &CMainFrame::OnWindowOnewindow)
	ON_COMMAND(ID_WINDOW_TWOWINDOW, &CMainFrame::OnWindowTwowindow)
	ON_COMMAND(ID_WINDOW_FOURWINDOW, &CMainFrame::OnWindowFourwindow)
	ON_COMMAND(ID_BUTTON_CURVE, &CMainFrame::OnButtonCurve)
	ON_COMMAND(ID_BUTTON_DATA, &CMainFrame::OnButtonData)
	ON_COMMAND(ID_BUTTON_AUTOSCALE, &CMainFrame::OnButtonAutoscale)

	//ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_WM_SETTINGCHANGE()
	
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2005);
	windowNum_ = 1;
	iShowCurve = 1;
	iShowData = 1;

	curFrameIndex = 0;
}

CMainFrame::~CMainFrame()
{
}
void CMainFrame::OnButtonCurve()
{
	if(!iShowCurve)
	{
		iShowCurve = 1;
		//m_wndOutput.SetAutoHideMode(true, CBRS_ALIGN_BOTTOM); 
		m_wndOutput.ShowPane(TRUE,FALSE,TRUE);   
	}
	else
	{
		iShowCurve = 0;
		//m_wndOutput.SetAutoHideMode(true, CBRS_ALIGN_BOTTOM);
		m_wndOutput.ShowPane(FALSE,FALSE,TRUE); 
	}
}
void CMainFrame::OnButtonAutoscale()
{
	// TODO: 在此添加命令处理程序代码

	for (int i = 0; i < windowNum_; i++)
	{
		cOSG* osg = viewSet[i]->getOSG();
		osg->UpdateScale();
	}
	
}

void CMainFrame::OnButtonData()
{
	if(!iShowData)
	{
		iShowData = 1;
		//m_wndOutput.SetAutoHideMode(true, CBRS_ALIGN_BOTTOM); 
		m_wndClassView.ShowPane(TRUE,FALSE,TRUE);   
	}
	else
	{
		iShowData = 0;
		//m_wndOutput.SetAutoHideMode(true, CBRS_ALIGN_BOTTOM);
		m_wndClassView.ShowPane(FALSE,FALSE,TRUE); 
	}
}
void CMainFrame::OnDestroy()
{
	windowNum_ = 0;
	CFrameWndEx::OnDestroy();
}
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	return m_wndSplitter.Create(this,
		2, 2,			// TODO: adjust the number of rows, columns
		CSize(10, 10),	// TODO: adjust the minimum pane size
		pContext);
}
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;
	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// prevent the menu bar from taking the focus on activation
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar.SetSizes(CSize(33,33),CSize(32,32));

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	// TODO: Delete these five lines if you don't want the toolbar and menubar to be dockable
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// create docking windows
	if (!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}

	m_TimePane.EnableDocking(CBRS_ALIGN_ANY);
	/*RECT re;
	re.bottom = 0;re.top = 100;re.left = 0; re.right = 100;*/
	DockPane(&m_TimePane);
	//m_wndFileView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndClassView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndClassView);
	//CDockablePane* pTabbedBar = NULL;
	//m_wndClassView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	
	

	//m_TimePane.DockToFrameWindow(CBRS_BOTTOM);

	// Enable enhanced windows management dialog
	//EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// Enable toolbar and docking window menu replacement
	CString strCustomize = L"";
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// enable quick (Alt+drag) toolbar customization
	//CMFCToolBar::EnableQuickCustomization();

	//CMFCToolBar::SetBasicCommands(lstBasicCommands);

	// Switch the order of document name and application name on the window title bar. This
	// improves the usability of the taskbar because the document name is visible with the thumbnail.
	ModifyStyle(0, FWS_PREFIXTITLE);

	int index = 9;// 占位符索引
	int width = 100;// 占位符宽度
	m_wndToolBar.SetButtonInfo(index,IDC_EDIT_PATH,TBBS_SEPARATOR,width);

	CString epathstr;//存储
	GetPrivateProfileString(_T("Export"),_T("ExportPath"),NULL,epathstr.GetBuffer(100),100,_T(".\\option.ini"));

	CRect rect;
	m_wndToolBar.GetItemRect(index,&rect);
	rect.top = 5;
	rect.bottom = rect.top + 25;
	rect.right = rect.left + 150;
	m_editExportPath.CreateEx(WS_EX_CLIENTEDGE, _T("EDIT"), epathstr, WS_CHILD|WS_VISIBLE,
		rect,&m_wndToolBar,IDC_EDIT_PATH);

	this->GetDockingManager()->DisableRestoreDockState(TRUE);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
    cs.style &= ~(LONG) FWS_ADDTOTITLE;           
	return CFrameWnd::PreCreateWindow(cs);
	//return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// Create class view
	CString strClassView;
	bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
	ASSERT(bNameValid);
	if (!m_wndClassView.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Class View window\n");
		return FALSE; // failed to create
	}

	// Create output window
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 250), TRUE, ID_VIEW_TIMEPANE, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Time window\n");
		return FALSE; // failed to create
	}

	// Create Time window
	if (!m_TimePane.Create(_T("Time"), this, CRect(0, 0, 100, 30), FALSE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI,  AFX_CBRS_REGULAR_TABS, AFX_CBRS_CLOSE))
	{
		TRACE0("Failed to create Output window\n");
		return FALSE; // failed to create
	}
	

	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	/*HICON hClassViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndClassView.SetIcon(hClassViewIcon, FALSE);

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);
*/
	//UpdateMDITabbedBarsIcons();
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

//void CMainFrame::OnWindowManager()
//{
//	ShowWindowsDialog();
//}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// base class does the real work

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// enable customization button for all user toolbars
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
	//m_wndOutput.UpdateFonts();
}

void CMainFrame::splitColumn()
{
	CRect rect;  
	m_wndSplitter.GetClientRect(&rect); 
	if (m_wndSplitter.GetColumnCount()==1)
	{
		m_wndSplitter.SplitColumn(rect.Width()/2);
	}
	else
	{
		m_wndSplitter.SetColumnInfo(0,rect.Width()/2,0);
		m_wndSplitter.ShowVSplider();
	}
}
void CMainFrame::splitRow()
{
	CRect rect;  
	m_wndSplitter.GetClientRect(&rect); 
	if (m_wndSplitter.GetRowCount()==1)
	{
		m_wndSplitter.SplitRow(rect.Height()/2);
	}
	else
	{
		m_wndSplitter.SetRowInfo(0,rect.Height()/2,0);
		m_wndSplitter.ShowHSplider();
	}
}
void CMainFrame::mergeColumn()
{
	CRect rect;  
	m_wndSplitter.GetClientRect(&rect); 
	if (m_wndSplitter.GetColumnCount()==2)
	{
		m_wndSplitter.SetColumnInfo(0,rect.Width(),0);
		m_wndSplitter.hideVSplider();
	}
}
void CMainFrame::mergeRow()
{
	CRect rect;  
	m_wndSplitter.GetClientRect(&rect); 
	if (m_wndSplitter.GetRowCount()==2)
	{
		m_wndSplitter.SetRowInfo(0,rect.Height(),0);
		m_wndSplitter.hideHSplider();
	}
}
void CMainFrame::convertWindowNum(int toWinNum)
{
	int preWinN = windowNum_;

	if (preWinN == 1 && toWinNum == 2)
	{
		splitColumn();
	}
	if (preWinN == 1 && toWinNum == 4)
	{
		splitColumn();
		splitRow();
	}
	if (preWinN == 2 && toWinNum == 4)
	{
		splitRow();
	}

	if (preWinN == 2 && toWinNum == 1)
	{
		mergeColumn();
	}
	if (preWinN == 4 && toWinNum == 1)
	{
		mergeRow();
		mergeColumn();
	}
	if (preWinN == 4 && toWinNum == 2)
	{
		mergeRow();
	}
	windowNum_  = toWinNum;
	m_wndSplitter.RecalcLayout();
}
void CMainFrame::OnWindowOnewindow()
{
	if (windowNum_ != 1)
	{
		convertWindowNum(1);
	}
}
void CMainFrame::OnWindowTwowindow()
{
	if (windowNum_ != 2)
	{
		convertWindowNum(2);
	}

}
void CMainFrame::OnWindowFourwindow()
{
	if (windowNum_ != 4)
	{
		convertWindowNum(4);
	}

}
void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWndEx::OnSize(nType, cx, cy);

	if (m_wndSplitter.GetRowCount() == 1 && m_wndSplitter.GetColumnCount() == 1)
	{
		return;
	}
	//m_chart.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	CRect Rect;
	m_wndSplitter.GetClientRect(&Rect); 
	GetClientRect(Rect);
	if (windowNum_ == 1)
	{
		if (m_wndSplitter.GetRowCount() == 2)
			m_wndSplitter.SetRowInfo(0,Rect.Height(),0);
		if (m_wndSplitter.GetColumnCount() == 2)
			m_wndSplitter.SetColumnInfo(0,Rect.Width(),0);
		m_wndSplitter.RecalcLayout();
	}
	if (windowNum_ == 2)
	{
		if (m_wndSplitter.GetRowCount() == 2)
			m_wndSplitter.SetRowInfo(0,Rect.Height(),0);
		if (m_wndSplitter.GetColumnCount() == 2)
			m_wndSplitter.SetColumnInfo(0,Rect.Width()/2,0);
		m_wndSplitter.RecalcLayout();
	}
	if (windowNum_ == 4)
	{
		if (m_wndSplitter.GetRowCount() == 2)
			m_wndSplitter.SetRowInfo(0,Rect.Height()/2,0);
		if (m_wndSplitter.GetColumnCount() == 2)
			m_wndSplitter.SetColumnInfo(0,Rect.Width()/2,0);
		m_wndSplitter.RecalcLayout();
	}
}