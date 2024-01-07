/*
Copyright (c) 2023 - 2024 Americus Maximus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
    ON_CBN_SELCHANGE(IDC_COMBOBOX_PLAYER_CAR, &CLauncherDlg::OnCbnSelchange1002)
    ON_CBN_SELCHANGE(IDC_COMBOBOX_PLAYER_SKIN, &CLauncherDlg::OnCbnSelchange1003)
    ON_BN_CLICKED(IDC_CHECK_OPPONENT_SAME_AS_PLAYER, &CLauncherDlg::OnBnClicked1006)
    ON_CBN_SELCHANGE(IDC_COMBOBOX_OPPONENT_CAR, &CLauncherDlg::OnCbnSelchange1007)
    ON_BN_CLICKED(IDC_BUTTON_SAVE, &CLauncherDlg::OnBnClicked1020)
    ON_BN_CLICKED(IDC_BUTTON_EXIT, &CLauncherDlg::OnBnClicked1021)
END_MESSAGE_MAP()


// CLauncherDlg message handlers

BOOL CLauncherDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    // Player -> Car
    {
        int indx = -1;

        CComboBox* cars = (CComboBox*)GetDlgItem(IDC_COMBOBOX_PLAYER_CAR);

        for (uint32_t x = 0; x < AppState.Cars->GetCount(); x++)
        {
            Car* car = AppState.Cars->GetAt(x);

            CString value;
            value.Format(CAR_LIST_ITEM_TEMAPLE, car->Name->GetString(), car->ID);

            cars->AddString(value.GetString());

            if (car->ID == AppState.Player.Car) { indx = x; }
        }

        cars->SetCurSel(indx == -1 ? 0 : indx);

        OnCbnSelchange1002();
    }

    // Player -> Mode

    // Opponents -> Same as Player
    const BOOL same = AppState.Player.Car == AppState.Opponents.Car && AppState.Player.Skin == AppState.Opponents.Skin;

    {
        ((CButton*)GetDlgItem(IDC_CHECK_OPPONENT_SAME_AS_PLAYER))->SetCheck(same ? BST_CHECKED : BST_UNCHECKED);

        ((CComboBox*)GetDlgItem(IDC_COMBOBOX_OPPONENT_CAR))->EnableWindow(!same);
        ((CComboBox*)GetDlgItem(IDC_COMBOBOX_OPPONENT_SKIN))->EnableWindow(!same);
    }

    // Opponents -> Car
    {
        int indx = -1;

        CComboBox* cars = (CComboBox*)GetDlgItem(IDC_COMBOBOX_OPPONENT_CAR);

        for (uint32_t x = 0; x < AppState.Cars->GetCount(); x++)
        {
            Car* car = AppState.Cars->GetAt(x);

            CString value;
            value.Format(CAR_LIST_ITEM_TEMAPLE, car->Name->GetString(), car->ID);

            cars->AddString(value.GetString());

            if (car->ID == AppState.Opponents.Car) { indx = x; }
        }

        cars->SetCurSel(indx == -1 ? 0 : indx);

        OnCbnSelchange1007();
    }

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

// IDC_COMBOBOX_PLAYER_CAR
void CLauncherDlg::OnCbnSelchange1002()
{
    const int indx = ((CComboBox*)GetDlgItem(IDC_COMBOBOX_PLAYER_CAR))->GetCurSel();

    // Player -> Skin
    {
        CComboBox* skins = (CComboBox*)GetDlgItem(IDC_COMBOBOX_PLAYER_SKIN);

        skins->ResetContent();

        if (indx < 0) { return; }

        CArray<Skin*, Skin*>* carSkins = AppState.Cars->GetAt(indx)->Skins;

        for (uint32_t x = 0; x < carSkins->GetCount(); x++)
        {
            Skin* carSkin = carSkins->GetAt(x);

            skins->AddString(carSkin->Name->GetString());
        }

        skins->SetCurSel(0);
    }

    // Opponent -> Car
    {
        CButton* check = (CButton*)GetDlgItem(IDC_CHECK_OPPONENT_SAME_AS_PLAYER);

        if (check->GetCheck() == BST_CHECKED)
        {
            CComboBox* cars = (CComboBox*)GetDlgItem(IDC_COMBOBOX_OPPONENT_CAR);

            cars->SetCurSel(indx);

            OnCbnSelchange1007();
        }
    }
}

// IDC_COMBOBOX_PLAYER_SKIN
void CLauncherDlg::OnCbnSelchange1003()
{
    if (((CButton*)GetDlgItem(IDC_CHECK_OPPONENT_SAME_AS_PLAYER))->GetCheck() == BST_CHECKED)
    {
        CComboBox* skins = (CComboBox*)GetDlgItem(IDC_COMBOBOX_OPPONENT_SKIN);

        skins->SetCurSel(((CComboBox*)GetDlgItem(IDC_COMBOBOX_PLAYER_SKIN))->GetCurSel());
    }
}

// IDC_CHECK_OPPONENT_SAME_AS_PLAYER
void CLauncherDlg::OnBnClicked1006()
{
    const BOOL checked = ((CButton*)GetDlgItem(IDC_CHECK_OPPONENT_SAME_AS_PLAYER))->GetCheck() == BST_CHECKED;

    CComboBox* cars = (CComboBox*)GetDlgItem(IDC_COMBOBOX_OPPONENT_CAR);
    CComboBox* skins = (CComboBox*)GetDlgItem(IDC_COMBOBOX_OPPONENT_SKIN);

    // Opponents -> Car
    if (checked)
    {
        cars->SetCurSel(((CComboBox*)GetDlgItem(IDC_COMBOBOX_PLAYER_CAR))->GetCurSel());
    }
    else
    {
        int indx = -1;

        for (uint32_t x = 0; x < AppState.Cars->GetCount(); x++)
        {
            if (AppState.Cars->GetAt(x)->ID == AppState.Opponents.Car) { indx = x; break; }
        }

        cars->SetCurSel(indx == -1 ? 0 : indx);
    }

    OnCbnSelchange1007();

    cars->EnableWindow(!checked);
    skins->EnableWindow(!checked);
}

// IDC_COMBOBOX_OPPONENT_CAR
void CLauncherDlg::OnCbnSelchange1007()
{
    const int indx = ((CComboBox*)GetDlgItem(IDC_COMBOBOX_OPPONENT_CAR))->GetCurSel();

    // Opponents -> Skin
    {
        CComboBox* skins = (CComboBox*)GetDlgItem(IDC_COMBOBOX_OPPONENT_SKIN);

        skins->ResetContent();

        if (indx < 0) { return; }

        int skn = -1;

        CArray<Skin*, Skin*>* carSkins = AppState.Cars->GetAt(indx)->Skins;

        for (uint32_t x = 0; x < carSkins->GetCount(); x++)
        {
            Skin* carSkin = carSkins->GetAt(x);

            skins->AddString(carSkin->Name->GetString());

            if (carSkin->ID == AppState.Opponents.Skin) { skn = x; }
        }

        const BOOL checked = ((CButton*)GetDlgItem(IDC_CHECK_OPPONENT_SAME_AS_PLAYER))->GetCheck() == BST_CHECKED;

        skins->SetCurSel(checked
            ? ((CComboBox*)GetDlgItem(IDC_COMBOBOX_PLAYER_SKIN))->GetCurSel()
            : (skn == -1 ? 0 : skn));
    }
}