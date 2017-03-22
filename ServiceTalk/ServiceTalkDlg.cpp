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
, m_pIncommingDlg(NULL)
{
	//{{AFX_DATA_INIT(CTalkDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CServiceTalkDlg::~CServiceTalkDlg()
{
	if ( NULL != m_pUsbDevice)
	{
		delete m_pUsbDevice;
	}

	if (NULL != m_pIncommingDlg)
	{
		delete m_pIncommingDlg;
	}
}

void CServiceTalkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTalkDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_LIST_CLIENT, m_listClient);
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

	//被叫事件
	m_hAcceptCallEvents[0] = CreateEventA(NULL, TRUE, FALSE, NULL);//接听事件
	m_hAcceptCallEvents[1] = CreateEventA(NULL, TRUE, FALSE, NULL);//挂断事件

	m_pIncommingDlg = new CIncommingDialog(this);
	m_pIncommingDlg->Create(IDD_DIALOG_INCOMING, this);
	m_pIncommingDlg->ShowWindow(SW_HIDE);

	//VID:0x258A, PID:0x001B
	m_pUsbDevice = new CUsbDevice(m_dwVID, m_dwPID);
	//初始化硬件	
	m_pUsbDevice->SetOwner(m_hWnd);
	m_pUsbDevice->ConnectDevice();
	m_pUsbDevice->StartMonitor();

	// TODO: Add extra initialization here
	int ServiceCallBack(int type, char *p);
	m_talk.Ini(ServiceCallBack);
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


//TODO:接听过程中,不会有该线程的存在,也就不会有等待事件的情况,只要通过当前的状态,检查当前出于什么状态;
// 1. 通话中,则发送挂断指令;hangup
// 2. 响铃中,这reject;

UINT __stdcall AcceptCallFunc(void *pvoid)
{
	_tagCallFrom *pFrom = (_tagCallFrom *)pvoid;
	CServiceTalkDlg *pDlag = (CServiceTalkDlg *)pFrom->pWnd;
	if (NULL== pDlag) return 0;

	//电话60s不接听，认为超时，关闭铃声
	//TODO：是否需要通过配置文件设置
	DWORD dwRet = WaitForMultipleObjects(2, pDlag->m_hAcceptCallEvents, FALSE, 60 * 1000);
	if (dwRet ==  WAIT_OBJECT_0 )//接听事件
	{
		//直接停止不行,需要发送消息
		PostMessage(pDlag->m_hWnd, WM_STOPMUSIC, NULL, NULL);
		//重置为无信号状态
		//调用 SetEvent设置有信号之后,需要调用reset设置为无信号
		ResetEvent(pDlag->m_hAcceptCallEvents[0]);

		pDlag->m_callStatus = ONLINE;
		pFrom->bAcceptCall = true;
	}
	else  //挂断事件,超时停止 或 异常 都停止
	{
		PostMessage(pDlag->m_hWnd, WM_STOPMUSIC, NULL, NULL);
		//1. 超时或者是拒绝进接听
		pFrom->bAcceptCall = false;
		//设置为无信号
		ResetEvent(pDlag->m_hAcceptCallEvents[1]);
		pDlag->m_callStatus = INITIAL;
	}
	
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

//192.168.1.1;china;
bool SplitIpAndName(const string &strIpName, string &ip, string &name)
{
	size_t pos = strIpName.find_first_of(';');
	if (pos != std::string::npos)
	{
		ip    = strIpName.substr(0, pos);
		name  = strIpName.substr(pos + 1);
	}

	return true;
}

bool CServiceTalkDlg::AcceptCallFrom(const char* pIpAndName)
{
	//接听状态
	m_callStatus = ACCEPTING;

	bool bAcceptCall(false);

	string strIp, strName;
    if (pIpAndName)
	{
		SplitIpAndName(pIpAndName, strIp, strName);
	}

	if (m_pIncommingDlg)
	{
		m_pIncommingDlg->SetDlgItemText(IDC_STATIC_IPADDRESS, strIp.c_str());
		m_pIncommingDlg->SetDlgItemText(IDC_STATIC_COMPUTERNAME, strName.c_str());
		m_pIncommingDlg->ShowWindow(SW_SHOW);
		m_pIncommingDlg->CenterWindow();
	}
	//创建播放声音线程
	//TODO:显示提示来电提醒对话框
	_tagCallFrom *pFrom = new _tagCallFrom;
	pFrom->ip   = pIpAndName;
	pFrom->pWnd = this;
	pFrom->bAcceptCall = false;

	//状态1: 新来电，可以 接听 or 挂断
	//状态2: 通话中,可以挂断;
	HANDLE hPlayMusic = (HANDLE)_beginthreadex(NULL, 0, AcceptCallFunc, pFrom, 0, 0);
	PlaySound(m_strPathIncommingBell);
	//简单处理,因为是永远不超时的等待,所有必须有消息或退出消息才会返回
	MessageLoop(&hPlayMusic, 1);	
	CloseHandle(hPlayMusic);
	//提示框 showWindow(SW_HIDE);
	bAcceptCall = pFrom->bAcceptCall;
	delete pFrom;

	return bAcceptCall;
}


extern CServiceTalkApp theApp;

int ServiceCallBack(int type, char *p)
{
	CServiceTalkDlg *pMainDlg = dynamic_cast<CServiceTalkDlg *>(theApp.m_pMainWnd);
	if (NULL == pMainDlg)
	{
		return -1;
	}
	switch(type)
	{
	case MSG_ADDCLIENTUSER :
		//名字;IP
		pMainDlg->m_listClient.AddNewUser(p);
		break;
	case MSG_CallIn:
		//来电,同步等待完成
		return pMainDlg->AcceptCallFrom(p);
	case MSG_CallOk  :
		{
			CString str="对讲中--";
			str=str+pMainDlg->m_name;
			pMainDlg->SetWindowText(str);
			pMainDlg->KillTimer(900);
		}
		break;
	case MSG_CallClose :
		//
		//	SetWindowText("语音呼叫机");
		break;
	}

	return 0;
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
		else if (ACCEPTING == m_callStatus)
		{
			SetEvent(m_hAcceptCallEvents[0]);
		}
		break;
		//挂断
	case RIGHT_KEY:
		//主叫
		if (ONLINE == m_callStatus)
		{
			//挂断 hangup
			m_talk.End();
			m_callStatus = INITIAL;
		}
		else if (ACCEPTING == m_callStatus)//来电响铃...
		{
			//reject
			SetEvent(m_hAcceptCallEvents[1]);
			//m_pCallCommingDialog->ShowWindow(SW_HIDE);
			m_talk.End();
			m_callStatus = INITIAL;
		}
		m_pIncommingDlg->ShowWindow(SW_HIDE);
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
	//this->ShowWindow(SW_HIDE);
	KillTimer(990);
	CDialog::OnTimer(nIDEvent);
}

BOOL CServiceTalkDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::DestroyWindow();
}
