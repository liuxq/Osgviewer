
// OsgviewerDoc.cpp : implementation of the COsgviewerDoc class
//

#include "stdafx.h"
#include "commonData.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Osgviewer.h"
#endif

#include "OsgviewerDoc.h"
#include "OsgviewerView.h"


#include "MFC_OSG.h"
#include "winsock2.h"
#include "MainFrm.h"
#include "OutputWnd.h"
#include "resource.h"
#include "ConnectDlg.h"
#include "ExportDlg.h"
#include "OptionDlg.h"


#include <propkey.h>

#include "bvh.h"

//#include <fstream>
//using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern COsgviewerView* viewSet[4];
extern int realWindowNum;
extern int windowNum_;

extern list<vector<sDataBufferEngine>> framedatas;
extern CCriticalSection g_cs;
extern CCriticalSection g_csForView;
//extern COutputWnd* gOutputWnd;
extern map<QString,int> dataNameMap;

// COsgviewerDoc

IMPLEMENT_DYNCREATE(COsgviewerDoc, CDocument)

BEGIN_MESSAGE_MAP(COsgviewerDoc, CDocument)
	ON_COMMAND(ID_CONNECT_TCP, &COsgviewerDoc::OnTCLConnect)
	ON_COMMAND(ID_BUTTON_CONNECT, &COsgviewerDoc::OnTCLConnect)
	ON_COMMAND(ID_OPTION_PREFERENCES, &COsgviewerDoc::OnOptionPreference)
	ON_COMMAND(ID_FILE_EXPORT, &COsgviewerDoc::OnFileExport)
	ON_COMMAND(ID_FILE_RECORD, &COsgviewerDoc::OnRecord)
	ON_COMMAND(ID_FILE_STOP, &COsgviewerDoc::OnStop)
END_MESSAGE_MAP()


// COsgviewerDoc construction/destruction

COsgviewerDoc::COsgviewerDoc()
{
	// TODO: add one-time construction code here
	realWindowNum = 0;

	QString dataNameList[28] = {"hip","hip1","hip2","hip3","chest","neck","head","",
		"lCollar","lShldr","lForeArm","lHand","",
		"rCollar","rShldr","rForeArm","rHand","",
		"lThigh","lShin","lFoot","lFoot1","",
		"rThigh","rShin","rFoot","rFoot1","",
	};

	for(int i = 0; i < 28; i++)
	{
		if (dataNameList[i] != "")
		{
			dataNameMap[dataNameList[i]] = i;
		}
	}
	recordStart = -1;
	recordEnd = -1;
	curFrameIndex = 0;
}
volatile bool isFinish = false;
COsgviewerDoc::~COsgviewerDoc()
{
	isFinish = true;
}
void COsgviewerDoc::OnRecord()
{
	vector<sDataBufferEngine>* pFrameData = getCurFrame(curFrameIndex);
	if (!pFrameData)
		return;
	vector<sDataBufferEngine>& framedata = *pFrameData;
	recordStart = framedata.size();

	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	pMain->getTimePane().startRecord();

	/*ifstream lxqin("lxq.bb",ios::binary);
	unsigned int sz = -1;
	lxqin.read((char*)&sz, sizeof(sz));
	framedata.clear();
	framedata.resize(sz);
	for (int i = 0; i <sz; i++)
	{
	lxqin.read((char*)&framedata.at(i), sizeof(sDataBufferEngine));
	}*/
}
void COsgviewerDoc::OnStop()
{
	vector<sDataBufferEngine>* pFrameData = getCurFrame(curFrameIndex);
	if (!pFrameData)
		return;
	vector<sDataBufferEngine>& framedata = *pFrameData;
	recordEnd = framedata.size()-1;

	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	pMain->getTimePane().stopRecord();

	

	/*g_cs.Lock();
	ofstream lxqout("lxq.bb",ios::binary);
	unsigned int sz = framedata.size();
	lxqout.write((char*)&sz, sizeof(sz));
	for (int i = 0; i < framedata.size(); i++)
	{
	lxqout.write((char*)&framedata.at(i), sizeof(sDataBufferEngine));
	}
	g_cs.Unlock();*/
}

