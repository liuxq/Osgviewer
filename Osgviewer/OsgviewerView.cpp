
// OsgviewerView.cpp : implementation of the COsgviewerView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Osgviewer.h"
#endif

#include "OsgviewerDoc.h"
#include "OsgviewerView.h"
#include "MainFrm.h"

#include "OutputWnd.h"

#include "time.h"


extern COsgviewerView* viewSet[4];
extern int realWindowNum;

extern CCriticalSection g_csForView;



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COsgviewerView

IMPLEMENT_DYNCREATE(COsgviewerView, CView)

BEGIN_MESSAGE_MAP(COsgviewerView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &COsgviewerView::OnFilePrintPreview)

	ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_KEYDOWN()
    ON_WM_ERASEBKGND()

	ON_COMMAND(ID_CAMERACONTROL_FOLLOW, &COsgviewerView::OnContorlFollow)
	ON_UPDATE_COMMAND_UI(ID_CAMERACONTROL_FOLLOW, &COsgviewerView::OnUpdateContorlFollow)
	ON_COMMAND(ID_CAMERACONTROL_USERCONTROL, &COsgviewerView::OnContorlUsercontrol)
	ON_UPDATE_COMMAND_UI(ID_CAMERACONTROL_USERCONTROL, &COsgviewerView::OnUpdateContorlUsercontrol)
	ON_COMMAND(ID_CAMERACONTROL_FOLLOW_TOP, &COsgviewerView::OnContorlFollowTop)
	ON_UPDATE_COMMAND_UI(ID_CAMERACONTROL_FOLLOW_TOP, &COsgviewerView::OnUpdateContorlFollowTop)
	ON_COMMAND(ID_CAMERACONTROL_FOLLOW_RIGHT, &COsgviewerView::OnContorlFollowRight)
	ON_UPDATE_COMMAND_UI(ID_CAMERACONTROL_FOLLOW_RIGHT, &COsgviewerView::OnUpdateContorlFollowRight)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_SCENECONTROL_SHOWFLOOR, &COsgviewerView::OnScenecontrolShowf)
	ON_UPDATE_COMMAND_UI(ID_SCENECONTROL_SHOWFLOOR, &COsgviewerView::OnUpdateScenecontrolShowf)
	ON_COMMAND(ID_SCENECONTROL_SHOWSHADOW, &COsgviewerView::OnScenecontrolShowshadow)
	ON_UPDATE_COMMAND_UI(ID_SCENECONTROL_SHOWSHADOW, &COsgviewerView::OnUpdateScenecontrolShowshadow)
	ON_COMMAND(ID_SCENECONTROL_SHOWLABEL, &COsgviewerView::OnScenecontrolShowlable)
	ON_UPDATE_COMMAND_UI(ID_SCENECONTROL_SHOWLABEL, &COsgviewerView::OnUpdateScenecontrolShowlable)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_SCENECONTROL_AUTOSCALESKELETON, &COsgviewerView::OnScenecontrolAutoscaleskeleton)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEHOVER()
END_MESSAGE_MAP()

// COsgviewerView construction/destruction

COsgviewerView::COsgviewerView():isConnected(false)
{
	// TODO: add construction code here
	iFollow = 1;
	bShowFloor = bShowShadow = bShowLable = 1;
	bIn = true;
}

COsgviewerView::~COsgviewerView()
{
}

BOOL COsgviewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// COsgviewerView drawing

void COsgviewerView::OnDraw(CDC* /*pDC*/)
{
	/*if (!isConnected)
		mOSG->UpdateFrame();*/
	COsgviewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// COsgviewerView printing


void COsgviewerView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL COsgviewerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}
int COsgviewerView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

    mOSG = new cOSG(m_hWnd);

    return 1;
}
void COsgviewerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}
void COsgviewerView::OnDestroy()
{
	
	/*g_csForView.Lock();
	for(vector<COsgviewerView*>::iterator i = viewSet.begin(); i != viewSet.end(); i++)
	{
		if (*i == this)
		{
			viewSet.erase(i);
			break;
		}
	}

	g_csForView.Unlock();*/

    if(mOSG != 0) delete mOSG;
    //WaitForSingleObject(mThreadHandle, 1000);
	realWindowNum--;
    CView::OnDestroy();
}
void COsgviewerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void COsgviewerView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	CString csFileName = GetDocument()->GetFileName();
// COsgviewerView diagnostics
	char chr[30];
	WideCharToMultiByte( CP_ACP, 0, csFileName.GetBuffer(0), -1,  
		chr, sizeof(chr), NULL, NULL );
	
	
	mOSG->InitOSG(chr);
	
	mOSG->Follow(iFollow);

	/*CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	gOutputWnd = &(pMain->getOutPutWnd());*/

	mOSG->getViewer()->frame();

	g_csForView.Lock();
	viewSet[realWindowNum++] = this;
	g_csForView.Unlock();

	//mThreadHandle = (HANDLE)_beginthread(&cOSG::Render, 0, mOSG); 
}

void COsgviewerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}


void COsgviewerView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Pass Key Presses into OSG
	mOSG->getViewer()->getEventQueue()->keyPress(nChar);

	// Close Window on Escape Key
	if(nChar == VK_ESCAPE)
	{
		GetParent()->SendMessage(WM_CLOSE);
	}
}

void COsgviewerView::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == 1)
	{

	}
	CView::OnTimer(nIDEvent);
}


BOOL COsgviewerView::OnEraseBkgnd(CDC* pDC)
{
	return true;
}
#ifdef _DEBUG
void COsgviewerView::AssertValid() const
{
	CView::AssertValid();
}

void COsgviewerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

COsgviewerDoc* COsgviewerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COsgviewerDoc)));
	return (COsgviewerDoc*)m_pDocument;
}
#endif //_DEBUG


