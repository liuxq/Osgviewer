
// Osgviewer.h : main header file for the Osgviewer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// COsgviewerApp:
// See Osgviewer.cpp for the implementation of this class
//

class COsgviewerApp : public CWinAppEx
{
public:
	COsgviewerApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern COsgviewerApp theApp;