volatile int connectState = 1;
void Render(void * b)
{	
	sDataBufferEngine sending_buffer;

	/*while(1)
	{*/
	CString strIP;
	int nPort;
	if (connectState == 0)
	{
		GetPrivateProfileString(_T("Server"),_T("ServerIP"),NULL,strIP.GetBuffer(40),40,_T(".\\option.ini"));
 
		CString portstr;//存储
		GetPrivateProfileString(_T("Server"),_T("Port"),NULL,portstr.GetBuffer(40),40,_T(".\\option.ini"));
		nPort = _ttoi(portstr);

		connectState = 1;
	}
	else
	{
		ConnectDlg optionD;
		optionD.DoModal();
		strIP = optionD.ip;//_T("192.168.1.2");
		//转换需要连接的端口内容类型
		nPort = _ttoi(optionD.port);
	}

	AfxSocketInit();
	CSocket aSocket;
	//初始化 CSocket 对象, 因为客户端不需要绑定任何端口和地址, 所以用默认参数即可
	if(!aSocket.Create())
	{
		AfxMessageBox(_T("error!"));
		return;
	}
		
	//连接指定的地址和端口
	if(aSocket.Connect(strIP, nPort))
	{
		char szRecValue[1024] = {0};
		g_cs.Lock();
		framedatas.resize(framedatas.size()+1);
		vector<sDataBufferEngine>& framedata = framedatas.back();
		g_cs.Unlock();
		CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		pMain->getClassView().addPerson();

		//增加一个模型
		cOSG::addSkeleton();
		for (int i = 0; i < windowNum_; i++)
		{
			cOSG* osg = viewSet[i]->getOSG();
			osg->ResetSkeleton();
		}
		//sDataBufferEngine sdb;
		while(aSocket.Receive((void *)&sending_buffer, sizeof(sending_buffer))) //接收服务器发送回来的内容(该方法会阻塞, 在此等待有内容接收到才继续向下执行)
		{
			//osg->skeleton->sBuffer1.setBySdb(sdb);
			g_cs.Lock();

			framedata.push_back(sending_buffer);
			g_cs.Unlock();
		}
	}
	else
	{
		AfxMessageBox(_T("连接失败，请检查地址"));
	}
	aSocket.Close();

	//}

	_endthread();
}

void Render2(void * b)
{	
	sDataBufferEngine sending_buffer;
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	while(!isFinish)
	{
		if (connectState == 0)
		{
			Sleep(100);
		}
		if (realWindowNum == 0)
		{
			if (windowNum_ == 0)
			{
				break;
			}
			continue;
		}
		//更新模型信息
		g_cs.Lock();
		int j = 0;
		for (list<vector<sDataBufferEngine>>::iterator i = framedatas.begin(); i != framedatas.end(); i++, j++)
		{
			if (!(i->empty()))
			{
				cOSG::skeletonArray[j]->sBuffer1 = i->back();
			}
		}
		g_cs.Unlock();

		//刷新窗口
		g_csForView.Lock();
		for (int i = 0; i < windowNum_; i++)
		{
			viewSet[i]->isConnected = true;
			cOSG* osg = viewSet[i]->getOSG();
			osg->UpdateFrame();
		}

		pMain->getOutPutWnd().updateChart();
		pMain->getTimePane().calculate();
		g_csForView.Unlock();
	}
	_endthread();

}

BOOL COsgviewerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	m_csFileName = L"biped.3ds";

	isFinish = false;
	mThreadHandle2 = (HANDLE)_beginthread(Render2, 0, 0); 

	connectState = 0;
	//mThreadHandle = (HANDLE)_beginthread(Render, 0, 0); 
    this->SetTitle(_T("lxq"));

	return TRUE;
}


// COsgviewerDoc serialization

void COsgviewerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

void COsgviewerDoc::OnTCLConnect()
{
	mThreadHandle = (HANDLE)_beginthread(Render, 0, 0); 
	
}
void COsgviewerDoc::OnOptionPreference()
{
	OptionDlg optionD;
	optionD.DoModal();
}
void COsgviewerDoc::OnFileExport()
{
	if (recordStart == -1 || recordEnd == -1)
	{
		vector<sDataBufferEngine>* pFrameData = getCurFrame(curFrameIndex);
		if (!pFrameData)
			return;
		vector<sDataBufferEngine>& framedata = *pFrameData;
		ExportDlg eDlg(0, framedata.size()-1);
		eDlg.DoModal();
	}
	else
	{
		ExportDlg eDlg(recordStart, recordEnd);
		eDlg.DoModal();
	}	
}
void COsgviewerDoc::OnInitialUpdate()
{
	
}
#ifdef SHARED_HANDLERS

// Support for thumbnails
void COsgviewerDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void COsgviewerDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void COsgviewerDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// COsgviewerDoc diagnostics

#ifdef _DEBUG
void COsgviewerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void COsgviewerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// COsgviewerDoc commands
