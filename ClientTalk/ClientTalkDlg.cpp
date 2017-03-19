// TalkCallDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClientTalk.h"
#include "ClientTalkDlg.h"
#include "configdeal.h"
#include <mmsystem.h>
#include <afxpriv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CClientTalkApp theApp;
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
// CTalkCallDlg dialog

CClientTalkDlg::CClientTalkDlg(CWnd* pParent /*=NULL*/)
: CDialog(CClientTalkDlg::IDD, pParent)
, m_callStatus(INITIAL)
, m_pUsbDevice(NULL)
{
	//{{AFX_DATA_INIT(CTalkCallDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bCall=false;
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
	
		int cxScreen = ::GetSystemMetrics(SM_CXSCREEN);//�����Ļ��
		GetWindowRect(&rc);
		
		if (GetSafeHwnd())
		{
			MoveWindow(cxScreen-rc.right,50,rc.right,rc.bottom);
		}
		
	}
	//*/
	::SetWindowPos(m_hWnd,  HWND_TOPMOST ,
		0, 0, 50,50, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	GetConfigInfo();

	//TODO: to be delete �����¼�
	m_hDialEvents[0] = CreateEventA(NULL, TRUE, FALSE, NULL);//�����¼�
	m_hDialEvents[1] = CreateEventA(NULL, TRUE, FALSE, NULL);//�����Ҷ��¼�

	//VID:0x258A, PID:0x001B
	m_pUsbDevice = new CUsbDevice(m_dwVID, m_dwPID);
	//��ʼ��Ӳ��	
	m_pUsbDevice->SetOwner(m_hWnd);
	m_pUsbDevice->ConnectDevice();
	m_pUsbDevice->StartMonitor();
	
	// TODO: Add extra initialization here
	int CallBack(int type, char *p);
	m_talk.Ini(CallBack);

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
		//����
		if(DIALING == m_callStatus)
		{
			PlaySound(m_strPathDialingBell);
		}
		//����
		else if ( ACCEPTING == m_callStatus)
		{
			PlaySound(m_strPathIncommingBell);
		}
		//æ��
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
		//����
	case LEFT_KEY:
		//����ģʽ, ��Ϊ�յ�����,״̬����ACCEPTING
		if (INITIAL == m_callStatus)
		{
			OnBnClickedBtnCall();
		}
		break;
		//�Ҷ�
	case RIGHT_KEY:
		//���з�ͨ����
		if (ONLINE == m_callStatus )
		{
			//�Ҷ� hangup
			m_callStatus = INITIAL;
			EndCall();
		}
		else if ( DIALING == m_callStatus )//��������...
		{
			SetEvent(m_hDialEvents[1]);
			EndCall();
			m_callStatus = INITIAL;
		}
		
		break;
	default:
		TRACE("%s,EVENT: %d ��Ч����,�ݲ�����!!!\n", __FUNCTION__, event);
		break;
	}

	return 1;
}


void CClientTalkDlg::PlaySound(const CString &strSonndPath)
{
	//��ͣ��ԭ�е�����
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

	//Ӳ��PID,VID
	m_dwPID = GetPrivateProfileInt("�Խ���", "PID", 0x001B, strConfigFilePath);
	m_dwVID = GetPrivateProfileInt("�Խ���", "VID", 0x258A, strConfigFilePath);
	char chBuffer[128] = {0};
	GetPrivateProfileString("�Խ���", "ServerIP", "", chBuffer, sizeof(chBuffer), strConfigFilePath);
	m_strServiceIP = chBuffer;

	//��������
	memset(chBuffer, 0, sizeof(chBuffer));
	GetPrivateProfileString("�Խ���", "��������", "", chBuffer, sizeof(chBuffer), strConfigFilePath);
	m_strPathDialingBell = chBuffer;
	//��������
	memset(chBuffer, 0, sizeof(chBuffer));
	GetPrivateProfileString("�Խ���", "��������", "", chBuffer, sizeof(chBuffer), strConfigFilePath);
	m_strPathIncommingBell = chBuffer;
	//æ��
	memset(chBuffer, 0, sizeof(chBuffer));
	GetPrivateProfileString("�Խ���", "æ������", "", chBuffer, sizeof(chBuffer), strConfigFilePath);
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
		//TODO: ������ܶ����еĶ�Ҫ����, ����Ҫ�ж�����ͨ����
		//if(m_bCall)
			m_talk.End();
		m_bCall=false;
		SetWindowText("�������л�");
		KillTimer(900);
		Sleep(100);
	}
	catch(...)
	{

		SetWindowText("�������л�");
	}
}



