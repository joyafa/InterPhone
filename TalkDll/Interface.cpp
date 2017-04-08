// Interface.cpp: implementation of the CInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Interface.h"
#include "head.h"
#include "MyWaveIn.h"	// Added by ClassView
#include "WaveOut.h"	// Added by ClassView
#include "ListenSocket.h"	// Added by ClassView
#include "SendClient.h"
#include "UdpSocket.h"
#include "RecSocket.h"
#include <afxmt.h>
#include "Log.hpp"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

LOG_INIT;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWaveOut *g_pOut;
CCriticalSection g_soLock;

CInterface::CInterface():
m_bIni (FALSE)
{	
	m_bWork = FALSE;
	m_pUdp = new CUdpSocket();
	m_pRec = new CRecSocket();
	
	g_pOut = new CWaveOut();
	m_pIn = new CMyWaveIn(m_pUdp);

	m_sopListen = new CListenSocket(this,m_pIn,m_pUdp);
	
	//	m_sopSend = new CSendClient(m_pIn,this);
	m_sopSend=0;
	m_CallBackFun=0;
}

CInterface::~CInterface()
{
	if (m_bIni)
	{
		m_sopListen->Close ();
	}
	if (m_bWork)
	{
		End();
	}
	m_pIn->StopRec ();
	m_pUdp->CloseSocket ();
	
	m_pRec->CloseSocket ();
	g_pOut->StopPlay ();
	
	//	delete m_sopSend;
	delete m_sopListen;
	delete m_pIn;
	delete g_pOut;
	delete m_pUdp;
	delete m_pRec;
}

CString CInterface::GetHostIpName()
{
	char chName[128] = {0};
	if (SOCKET_ERROR  == gethostname(chName, sizeof(chName)))
	{
		LOG_ERR << "Get host name failed!";
		return "";
	}
	struct hostent *phost;
	phost = gethostbyname (chName);
	if (phost == NULL)
	{
		TRACE("gethostbyname error .\n");
		LOG_ERR << "gethostbyname error!";
		return "";
	}

	CString loip;
	int i = 0;
	while (phost->h_addr_list[i])
	{
		loip = inet_ntoa (*(struct in_addr *)phost->h_addr_list[i++]);
		break;
	}
	//name\nip 机器名\nIP
	return CString("机器名:") + chName + "\n" + "IP地址:" + loip + "\n";
}

string CInterface::GetLocalIpAddress()
{
	char hostName[128] = {0};
	if (SOCKET_ERROR == gethostname(hostName, sizeof(hostName)))
	{
		return "";
	}

	struct hostent *phost;
	phost = gethostbyname(hostName);
	if (NULL == phost)
	{
		return "";
	}

	int i = 0;
	CString strIpAddress;
	while (phost->h_addr_list[i])
	{
		strIpAddress = inet_ntoa(*(struct in_addr *)phost->h_addr_list[i++]);
		break;
	}

	return strIpAddress;
}

BOOL CInterface::Start(const char *ip)
{
	BOOL bRet  = FALSE;
	try
	{
		char name[128];
		int iLen = 128;
		int i = 0;
		CString loip;

		do
		{
			if (m_bWork)
			{
				TRACE("The talk has worked.\n");
				break;
			}
			m_sIp = ip;

			if (SOCKET_ERROR  == gethostname(name,iLen))
			{
				TRACE("I can't get the name.\n");
				break;
			}
			struct hostent *phost;
			phost = gethostbyname (name);
			if (phost == NULL)
			{
				TRACE("gethostbyname error .\n");
				break;
			}

			i = 0;
			while (phost->h_addr_list[i])
			{
				loip = inet_ntoa (*(struct in_addr *)phost->h_addr_list[i++]);
				if (loip == ip)
					break;
			}

			if(NULL == m_sopSend)
			{
				m_sopSend = new CSendClient(m_pIn,this);
				bRet      = m_sopSend->Create ();
				if (!bRet)
				{
					break;;
				}
			}
			else
			{
				break;
			}

			//客户端TCP方式连接服务器
			bool nRet = m_sopSend->Connect (ip,TALK_COM_PORT);
			if (!nRet)
			{
				DWORD dwErr = GetLastError();
			}
			if (GetLastError() != WSAEWOULDBLOCK)
			{
				m_sopSend->Close ();
				break;
			}
			m_pUdp->SetIp (ip);

			m_bWork = TRUE;
			break;
		}while(false);
	}
	catch(...)
	{
		TRACE("Start failed!!!");
	}

	return bRet;
}


