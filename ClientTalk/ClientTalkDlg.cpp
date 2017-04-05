// ClientTalkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClientTalk.h"
#include "ClientTalkDlg.h"
#include "configdeal.h"
#include <mmsystem.h>
#include <afxpriv.h>
#include <atlimage.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CClientTalkApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CClientTalkDlg dialog

CClientTalkDlg::CClientTalkDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CClientTalkDlg::IDD, pParent)
, m_callStatus(INITIAL)
, m_pUsbDevice(NULL)
{
	//{{AFX_DATA_INIT(CClientTalkDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bCall=false;
}

void CClientTalkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClientTalkDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_BTN_CALL, m_btnDial);
	DDX_Control(pDX, IDC_BTN_HANGUP, m_btnHangup);
}
#define WM_STOPMUSIC WM_USER + 2
BEGIN_MESSAGE_MAP(CClientTalkDlg, CDialogEx)
//{{AFX_MSG_MAP(CClientTalkDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_WM_TIMER()

//}}AFX_MSG_MAP
ON_MESSAGE(WM_PHONE,     &CClientTalkDlg::OnHandlePhone)
ON_MESSAGE(MM_MCINOTIFY, &CClientTalkDlg::OnMCINotify)
ON_MESSAGE(WM_STOPMUSIC, &CClientTalkDlg::OnStopMusic)

ON_BN_CLICKED(IDC_BTN_CALL, &CClientTalkDlg::OnBnClickedBtnCall)
ON_BN_CLICKED(IDC_BTN_HANGUP, &CClientTalkDlg::OnBnClickedBtnHangup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientTalkDlg message handlers

BOOL CClientTalkDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
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
	/*::SetWindowPos(m_hWnd,  HWND_TOPMOST ,
		0, 0, 50,50, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);*/
	CenterWindow();

	GetConfigInfo();

	//TODO: to be delete 主叫事件
	m_hDialEvents[0] = CreateEventA(NULL, TRUE, FALSE, NULL);//主叫事件
	m_hDialEvents[1] = CreateEventA(NULL, TRUE, FALSE, NULL);//主动挂断事件

	//VID:0x258A, PID:0x001B
	m_pUsbDevice = new CUsbDevice(m_dwVID, m_dwPID);
	//初始化硬件	
	m_pUsbDevice->SetOwner(m_hWnd);
	m_pUsbDevice->ConnectDevice();
	m_pUsbDevice->StartMonitor();
	
	// TODO: Add extra initialization here
	int CallBack(int type, char *p);
	m_talk.Ini(CallBack);

	//image button
	short	shBtnColor = 30;
	//accept
	CImage imageDial;
	imageDial.Load(".\\res\\dial.png");
	HBITMAP hBitmap = imageDial.Detach();
	m_btnDial.SetBitmaps(hBitmap, RGB(255, 100, 100));
	m_btnDial.SetTooltipText(_T("接听来电"));
	m_btnDial.SetFlat();
	m_btnDial.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shBtnColor);

	//reject & hangup
	CImage imageHangup;
	imageHangup.Load(".\\res\\hangup.png");
	hBitmap = imageHangup.Detach();
	m_btnHangup.SetBitmaps(hBitmap, RGB(255, 100, 100));
	m_btnHangup.SetTooltipText(_T("挂断电话"));
	m_btnHangup.SetFlat();
	m_btnHangup.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shBtnColor);
	
	m_btnDial.ShowWindow(SW_NORMAL);
	m_btnHangup.ShowWindow(SW_HIDE);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CClientTalkDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
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
    TRACE("%s,EVENT: %d\n", __FUNCTION__, event);
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
		OnBnClickedBtnHangup();		
		break;
	default:
		TRACE("%s,EVENT: %d 无效按键,暂不处理!!!\n", __FUNCTION__, event);
		break;
	}

	return 1;
}


