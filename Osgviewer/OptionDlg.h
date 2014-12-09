#pragma once


// OptionDlg dialog

class OptionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(OptionDlg)

public:
	OptionDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~OptionDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_OPTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonOptionBrowse();
};
