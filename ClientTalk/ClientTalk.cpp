// ClientTalk.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ClientTalk.h"
#include "ClientTalkDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientTalkApp

BEGIN_MESSAGE_MAP(CClientTalkApp, CWinApp)
	//{{AFX_MSG_MAP(CClientTalkApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientTalkApp construction

CClientTalkApp::CClientTalkApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CClientTalkApp object

CClientTalkApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CClientTalkApp initialization

BOOL CClientTalkApp::InitInstance()
{
	//确保只有一个实例
	HANDLE hMutex=CreateMutex(0,FALSE,"NETTALK_MUTEX_ONCE1");
	if(hMutex)
	{
		if(GetLastError()==ERROR_ALREADY_EXISTS)
			return 0;
	}

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CClientTalkDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
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

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

BOOL CClientTalkApp::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->wParam == WM_KEYDOWN)
	{
		if (VK_ESCAPE == pMsg->lParam)
		{
			return TRUE;
		}
	}

	return CWinApp::PreTranslateMessage(pMsg);
}
