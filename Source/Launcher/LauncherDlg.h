
// LauncherDlg.h : header file
//

#pragma once


// CLauncherDlg dialog
class CLauncherDlg : public CDialogEx
{
// Construction
public:
	CLauncherDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LAUNCHER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClicked1001();
	afx_msg void OnBnClicked1020();
	afx_msg void OnBnClicked1021();
};
