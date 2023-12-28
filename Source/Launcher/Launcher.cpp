
// Launcher.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "Launcher.h"
#include "LauncherDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLauncherApp

BEGIN_MESSAGE_MAP(CLauncherApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CLauncherApp construction

CLauncherApp::CLauncherApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CLauncherApp object

CLauncherApp theApp;


// CLauncherApp initialization

BOOL CLauncherApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Motor City Online Launcher"));

	// Initialization
	{
		TCHAR szDirectory[MAX_PATH] = _T("");
		GetCurrentDirectory(MAX_PATH - 1, szDirectory);

		// Verify that the launcher is in the same directory as the game
		{
			CString appName;
			appName.Format(FILE_NAME_TEMPLATE, szDirectory, APP_FILE_NAME);

			CFileException ex;
			CFile app;
			if (!app.Open(appName.GetString(), CFile::modeRead, &ex))
			{
				CString error;
				error.Format(ERROR_MESSAGE_MISSING_APP_FILE, APP_FILE_NAME);

				AfxMessageBox(error.GetString(), MB_OK | MB_ICONERROR);

				return FALSE;
			}

			app.Close();
		}

		// Verify that Motor City Online has nofront.ini file
		{
			CString iniName;
			iniName.Format(FILE_NAME_TEMPLATE, szDirectory, INI_FILE_NAME);

			CFileException ex;
			CFile ini;
			if (!ini.Open(iniName.GetString(), CFile::modeRead, &ex))
			{
				CString error;
				error.Format(ERROR_MESSAGE_MISSING_CONFIG_FILE, INI_FILE_NAME);

				AfxMessageBox(error.GetString(), MB_OK | MB_ICONERROR);

				return FALSE;
			}

			ini.Close();
		}

		// Verify that Motor City Online has mcitynofrontskinslist.txt file
		{
			CString txtName;
			txtName.Format(FILE_NAME_TEMPLATE, szDirectory, TEXT_FILE_NAME);

			CFileException ex;
			CFile txt;
			if (!txt.Open(txtName.GetString(), CFile::modeRead, &ex))
			{
				CString error;
				error.Format(ERROR_MESSAGE_MISSING_CONFIG_FILE, TEXT_FILE_NAME);

				AfxMessageBox(error.GetString(), MB_OK | MB_ICONERROR);

				return FALSE;
			}

			txt.Close();
		}
	}

	// Init
	App::Init();

	CLauncherDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}