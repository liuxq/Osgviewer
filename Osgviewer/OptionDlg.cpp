// OptionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Osgviewer.h"
#include "OptionDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"


// OptionDlg dialog

IMPLEMENT_DYNAMIC(OptionDlg, CDialogEx)

OptionDlg::OptionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(OptionDlg::IDD, pParent)
{

}

OptionDlg::~OptionDlg()
{
}

BOOL OptionDlg::OnInitDialog()
{
	((CComboBox*)GetDlgItem(IDC_COMBO_OPTIONEXPORT_TYPE))->AddString(_T("*.BVH File(MotionBuilder)"));
	((CComboBox*)GetDlgItem(IDC_COMBO_OPTIONEXPORT_TYPE))->AddString(_T("*.BVH File(3DMAX)"));
	((CComboBox*)GetDlgItem(IDC_COMBO_OPTIONEXPORT_TYPE))->AddString(_T("*.FBX File"));
	((CComboBox*)GetDlgItem(IDC_COMBO_OPTIONEXPORT_TYPE))->AddString(_T("*.C3D File"));

	CString ipstr;//存储IP地址
	GetPrivateProfileString(_T("Server"),_T("ServerIP"),NULL,ipstr.GetBuffer(40),40,_T(".\\option.ini"));
	GetDlgItem(IDC_EDIT_OPTION_IP)->SetWindowText(ipstr);

	CString portstr;//存储
	GetPrivateProfileString(_T("Server"),_T("Port"),NULL,portstr.GetBuffer(40),40,_T(".\\option.ini"));
	GetDlgItem(IDC_EDIT_OPTION_PORT)->SetWindowText(portstr);

	CString epathstr;//存储
	GetPrivateProfileString(_T("Export"),_T("ExportPath"),NULL,epathstr.GetBuffer(100),100,_T(".\\option.ini"));
	GetDlgItem(IDC_EDIT_OPTION_EXPORTPATH)->SetWindowText(epathstr);

	CString epathtype;//存储
	GetPrivateProfileString(_T("Export"),_T("ExportType"),NULL,epathtype.GetBuffer(100),100,_T(".\\option.ini"));
	((CComboBox*)GetDlgItem(IDC_COMBO_OPTIONEXPORT_TYPE))->SetCurSel(_ttoi(epathtype));

	return true;
}

void OptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(OptionDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &OptionDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_OPTION_BROWSE, &OptionDlg::OnBnClickedButtonOptionBrowse)
END_MESSAGE_MAP()


// OptionDlg message handlers


void OptionDlg::OnBnClickedOk()
{
	CDialogEx::OnOK();
	CString ipstr;
	GetDlgItem(IDC_EDIT_OPTION_IP)->GetWindowText(ipstr);
	WritePrivateProfileString(_T("Server"),_T("Port"),ipstr,_T(".\\option.ini"));

	CString portstr;
	GetDlgItem(IDC_EDIT_OPTION_PORT)->GetWindowText(portstr);
	WritePrivateProfileString(_T("Server"),_T("Port"),portstr,_T(".\\option.ini"));

	CString epathstr;
	GetDlgItem(IDC_EDIT_OPTION_EXPORTPATH)->GetWindowText(epathstr);
	WritePrivateProfileString(_T("Export"),_T("ExportPath"),epathstr,_T(".\\option.ini"));

	int cursel = ((CComboBox*)GetDlgItem(IDC_COMBO_OPTIONEXPORT_TYPE))->GetCurSel();
	CString curselstr;
	curselstr.Format(_T("%d"),cursel);
	WritePrivateProfileString(_T("Export"),_T("ExportType"),curselstr,_T(".\\option.ini"));

	CString epathstr1;//存储
	GetPrivateProfileString(_T("Export"),_T("ExportPath"),NULL,epathstr1.GetBuffer(100),100,_T(".\\option.ini"));
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	pMain->m_editExportPath.SetWindowText(epathstr1);
}


void OptionDlg::OnBnClickedButtonOptionBrowse()
{
	CString str;
	BROWSEINFO bi;
	TCHAR name[MAX_PATH];
	ZeroMemory(&bi,sizeof(BROWSEINFO));
	bi.hwndOwner = GetSafeHwnd();
	bi.pszDisplayName = name;
	bi.lpszTitle = _T("Select folder");
	//bi.ulFlags = BIF_USENEWUI;
	bi.ulFlags = BIF_RETURNFSANCESTORS;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if(idl == NULL)
		return;
	SHGetPathFromIDList(idl, str.GetBuffer(MAX_PATH));
	str.ReleaseBuffer();
	CString m_strPath = str;//为对话框中与一编辑框对应的CString型变量，保存并显示选中的路径。
	if(str.GetAt(str.GetLength()-1)!='\\')
		m_strPath+="\\";
	UpdateData(FALSE); 

	GetDlgItem(IDC_EDIT_OPTION_EXPORTPATH)->SetWindowText(m_strPath);
}
