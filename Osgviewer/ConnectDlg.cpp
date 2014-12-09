// ConnectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Osgviewer.h"
#include "ConnectDlg.h"
#include "afxdialogex.h"


// ConnectDlg dialog

IMPLEMENT_DYNAMIC(ConnectDlg, CDialogEx)

ConnectDlg::ConnectDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(ConnectDlg::IDD, pParent)
{
	
}

ConnectDlg::~ConnectDlg()
{
}


void ConnectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ConnectDlg, CDialogEx)
	
	ON_BN_CLICKED(IDC_BUTTON_OK, &ConnectDlg::OnBnClickedButtonOk)
	
	ON_BN_CLICKED(IDOK, &ConnectDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// ConnectDlg message handlers
BOOL ConnectDlg::OnInitDialog()
{
	CString ipstr;//´æ´¢IPµØÖ·
	GetPrivateProfileString(_T("Server"),_T("ServerIP"),NULL,ipstr.GetBuffer(40),40,_T(".\\option.ini"));
	GetDlgItem(IDC_EDIT_IP)->SetWindowText(ipstr);

	CString portstr;//´æ´¢
	GetPrivateProfileString(_T("Server"),_T("Port"),NULL,portstr.GetBuffer(40),40,_T(".\\option.ini"));
	GetDlgItem(IDC_EDIT_PORT)->SetWindowText(portstr);

	return 1;
}

void ConnectDlg::OnBnClickedButtonOk()
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_EDIT_IP)->GetWindowText(ip);
	GetDlgItem(IDC_EDIT_PORT)->GetWindowText(port);
	CDialog::OnOK();
}


void ConnectDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}