int CallBack(int type, char *p)
{
	CClientTalkDlg *pMainDlg = (CClientTalkDlg *)theApp.m_pMainWnd;

	switch(type)
	{
	case MSG_CallOut :
		//MessageBox(spClientTalkDlg->m_hWnd, p, "����", MB_ICONINFORMATION);
		break;
	case MSG_CallIn  :
		MessageBox(pMainDlg->m_hWnd, p, "����", MB_ICONINFORMATION);
		break;
		break;
	case MSG_CallOk  :		
		{
			SetEvent(pMainDlg->m_hDialEvents[0]);

			CString str="��ǰ̨ͨ����...";
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
		pMainDlg->m_callStatus = INITIAL;
		pMainDlg->m_bCall=false;
		pMainDlg->SetWindowText("�������л�");
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

UINT __stdcall DialFunc(void *pvoid)
{
	//_tagJidFrom *pFrom = (_tagJidFrom *)pvoid;
	CClientTalkDlg *pDlag = (CClientTalkDlg *)pvoid;
	//if (NULL== pDlag) return 0;

	//����60s�Է�����������Ϊ��ʱ���ر�����
	//TODO:�Ƿ�Ҫæ��
	//TODO���Ƿ���Ҫͨ�������ļ����ó�ʱʱ��
	DWORD dwRet = WaitForMultipleObjects(2, pDlag->m_hDialEvents, FALSE, 60 * 1000);
	if (dwRet ==  WAIT_OBJECT_0)//�����¼�
	{
		//ֱ��ֹͣ����,��Ҫ������Ϣ
		PostMessage(pDlag->m_hWnd, WM_STOPMUSIC, NULL, NULL);
		pDlag->m_callStatus = ONLINE;
		//����Ϊ���ź�״̬
		//���� SetEvent�������ź�֮��,��Ҫ����reset����Ϊ���ź�
		ResetEvent(pDlag->m_hDialEvents[0]);
	}
	else  //�Ҷ��¼�,��ʱֹͣ �� �쳣 ��ֹͣ
	{
		PostMessage(pDlag->m_hWnd, WM_STOPMUSIC, NULL, NULL);
		//����Ϊ���ź�
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
		EndCall();
		Sleep(5000);		
		
		////////////
		if(m_talk.Start(m_strServiceIP.GetString())==1)
		{
			//����״̬
			m_callStatus = DIALING;
			//״̬1: ����,�������йҶ�;
			//״̬2: ���г�ʱ������,��������æ��

			HANDLE hPlayMusic = (HANDLE)_beginthreadex(NULL, 0, DialFunc, this, 0, 0);			
			PlaySound(m_strPathDialingBell);//��绰
			MessageLoop(&hPlayMusic, 1);	
			CloseHandle(hPlayMusic);
			
			/*bAcceptCall = pFrom->bAcceptCall;
			delete pFrom;*/
			if (ONLINE == m_callStatus)
			{
				m_bCall=true;
			    m_name="��������ǰ̨...";
			    SetWindowText(m_name);
			}
			else
			{
				//����ȡ�����Ż������Ѿ��ҵ�
			}
			////m_name="����ǰ̨";
			//TODO: ?? Ϊʲô
			//SetTimer(900,5000,NULL);
		}
		else
		{
			SetWindowText("�������л�");
		}
	}
	catch(...)
	{

		SetWindowText("�������л�");
	}
}
