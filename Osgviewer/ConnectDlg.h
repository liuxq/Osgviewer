#pragma once


// ConnectDlg dialog

class ConnectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ConnectDlg)

public:
	ConnectDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ConnectDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_CONNECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOk();

	CString ip;
	CString port;
	afx_msg void OnBnClickedOk();
};
