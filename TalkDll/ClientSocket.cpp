// ClientSocket.cpp : implementation file
//

#include "stdafx.h"
#include "ClientSocket.h"
#include "head.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientSocket

CClientSocket::CClientSocket(CInterface *pInterFace,CMyWaveIn *pIn,CUdpSocket *pUdp):
	m_bConnect(FALSE),
	m_pInterface (NULL),
	m_pIn(NULL),
	m_pUdp (NULL)
{
	m_pBuffer = new char[1024];
	m_pInterface = pInterFace;
	m_pIn = pIn;
	m_pUdp = pUdp;
}

CClientSocket::~CClientSocket()
{
	delete [1024]m_pBuffer;
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CClientSocket, CSocket)
	//{{AFX_MSG_MAP(CClientSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CClientSocket member functions

void CClientSocket::OnClose(int nErrorCode)
{
	m_bConnect = FALSE;

	Close ();
	
	//服务端关掉客户端连接
	m_pInterface->BeClose ();
	
	CSocket::OnClose(nErrorCode);
}

void CClientSocket::OnReceive(int nErrorCode) 
{
	KillTimer(NULL, 111);
	// TODO: Add your specialized code here and/or call the base class
	struct TalkFrame *frame;
	frame = (struct TalkFrame *)m_pBuffer;
	
  //读取完整 TalkFrame 帧的数据
	int iLen = sizeof(struct TalkFrame);
	while(iLen > 0)
	{  
		int i = Receive (m_pBuffer + sizeof(struct TalkFrame) - iLen, iLen);
		if (i == SOCKET_ERROR )
			return ;
		iLen -= i;
	}

	if (strcmp(frame->cFlag ,"NETTALK") != 0)
	{
		//TODO: add 包标志不对,直接close连接
		//Send(m_pBuffer, sizeof(struct TalkFrame));
		OnClose(0);
		return;
	}

	std::string strIpName;
	//读数据体
	iLen = frame->iLen;
	while (iLen > 0)
	{
		int i = Receive (
			m_pBuffer + sizeof(struct TalkFrame) + (frame->iLen - iLen),
			iLen);
		if (i == SOCKET_ERROR )
			return ;
		iLen -= i;
	}

	CString strClientPeerAddress;
	UINT nClientPort;
	switch (frame->iCom )
	{
	case TC_NORMAL_TALK:
		//取到客户端信息,ip 名称, 服务端显示
		strIpName = ((struct TalkFrame *)m_pBuffer)->chClientInfo;
		m_pInterface->m_CallBackFun(MSG_ADDCLIENTUSER, const_cast<char *>(strIpName.c_str()));
		memset(frame,0,sizeof (struct TalkFrame));
		sprintf(frame->cFlag,"NETTALK");
		//获取对等连接的ip 和 端口
		GetPeerName (strClientPeerAddress, nClientPort);
		//TODO:这里应该是异步的,
		if (m_pInterface ->IncomingCall (strIpName.c_str()) )
		{
			//接听
			frame->iCom = TC_AGREE_TALK;
			frame->iLen = 0;
			if (SOCKET_ERROR  == Send (m_pBuffer,sizeof(struct TalkFrame)))
			{
				TRACE("Send agree command fail.\n");
				break ;
			};

			m_pInterface->TalkStart (strClientPeerAddress);
			m_pUdp->SetIp (strClientPeerAddress);
			m_pIn->EnableSend (TRUE);
			m_pInterface->m_bWork = TRUE;
			break;
		}
		else
		{
			//拒绝
			frame->iCom = TC_DISAGREE_TALK;
			frame->iLen = 0;
			Send (m_pBuffer,sizeof(struct TalkFrame));
			//主动断开,非法包
			OnClose(0);			
		}
		break;
	default:
		break;
	}

	CSocket::OnReceive(nErrorCode);
}

//TODO: 存在的问题,当telnet连过来后,也被当成连接了, 导致其他有效连接无法连接过来
//测试过程中遇到客户端怎么都连接不过来了
//测试网络断开 恢复 后 连接不上

//检查无效连接,将其删除 close