void CClientTalkDlg::PlaySound(const CString &strSonndPath)
{
	//先停掉原有的声音
	m_mciMusic.stop();

	DWORD dwResult = m_mciMusic.play(this, strSonndPath );  
	if (dwResult != 0)  
	{  
		//beatLog_Error(("CLibJingleDlg", __FUNCDNAME__, "Play sound failed: %s", m_mciMusic.getErrorMsg(dwResult)));
	}  
}

CString CClientTalkDlg::GetMoudleConfigFilePath()
{
	CString strFileName;
	AfxGetModuleFileName(NULL, strFileName);
	int nPos = strFileName.Delete(strFileName.GetLength() - 3, 3);
	strFileName.Append("ini");

	return strFileName;
}

void CClientTalkDlg::GetConfigInfo()
{
	CString strConfigFilePath = GetMoudleConfigFilePath();

	//硬件PID,VID
	m_dwPID = GetPrivateProfileInt("对讲机", "PID", 0x001B, strConfigFilePath);
	m_dwVID = GetPrivateProfileInt("对讲机", "VID", 0x258A, strConfigFilePath);
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
		CDialogEx::OnPaint();
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
	/*if ( ONLINE != m_callStatus &&
		 DIALING != m_callStatus )
	{
		return;
	}*/

	try
	{
		/*if(m_bCall)
		{
			m_talk.End();
		}*/
		m_talk.End();
		m_bCall=false;
	}
	catch(...)
	{
	}

	SetWindowText("结束通话");
	m_callStatus = INITIAL;

	m_btnDial.ShowWindow(SW_NORMAL);
	m_btnHangup.ShowWindow(SW_HIDE);
}



int CallBack(int type, char *p)
{
	CClientTalkDlg *pMainDlg = (CClientTalkDlg *)theApp.m_pMainWnd;

	switch(type)
	{
	case MSG_CallOut :
		//MessageBox(spClientTalkDlg->m_hWnd, p, "呼出", MB_ICONINFORMATION);
		break;
	case MSG_CallIn  :
		MessageBox(pMainDlg->m_hWnd, p, "来电", MB_ICONINFORMATION);
		break;
		break;
	case MSG_CallOk  :		
		{
			SetEvent(pMainDlg->m_hDialEvents[0]);

			CString str="通话中...";
			str=str + pMainDlg->m_name;
			pMainDlg->SetWindowText(str);
			pMainDlg->KillTimer(900);
		}
		break;
	case MSG_CallClose :
		if (DIALING == pMainDlg->m_callStatus)
		{
			SetEvent(pMainDlg->m_hDialEvents[1]);	
		    pMainDlg->EndCall();
		}
		pMainDlg->m_btnDial.ShowWindow(SW_NORMAL);
		pMainDlg->m_btnHangup.ShowWindow(SW_HIDE);
		pMainDlg->m_callStatus = INITIAL;
		pMainDlg->m_bCall=false;
		pMainDlg->SetWindowText("通话关闭");
		break;
	}
	
	return 0;
}

LRESULT CClientTalkDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	return CDialogEx::DefWindowProc(message, wParam, lParam);
}

BOOL CClientTalkDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch ( wParam )
	{
		
	case IDOK :	return FALSE; break ;
	case IDCANCEL :	return FALSE; break ;
	}		
	return CDialogEx::OnCommand(wParam, lParam);
}



UINT __stdcall DialFunc(void *pvoid)
{
	//_tagJidFrom *pFrom = (_tagJidFrom *)pvoid;
	CClientTalkDlg *pDlag = (CClientTalkDlg *)pvoid;
	//if (NULL== pDlag) return 0;

	//呼叫60s对方不接听，认为超时，关闭铃声
	//TODO:是否要忙音
	//TODO：是否需要通过配置文件设置超时时间
	DWORD dwRet = WaitForMultipleObjects(2, pDlag->m_hDialEvents, FALSE, 60 * 1000);
	if (dwRet ==  WAIT_OBJECT_0)//接听事件
	{
		//直接停止不行,需要发送消息
		PostMessage(pDlag->m_hWnd, WM_STOPMUSIC, NULL, NULL);
		pDlag->m_callStatus = ONLINE;

		//重置为无信号状态
		//调用 SetEvent设置有信号之后,需要调用reset设置为无信号
		ResetEvent(pDlag->m_hDialEvents[0]);
	}
	else  //挂断事件,超时停止 或 异常 都停止
	{
		PostMessage(pDlag->m_hWnd, WM_STOPMUSIC, NULL, NULL);
		//设置为无信号
		ResetEvent(pDlag->m_hDialEvents[1]);
		pDlag->m_callStatus = INITIAL;
	}

	//delete pFrom;

	return 1;
}

