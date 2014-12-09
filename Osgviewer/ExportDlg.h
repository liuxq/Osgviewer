#pragma once
#include "afxwin.h"


// ExportDlg dialog

class ExportDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ExportDlg)

public:
	ExportDlg(int,int, CWnd* pParent = NULL);   // standard constructor
	virtual ~ExportDlg();

	CString m_strPath;

	int startFrameIdx;
	int endFrameIdx;

// Dialog Data
	enum { IDD = IDD_PROPPAGE_EXPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBrowse();
	afx_msg void OnBnClickedButtonExport();
	afx_msg void OnBnClickedButtonCancel();
};
