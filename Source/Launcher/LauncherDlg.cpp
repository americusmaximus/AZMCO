
// LauncherDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Launcher.h"
#include "LauncherDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace App;


// CLauncherDlg dialog



CLauncherDlg::CLauncherDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_LAUNCHER_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLauncherDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLauncherDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_GO, &CLauncherDlg::OnBnClicked1001)
    ON_BN_CLICKED(1020, &CLauncherDlg::OnBnClicked1020)
    ON_BN_CLICKED(1021, &CLauncherDlg::OnBnClicked1021)
END_MESSAGE_MAP()


// CLauncherDlg message handlers

BOOL CLauncherDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    // TODO: Add extra initialization here

    // Player -> Car

    // Player -> Skin

    // Player -> Mode

    // Opponents -> Same as Player

    // Opponents -> Car

    // Opponents -> Skin

    // Opponents -> Opponents

    {
        CComboBox* opponents = (CComboBox*)GetDlgItem(IDC_COMBOBOX_OPPONENT_COUNT);

        for (uint32_t x = 0; x < 6; x++)
        {
            CString value;
            value.Format(_T("%d"), x);

            opponents->AddString(value.GetString());
        }

        opponents->SetCurSel(AppState.Opponents.Count);
    }

    // Opponents -> Difficulty
    {
        CComboBox* difficulty = (CComboBox*)GetDlgItem(IDC_COMBOBOX_OPPONENT_DIFFICULTY);

        for (uint32_t x = 0; x < AppState.Difficulties->GetCount(); x++)
        {
            difficulty->AddString(AppState.Difficulties->GetAt(x).GetString());
        }

        difficulty->SetCurSel(AppState.Opponents.Difficulty);
    }

    // Track -> Track
    {
        CComboBox* track = (CComboBox*)GetDlgItem(IDC_COMBOBOX_TRACK_TRACK);

        for (uint32_t x = 0; x < AppState.Tracks->GetCount(); x++)
        {
            track->AddString(AppState.Tracks->GetAt(x).GetString());
        }

        // TODO track->SetCurSel();
    }

    // Track -> Laps

    // Track -> Night

    // Track -> Weather

    // Track -> No Damage

    // Track -> Mirrored

    // Track -> Backward

    // Track -> Traffic

    // Save Settings

    // Go!
    {
        CButton* go = (CButton*)GetDlgItem(IDC_BUTTON_GO);

        if (AppState.Tracks->GetCount() != 0)
        {
            go->EnableWindow(TRUE);
        }
    }

    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLauncherDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLauncherDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

// IDC_BUTTON_SAVE
void CLauncherDlg::OnBnClicked1020()
{
    // TODO: Add your control notification handler code here
}

// IDC_BUTTON_GO
void CLauncherDlg::OnBnClicked1001()
{
    TCHAR szDirectory[MAX_PATH] = _T("");
    GetCurrentDirectory(MAX_PATH - 1, szDirectory);
    
    CString appName;
    appName.Format(FILE_NAME_TEMPLATE, szDirectory, APP_FILE_NAME);

    intptr_t result = _wspawnl(_P_WAIT, appName, APP_FILE_NAME, _T("-nofront"), _T("-nomovie"), _T("-noturtle"), NULL);

    if (result != 0)
    {
        CString error;
        error.Format(_T("Process failed with return code: %d. Error #%d"), (int)result, GetLastError());

        AfxMessageBox(error.GetString(), MB_OK | MB_ICONERROR);
    }
}

// IDC_BUTTON_EXIT
void CLauncherDlg::OnBnClicked1021()
{
    CLauncherDlg::OnCancel();
}