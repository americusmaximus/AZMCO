
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
	afx_msg virtual void OnOK();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClicked1001();
	afx_msg void OnCbnSelchange1002();
	afx_msg void OnCbnSelchange1003();
	afx_msg void OnEnChange1004();
	afx_msg void OnDeltapos1005(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClicked1006();
	afx_msg void OnCbnSelchange1007();
	afx_msg void OnCbnSelchange1008();
	afx_msg void OnCbnSelchange1009();
	afx_msg void OnCbnSelchange1010();
	afx_msg void OnCbnSelchange1011();
	afx_msg void OnEnChange1012();
	afx_msg void OnDeltapos1013(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClicked1014();
	afx_msg void OnBnClicked1015();
	afx_msg void OnBnClicked1016();
	afx_msg void OnBnClicked1017();
	afx_msg void OnBnClicked1018();
	afx_msg void OnBnClicked1019();
	afx_msg void OnBnClicked1020();
	afx_msg void OnBnClicked1021();
};
