// ExportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Osgviewer.h"
#include "ExportDlg.h"
#include "afxdialogex.h"
#include "DeviceBuffer.h"

#include "bvh.h"
#include "fbx.h"
#include "c3d.h"

extern list<vector<sDataBufferEngine>> framedatas;
// ExportDlg dialog

IMPLEMENT_DYNAMIC(ExportDlg, CDialogEx)

ExportDlg::ExportDlg(int startFrame, int endFrame, CWnd* pParent /*=NULL*/)
	: CDialogEx(ExportDlg::IDD, pParent),startFrameIdx(startFrame), endFrameIdx(endFrame)
{
	
}

ExportDlg::~ExportDlg()
{
}



BEGIN_MESSAGE_MAP(ExportDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &ExportDlg::OnBnClickedButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, &ExportDlg::OnBnClickedButtonExport)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &ExportDlg::OnBnClickedButtonCancel)
END_MESSAGE_MAP()


// ExportDlg message handlers

void ExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL ExportDlg::OnInitDialog()
{
	//m_combobox_type.AddString(_T("绘制文件类型 (*.bvh)"));
	((CComboBox*)GetDlgItem(IDC_COMBO_EXPORT_TYPE))->AddString(_T("*.BVH File(MotionBuilder)"));
	((CComboBox*)GetDlgItem(IDC_COMBO_EXPORT_TYPE))->AddString(_T("*.BVH File(3DMAX)"));
	((CComboBox*)GetDlgItem(IDC_COMBO_EXPORT_TYPE))->AddString(_T("*.FBX File"));
	((CComboBox*)GetDlgItem(IDC_COMBO_EXPORT_TYPE))->AddString(_T("*.C3D File"));

	int frameSize = framedatas.size();
	CString strNum;
	for (int i = 0; i < frameSize; i++)
	{
		strNum.Format(_T("%d"), i);
		((CComboBox*)GetDlgItem(IDC_COMBO_PERSON_INDEX))->AddString(strNum);
	}
	
	CString epathtype;//存储
	GetPrivateProfileString(_T("Export"),_T("ExportType"),NULL,epathtype.GetBuffer(100),100,_T(".\\option.ini"));
	((CComboBox*)GetDlgItem(IDC_COMBO_EXPORT_TYPE))->SetCurSel(_ttoi(epathtype));
	CString startFrameStr;
	startFrameStr.Format(_T("%d"), startFrameIdx);
	GetDlgItem(IDC_EDIT_STARTFRAME)->SetWindowText(startFrameStr);
	CString endFrameStr;
	endFrameStr.Format(_T("%d"), endFrameIdx);
	GetDlgItem(IDC_EDIT_ENDFRAME)->SetWindowText(endFrameStr);
	GetDlgItem(IDC_STATIC_MAXFRAME)->SetWindowText(_T("max:") + endFrameStr);

	CString epathstr;//存储
	GetPrivateProfileString(_T("Export"),_T("ExportPath"),NULL,epathstr.GetBuffer(100),100,_T(".\\option.ini"));
	GetDlgItem(IDC_EDIT_FILEPATH)->SetWindowText(epathstr);
	return true;
}
void ExportDlg::OnBnClickedButtonBrowse()
{

	// TODO: Add your control notification handler code here
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
	m_strPath = str;//为对话框中与一编辑框对应的CString型变量，保存并显示选中的路径。
	if(str.GetAt(str.GetLength()-1)!='\\')
		m_strPath+="\\";
	UpdateData(FALSE); 

	GetDlgItem(IDC_EDIT_FILEPATH)->SetWindowText(m_strPath);

}


void ExportDlg::OnBnClickedButtonExport()
{
	// TODO: Add your control notification handler code here
	CString name, path, startFrame, endFrame;
	GetDlgItem(IDC_EDIT_SESSION_NAME)->GetWindowText(name);
	GetDlgItem(IDC_EDIT_FILEPATH)->GetWindowText(path);
	GetDlgItem(IDC_EDIT_STARTFRAME)->GetWindowText(startFrame);
	GetDlgItem(IDC_EDIT_ENDFRAME)->GetWindowText(endFrame);
	int sf = _ttoi(startFrame);
	int ef = _ttoi(endFrame);

	int curSel = ((CComboBox*)GetDlgItem(IDC_COMBO_EXPORT_TYPE))->GetCurSel();
	int curPersonSel = ((CComboBox*)GetDlgItem(IDC_COMBO_PERSON_INDEX))->GetCurSel();
	if (curPersonSel < 0)
	{
		curPersonSel = 0;
	}
	
	if (name != _T(""))
	{
		if (curSel == 0)
		{
			CString pathname = path + name + _T(".bvh");
			char chr[80];
			WideCharToMultiByte( CP_ACP, 0, pathname.GetBuffer(0), -1,  
				chr, sizeof(chr), NULL, NULL );
			BVH bv;
			bv.setCurFrameIndex(curPersonSel);
			bv.bvhWrite(chr, sf, ef, true);
			CDialog::OnOK();
		}
		else if (curSel == 1)
		{
			CString pathname = path + name + _T(".bvh");
			char chr[80];
			WideCharToMultiByte( CP_ACP, 0, pathname.GetBuffer(0), -1,  
				chr, sizeof(chr), NULL, NULL );
			BVH bv;
			bv.setCurFrameIndex(curPersonSel);
			bv.bvhWrite(chr, sf, ef, false);
			CDialog::OnOK();
		}
		else if (curSel == 2)
		{
			CString pathname = path + name + _T(".c3d");
			char chr[80];
			WideCharToMultiByte( CP_ACP, 0, pathname.GetBuffer(0), -1,  
				chr, sizeof(chr), NULL, NULL );
			C3D c3d_;
			c3d_.setCurFrameIndex(curPersonSel);
			c3d_.saveC3DFile(chr, sf, ef);
			CDialog::OnOK();
		}
		else if (curSel == 3)
		{
			CString pathname = path + name + _T(".fbx");
			char chr[80];
			WideCharToMultiByte( CP_ACP, 0, pathname.GetBuffer(0), -1,  
				chr, sizeof(chr), NULL, NULL );
			FBX fbx_;
			fbx_.setCurFrameIndex(curPersonSel);
			fbx_.fbxWrite(chr, sf, ef);
			CDialog::OnOK();
		}
		
	}
	
}


void ExportDlg::OnBnClickedButtonCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}