BOOL CInterface::End()
{
	try
	{
		//TODO: 这里可能对所有的都要结束, 不需要判断正在通话中
		//if (!m_bWork)
		//{
			//服务端主动关掉client,拒绝接听时候操作
		if (m_sopListen)
		{
			m_sopListen->CloseClient();
		}

		//	TRACE("The talk hasn't worked.\n");
		//	return FALSE;
		//}

		if (m_pIn)
		{
			m_pIn->EnableSend (FALSE);
		}

		if(m_sopSend)
		{
			m_sopSend->Close ();
			m_sopSend->m_bConnect = FALSE;
		}

		m_bWork = FALSE;
		
		if(m_sopSend)
		{
			delete m_sopSend;
			m_sopSend = NULL;
		}

		return TRUE;
	}
	catch(...)
	{

	}

	return FALSE;
}

BOOL CInterface::Ini(_CallBackFun __CallBackFun)
{
	try
	{
		m_CallBackFun=__CallBackFun;
		if (m_bIni)
		{
			TRACE("You have in the talk model.\n");
			return FALSE;
		}
		//创建tcp连接
		if (!m_sopListen->Create (TALK_COM_PORT))
		{
			TRACE("Error:%d \n", WSAGetLastError());
			goto Exit;
		}
		int timeout = 1000;
		setsockopt(m_sopListen->m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

		//设置Socket的选项, 解决10048错误必须的步骤,关闭scoket后端口便能正常释放
		BOOL bOptVal = TRUE;
		int bOptLen = sizeof(BOOL);
		m_sopListen->SetSockOpt(SO_REUSEADDR, (void *)&bOptVal, bOptLen, SOL_SOCKET);

		//监听
		m_sopListen->Listen ();
		
		//UDP 广播形式
		if (!m_pRec->Ini ())
		{
			goto Exit1;
		}

		//UDP连接
		if (!m_pUdp->Ini ())
		{
			goto Exit2;
		}
		//开始播放
		if (!g_pOut->StartPlay ())
		{
			goto Exit3;
		};

		//开始录音
		if (!m_pIn->StartRec ())
		{
			goto Exit4;
		};
		
		m_bIni = TRUE;

		goto Exit;
Exit4:
		g_pOut->StopPlay ();
Exit3:
		m_pUdp->CloseSocket ();
Exit2:
		m_pRec->CloseSocket ();
Exit1:
		m_sopListen->Close ();
Exit:
		return m_bIni;
	}
	catch(...)
	{
	}
	return FALSE;
}

BOOL CInterface::IncomingCall(CString ip)
{
	if(m_CallBackFun!=0)
	{
		m_CallBackFun(MSG_CallIn,"来电...");
	}
	//是否接听
	if (IDYES ==  MessageBox(NULL,"talk?","talk",MB_YESNO))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	
	return TRUE;
}

void CInterface::TalkStart(CString ip)
{
	if(m_CallBackFun!=0)
	{
		m_CallBackFun(MSG_CallOk,"对话");
	}
}

void CInterface::TalkBeClose()
{
	if(m_CallBackFun!=0)
	{
		m_CallBackFun(MSG_CallClose,"关闭");
	}
}

void CInterface::BeClose()
{
	try
	{
		g_soLock.Lock ();
		//if (m_bWork)
		{
			m_pIn->EnableSend (FALSE);
			if(NULL != m_sopSend)
			{
				m_sopSend->Close ();
				m_sopSend->m_bConnect = FALSE;
				m_sopSend = NULL;
			}

			m_bWork = FALSE;
			
			TalkBeClose();
		}
		g_soLock.Unlock ();
	}
	catch(...)
	{
	}
}

//连接服务器成功,发送信息,开始请求通话
void CInterface::ConnectResult(int nErrorCode)
{
	try
	{
		//连接服务器失败
		if (nErrorCode != 0)
		{
			m_bWork = FALSE;
			TalkOnConnect (FALSE);

			return ;
		}

		TalkOnConnect (TRUE);
		
		//发送客户端信息
		//TODO: 加上开始通话时间 以及 挂断时候 通话时长
		//TODO:list表中显示的图标用  电脑的 那个 图标 
		char buffer[240];
		memset(buffer,0,240);
		struct TalkFrame *frame;
		frame = (struct TalkFrame *)buffer;
		sprintf(frame->cFlag,"NETTALK");
		sprintf(frame->chClientInfo, GetHostIpName());
		frame->iLen = 0;
		frame->iCom = TC_NORMAL_TALK;
		//发送客户端信息 
		if(NULL != m_sopSend)
		{
			m_sopSend->Send (buffer,sizeof(struct TalkFrame));
		}
	}
	catch(...)
	{
	}
}

void CInterface::TalkOnConnect(BOOL bRet)
{
	//TODO: bRet false时候 连接失败,挂断处理

	if(m_CallBackFun!=0)
	{
		if (bRet)
		{
			m_CallBackFun(MSG_CallOut, "正在呼叫前台...");
		}
		else 
		{

			m_CallBackFun(MSG_CallClose, "呼叫中断");
		}
	}
}