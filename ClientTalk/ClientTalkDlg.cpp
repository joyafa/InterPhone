// TalkCallDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClientTalk.h"
#include "ClientTalkDlg.h"
#include "configdeal.h"
#include <mmsystem.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
CClientTalkDlg * CClientTalkDlg::pTalkCallDlg=0;
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
// CTalkCallDlg dialog

CClientTalkDlg::CClientTalkDlg(CWnd* pParent /*=NULL*/)
: CDialog(CClientTalkDlg::IDD, pParent)
, m_usbDevice(0x258A, 0x001B)
, m_callStatus(INITIAL)
{
	//{{AFX_DATA_INIT(CTalkCallDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	pTalkCallDlg=this;
	ISCall=false;
}

void CClientTalkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTalkCallDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}
#define WM_STOPMUSIC WM_USER + 2
BEGIN_MESSAGE_MAP(CClientTalkDlg, CDialog)
//{{AFX_MSG_MAP(CTalkCallDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_WM_TIMER()

//}}AFX_MSG_MAP
ON_MESSAGE(WM_PHONE,     &CClientTalkDlg::OnHandlePhone)
ON_MESSAGE(MM_MCINOTIFY, &CClientTalkDlg::OnMCINotify)
ON_MESSAGE(WM_STOPMUSIC, &CClientTalkDlg::OnStopMusic)

ON_BN_CLICKED(IDC_BTN_CALL, &CClientTalkDlg::OnBnClickedBtnCall)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTalkCallDlg message handlers

BOOL CClientTalkDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
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
	
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	
	if (GetSafeHwnd())
	{
		tagRECT rc;
	
		int cxScreen = ::GetSystemMetrics(SM_CXSCREEN);//获得屏幕宽
		GetWindowRect(&rc);
		
		if (GetSafeHwnd())
		{
			MoveWindow(cxScreen-rc.right,50,rc.right,rc.bottom);
		}
		
	}
	//*/
	::SetWindowPos(m_hWnd,  HWND_TOPMOST ,
		0, 0, 50,50, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	//初始化硬件	
	m_usbDevice.SetOwner(m_hWnd);
	m_usbDevice.ConnectDevice();
	m_usbDevice.StartMonitor();

	// TODO: Add extra initialization here
	m_talk.Ini(MSG);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CClientTalkDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

LRESULT CClientTalkDlg::OnMCINotify(WPARAM wParam, LPARAM lParam)  
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



LRESULT CClientTalkDlg::OnStopMusic( WPARAM wParam, LPARAM lParam )
{
	m_mciMusic.stop();

	return 1;
}



LRESULT CClientTalkDlg::OnHandlePhone( WPARAM wParam, LPARAM lParam )
{
	HardwareEventType event = (HardwareEventType)wParam;

	switch (event)
	{
		//呼叫
	case LEFT_KEY:
		//呼叫模式, 因为收到来电,状态会变成ACCEPTING
		if (INITIAL == m_callStatus)
		{
			OnBnClickedBtnCall();
		}
		break;
		//挂断
	case RIGHT_KEY:
		//主叫方通话中
		if (ONLINE == m_callStatus )
		{
			//挂断 hangup
			EndCall();
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


void CLibJingleDlg::PlaySound(const std::string &strSonndPath)
{
	//先停掉原有的声音
	m_mciMusic.stop();

	DWORD dwResult = m_mciMusic.play(this, CString(strSonndPath.c_str()) );  
	if (dwResult != 0)  
	{  
		//beatLog_Error(("CLibJingleDlg", __FUNCDNAME__, "Play sound failed: %s", m_mciMusic.getErrorMsg(dwResult)));
	}  
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClientTalkDlg::OnPaint() 
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
HCURSOR CClientTalkDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}



void CClientTalkDlg::EndCall() 
{
	try
	{
		if(ISCall)
			m_talk.End();
		ISCall=false;
		SetWindowText("语音对讲呼叫机");
		KillTimer(900);
		Sleep(100);
	}
	catch(...)
	{

		SetWindowText("语音对讲呼叫机");
	}
}

int CClientTalkDlg::MSG(int type, char *p)
{
	if(pTalkCallDlg==0)return 0;
	switch(type)
	{
	case MSG_CallOut :
		break;
	case MSG_CallIn  :
		break;
	case MSG_CallOk  :
		{
			CString str="对讲中--";
			str=str+pTalkCallDlg->m_name;
			
			pTalkCallDlg->SetWindowText(str);
			pTalkCallDlg->KillTimer(900);
		}
		break;
	case MSG_CallClose :
		pTalkCallDlg->ISCall=false;
		pTalkCallDlg->SetWindowText("语音对讲呼叫机");
		break;
	}
	
	return 0;
}

LRESULT CClientTalkDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	return CDialog::DefWindowProc(message, wParam, lParam);
}

BOOL CClientTalkDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch ( wParam )
	{
		
	case IDOK :	return FALSE; break ;
	case IDCANCEL :	return FALSE; break ;
	}		
	return CDialog::OnCommand(wParam, lParam);
}

void CClientTalkDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	EndCall();
	CDialog::OnTimer(nIDEvent);
}


void CClientTalkDlg::OnBnClickedBtnCall()
{
	try
	{
		EndCall();
		Sleep(500);
		/////////////
		Cconfigdeal		cfgdeal;
		char _IP[24];
		if (cfgdeal.setfilename("ip.ini") != 0)
			return ;
		if (cfgdeal.readcfgdata("IP", "1L", _IP, sizeof(_IP)) != 0)
			return;
		////////////
		m_ip=_IP;
		if(m_talk.Start(m_ip)==1)
		{
			//主叫状态
			m_callStatus = DIALING;
			//状态1: 呼叫,可以自行挂断;
			//状态2: 呼叫超时不接听,本地听到忙音
			HANDLE hPlayMusic = (HANDLE)_beginthreadex(NULL, 0, DialFunc, pFrom, 0, 0);
			CloseHandle(hPlayMusic);
			PlaySound(m_strPathDialingBell);//打电话

			ISCall=true;
			m_name="连接中--1连";
			SetWindowText(m_name);
			m_name="1连";
			SetTimer(900,5000,NULL);
			//		((CButton *)(this->GetDlgItem(IDC_BUTTON1)))->SetState(TRUE);
		}
		else
		{
			SetWindowText("语音对讲呼叫机");
		}
	}
	catch(...)
	{

		SetWindowText("语音对讲呼叫机");
	}
}