int   MessageLoop(   
	HANDLE*   lphObjects,  //   handles   that   need   to   be   waited   on   
	int       cObjects     //   number   of   handles   to   wait   on   
	) 
{   
	//   The   message   loop   lasts   until   we   get   a   WM_QUIT   message, 
	//   upon   which   we   shall   return   from   the   function. 
	while   (TRUE) 
	{ 
		//   block-local   variable   
		DWORD   result   ;   
		MSG   msg;   
		//   Read   all   of   the   messages   in   this   next   loop,   
		//   removing   each   message   as   we   read   it. 
		while   (PeekMessage(&msg,   NULL,   0,   0,   PM_REMOVE))   
		{   
			//   If   it 's   a   quit   message,   we 're   out   of   here. 
			if   (msg.message   ==   WM_QUIT)     
				return   -1;   
			//   Otherwise,   dispatch   the   message. 
			DispatchMessage(&msg);   
		}   //   End   of   PeekMessage   while   loop. 
		//   Wait   for   any   message   sent   or   posted   to   this   queue   
		//   or   for   one   of   the   passed   handles   be   set   to   signaled. 
		result   =   MsgWaitForMultipleObjects(cObjects,   lphObjects,   
			FALSE,   INFINITE,   QS_ALLINPUT);   
		//   The   result   tells   us   the   type   of   event   we   have. 
		if   (result   ==   (WAIT_OBJECT_0   +   cObjects)) 
		{ 
			//   New   messages   have   arrived.   
			//   Continue   to   the   top   of   the   always   while   loop   to   
			//   dispatch   them   and   resume   waiting. 
			continue; 
		}   
		else   
		{   
			//   One   of   the   handles   became   signaled.   
			return result - WAIT_OBJECT_0; 
		}   //   End   of   else   clause. 
	}  

	return -1;
}   


void CClientTalkDlg::OnBnClickedBtnCall()
{
	try
	{
		//挂断电话
		EndCall();
		////////////
		if(m_talk.Start(m_strServiceIP.GetString())==1)
		{
			SetWindowText("呼叫中...");
			//主叫状态
			m_callStatus = DIALING;
			m_btnDial.ShowWindow(SW_HIDE);
			m_btnHangup.ShowWindow(SW_NORMAL);

			//状态1: 呼叫,可以自行挂断;
			//状态2: 呼叫超时不接听,本地听到忙音

			HANDLE hPlayMusic = (HANDLE)_beginthreadex(NULL, 0, DialFunc, this, 0, 0);			
			PlaySound(m_strPathDialingBell);//打电话
			MessageLoop(&hPlayMusic, 1);	
			CloseHandle(hPlayMusic);
			
			if (ONLINE == m_callStatus)
			{
				m_bCall=true;
			    SetWindowText("通话中...");
			}
			else
			{
				m_bCall=false;
				SetWindowText("拒绝接听!");				
			}
		}
		else
		{
			SetWindowText("网络忙呼叫失败!");
		}
	}
	catch(...)
	{
		SetWindowText("网络异常呼叫失败");
	}

	/*if (!m_bCall)
	{
		m_btnDial.ShowWindow(SW_NORMAL);
		m_btnHangup.ShowWindow(SW_HIDE);
	}*/
}


void CClientTalkDlg::OnBnClickedBtnHangup()
{
	if (DIALING == m_callStatus)
	{
		SetEvent(m_hDialEvents[1]);
	}

	EndCall();
}