void COsgviewerView::OnContorlFollow()
{
	// TODO: 在此添加命令处理程序代码
	iFollow = 1;
	mOSG->Follow(iFollow);
	Invalidate(FALSE);
}


void COsgviewerView::OnUpdateContorlFollow(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetRadio(iFollow==1);
}


void COsgviewerView::OnContorlUsercontrol()
{
	// TODO: 在此添加命令处理程序代码
	iFollow = 0;
	mOSG->Follow(iFollow);
	Invalidate(FALSE);
}


void COsgviewerView::OnUpdateContorlUsercontrol(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetRadio(iFollow==0);
}


void COsgviewerView::OnContorlFollowTop()
{
	// TODO: 在此添加命令处理程序代码
	iFollow = 3;
	mOSG->Follow(iFollow);
	Invalidate(FALSE);
}


void COsgviewerView::OnUpdateContorlFollowTop(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetRadio(iFollow==3);
}


void COsgviewerView::OnContorlFollowRight()
{
	// TODO: 在此添加命令处理程序代码
	iFollow = 2;
	mOSG->Follow(iFollow);
	Invalidate(FALSE);
}


void COsgviewerView::OnUpdateContorlFollowRight(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetRadio(iFollow==2);
}




void COsgviewerView::OnScenecontrolShowf()
{
	// TODO: 在此添加命令处理程序代码
	bShowFloor = !bShowFloor;
	mOSG->ShowFloor(bShowFloor);
	Invalidate(FALSE);
}


void COsgviewerView::OnUpdateScenecontrolShowf(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(bShowFloor);
}


void COsgviewerView::OnScenecontrolShowshadow()
{
	// TODO: 在此添加命令处理程序代码
	bShowShadow = !bShowShadow;
	mOSG->ShowShadow(bShowShadow);
	Invalidate(FALSE);
}


void COsgviewerView::OnUpdateScenecontrolShowshadow(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(bShowFloor && bShowShadow);
}


void COsgviewerView::OnScenecontrolShowlable()
{
	// TODO: 在此添加命令处理程序代码
	bShowLable = !bShowLable;
	mOSG->ShowLable(bShowLable);
	Invalidate(FALSE);
}


void COsgviewerView::OnUpdateScenecontrolShowlable(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(bShowLable);
}

void COsgviewerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_MENU_SHOWOPTION);
	menu.GetSubMenu(0)
		->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y, this);

}
void COsgviewerView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(lHint == 123456789)
	{
		mOSG->SetPosAndRot((float*)pHint);
		if(iWheel)
		{
			mOSG->zdelta = iWheel;
			iWheel = 0;
		}
	}
}

void COsgviewerView::OnScenecontrolAutoscaleskeleton()
{
	// TODO: 在此添加命令处理程序代码
	mOSG->UpdateScale();
}

void COsgviewerView::OnMouseMove(UINT nFlags, CPoint point)
{
	mOSG->iButton = 0;
	if(nFlags & MK_LBUTTON )
	{
		mOSG->iButton = 1;
	}
	else if(nFlags & MK_MBUTTON )
	{
		mOSG->iButton = 2;
	}
	else if(nFlags & MK_RBUTTON )
	{
		mOSG->iButton = 3;
	}
	else
	{
		mOSG->x0 = point.x;
		mOSG->y0 = point.y;
	}
	mOSG->x = point.x;
	mOSG->y = point.y;
	CView::OnMouseMove(nFlags, point);

	//TRACKMOUSEEVENT   tme;   
	//tme.cbSize=sizeof(TRACKMOUSEEVENT);   
	//tme.dwFlags=TME_HOVER   |   TME_LEAVE;   
	////tme.dwHoverTime=HOVER_DEFAULT;  
	//tme.hwndTrack=m_hWnd; 
	//_TrackMouseEvent(&tme);
	TRACKMOUSEEVENT tme;  
	tme.cbSize = sizeof(tme);  
	tme.hwndTrack = m_hWnd;  
	tme.dwFlags = TME_LEAVE|TME_HOVER;  
	tme.dwHoverTime = 1;  
	BOOL m_bTracking = _TrackMouseEvent(&tme);
}
BOOL COsgviewerView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// 此功能要求 Windows Vista 或更高版本。
	// _WIN32_WINNT 符号必须 >= 0x0600。
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	mOSG->zdelta = zDelta; 
	CView::OnMouseHWheel(nFlags, zDelta, pt);
	return TRUE;
}
void COsgviewerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	mOSG->x = point.x;
	mOSG->y = point.y;
	mOSG->x0 = point.x;
	mOSG->y0 = point.y;
	CView::OnLButtonDown(nFlags, point);
}
void COsgviewerView::OnMButtonDown(UINT nFlags, CPoint point)
{
	mOSG->x = point.x;
	mOSG->y = point.y;
	mOSG->x0 = point.x;
	mOSG->y0 = point.y;
	CView::OnMButtonDown(nFlags, point);
}
void COsgviewerView::OnRButtonDown(UINT nFlags, CPoint point)
{
	mOSG->x = point.x;
	mOSG->y = point.y;
	mOSG->x0 = point.x;
	mOSG->y0 = point.y;
	CView::OnRButtonDown(nFlags, point);
}
void COsgviewerView::OnMouseLeave()
{
	bIn = false;
	mOSG->iButton = 0;
	mOSG->x = 0;
	mOSG->y = 0;
	mOSG->x0 = 0;
	mOSG->y0 = 0;
	CView::OnMouseLeave();
}
void COsgviewerView::OnMouseHover(UINT nFlags, CPoint point)
{
	//Invalidate(FALSE);
	bIn = true;
	CView::OnMouseHover(nFlags, point);
}