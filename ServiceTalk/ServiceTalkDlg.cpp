// TalkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ServiceTalk.h"
#include "ServiceTalkDlg.h"
#include "PWDLG.h"
#include <mmsystem.h>
#include <afxpriv.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTalkDlg dialog

CServiceTalkDlg::CServiceTalkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServiceTalkDlg::IDD, pParent)
, m_callStatus(INITIAL)
, m_pUsbDevice(NULL)
{
	//{{AFX_DATA_INIT(CTalkDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServiceTalkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTalkDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

#define WM_STOPMUSIC WM_USER + 2


BEGIN_MESSAGE_MAP(CServiceTalkDlg, CDialog)
	//{{AFX_MSG_MAP(CTalkDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_MENUITEM32772, OnMenuitem32772)
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
ON_MESSAGE(WM_PHONE,     &CServiceTalkDlg::OnHandlePhone)
ON_MESSAGE(MM_MCINOTIFY, &CServiceTalkDlg::OnMCINotify)
ON_MESSAGE(WM_STOPMUSIC, &CServiceTalkDlg::OnStopMusic)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTalkDlg message handlers

BOOL CServiceTalkDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	//初始化硬件	
	GetConfigInfo();

	//VID:0x258A, PID:0x001B
	m_pUsbDevice = new CUsbDevice(m_dwVID, m_dwPID);
	//初始化硬件	
	m_pUsbDevice->SetOwner(m_hWnd);
	m_pUsbDevice->ConnectDevice();
	m_pUsbDevice->StartMonitor();

	// TODO: Add extra initialization here
	m_talk.Ini();
	SetTimer(990,100,NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CServiceTalkDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

LRESULT CServiceTalkDlg::OnMCINotify(WPARAM wParam, LPARAM lParam)  
{  
	if (wParam == MCI_NOTIFY_SUCCESSFUL )  
	{  
		//呼叫
		if(DIALING == m_callStatus)
		{
			PlaySound(m_strPathDialingBell);
		}
		//接听
		else if ( ACCEPTING == m_callStatus)
		{
			PlaySound(m_strPathIncommingBell);
		}
		//忙音
		else
		{
			PlaySound(m_strPathBusyBell);
		}
	}  
	return 0;  
}  
LRESULT CServiceTalkDlg::OnStopMusic( WPARAM wParam, LPARAM lParam )
{
	m_mciMusic.stop();

	return 1;
}
LRESULT CServiceTalkDlg::OnHandlePhone( WPARAM wParam, LPARAM lParam )
{
	HardwareEventType event = (HardwareEventType)wParam;

	switch (event)
	{
		//接听
	case LEFT_KEY:
		//呼叫模式, 因为收到来电,状态会变成ACCEPTING
		if (INITIAL == m_callStatus)
		{
			//OnBnClickedBtnCall();
		}
		break;
		//挂断
	case RIGHT_KEY:
		//主叫方通话中
		if (ONLINE == m_callStatus )
		{
			//挂断 hangup
			m_talk.End();
		}
		else if ( DIALING == m_callStatus )//拨号响铃...
		{
			//取消拨号了  TODO: 取消拨号是怎么弄的???
			//SetEvent();
			//m_pCallDialog->ShowWindow(SW_HIDE);
			m_callStatus = INITIAL;
		}
		else if (ACCEPTING == m_callStatus)//来电响铃...
		{
			//reject
			SetEvent(m_hAcceptCallEvents[1]);
			//m_pCallCommingDialog->ShowWindow(SW_HIDE);
			m_callStatus = INITIAL;
		}
		break;
	}

	return 1;
}

CString CServiceTalkDlg::GetMoudleConfigFilePath()
{
	CString strFileName;
	AfxGetModuleFileName(NULL, strFileName);
	int nPos = strFileName.Delete(strFileName.GetLength() - 3, 3);
	strFileName.Append("ini");

	return strFileName;
}

void CServiceTalkDlg::GetConfigInfo()
{
	CString strConfigFilePath = GetMoudleConfigFilePath();

	//硬件PID,VID
	m_dwPID = GetPrivateProfileInt("对讲机", "PID", 0x258A, strConfigFilePath);
	m_dwVID = GetPrivateProfileInt("对讲机", "VID", 0x001B, strConfigFilePath);
	char chBuffer[128] = {0};
	GetPrivateProfileString("对讲机", "ServerIP", "", chBuffer, sizeof(chBuffer), strConfigFilePath);
	m_strServiceIP = chBuffer;

	//拨号铃声
	memset(chBuffer, 0, sizeof(chBuffer));
	GetPrivateProfileString("对讲机", "拨号铃声", "", chBuffer, sizeof(chBuffer), strConfigFilePath);
	m_strPathDialingBell = chBuffer;
	//来电铃声
	memset(chBuffer, 0, sizeof(chBuffer));
	GetPrivateProfileString("对讲机", "来电铃声", "", chBuffer, sizeof(chBuffer), strConfigFilePath);
	m_strPathIncommingBell = chBuffer;
	//忙音
	memset(chBuffer, 0, sizeof(chBuffer));
	GetPrivateProfileString("对讲机", "忙音铃声", "", chBuffer, sizeof(chBuffer), strConfigFilePath);
	m_strPathBusyBell = chBuffer;
}


void CServiceTalkDlg::PlaySound(const CString &strSonndPath)
{
	//先停掉原有的声音
	m_mciMusic.stop();

	DWORD dwResult = m_mciMusic.play(this, strSonndPath );  
	if (dwResult != 0)  
	{  
		//beatLog_Error(("CServiceTalkDlg", __FUNCDNAME__, "Play sound failed: %s", m_mciMusic.getErrorMsg(dwResult)));
	}  
}
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CServiceTalkDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CServiceTalkDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CServiceTalkDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch ( wParam )
	{

		case IDOK :	
		case IDCANCEL :	return FALSE; break ;
	

	}		
	return CDialog::OnCommand(wParam, lParam);
}

void CServiceTalkDlg::OnMenuitem32772() 
{
	// TODO: Add your command handler code here
	/*
	if (::MessageBoxEx(this->m_hWnd, "确认退出?", "语音对讲机", 
		MB_OKCANCEL | MB_ICONINFORMATION, 0) == IDCANCEL)
	{
		return;
	}
	*/
	try
	{
		PWDLG dlg;
		if(dlg.DoModal()== IDCANCEL)
		{
			return;
		}
		m_talk.End();
		Sleep(1000);
	}
	catch(...)
	{
		return;
	}
//	UnregisterHotKey(m_hWnd,1);
	CDialog::OnClose();
	CDialog::DestroyWindow();	
}

void CServiceTalkDlg::OnClose() 
{
	
	// TODO: Add your message handler code here and/or call default
	OnMenuitem32772() ;	
}

void CServiceTalkDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
//	this->ShowWindow(SW_HIDE);
}

void CServiceTalkDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	this->ShowWindow(SW_HIDE);
	KillTimer(990);
	CDialog::OnTimer(nIDEvent);
}

BOOL CServiceTalkDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::DestroyWindow();
}
