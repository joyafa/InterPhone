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
	
	//����˹ص��ͻ�������
	m_pInterface->BeClose ();
	
	CSocket::OnClose(nErrorCode);
}

void CClientSocket::OnReceive(int nErrorCode) 
{
	KillTimer(NULL, 111);
	// TODO: Add your specialized code here and/or call the base class
	struct TalkFrame *frame;
	frame = (struct TalkFrame *)m_pBuffer;
	
  //��ȡ���� TalkFrame ֡������
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
		//TODO: add ����־����,ֱ��close����
		//Send(m_pBuffer, sizeof(struct TalkFrame));
		OnClose(0);
		return;
	}

	std::string strIpName;
	//��������
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
		//ȡ���ͻ�����Ϣ,ip ����, �������ʾ
		strIpName = ((struct TalkFrame *)m_pBuffer)->chClientInfo;
		m_pInterface->m_CallBackFun(MSG_ADDCLIENTUSER, const_cast<char *>(strIpName.c_str()));
		memset(frame,0,sizeof (struct TalkFrame));
		sprintf(frame->cFlag,"NETTALK");
		//��ȡ�Ե����ӵ�ip �� �˿�
		GetPeerName (strClientPeerAddress, nClientPort);
		//TODO:����Ӧ�����첽��,
		if (m_pInterface ->IncomingCall (strIpName.c_str()) )
		{
			//����
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
			//�ܾ�
			frame->iCom = TC_DISAGREE_TALK;
			frame->iLen = 0;
			Send (m_pBuffer,sizeof(struct TalkFrame));
			//�����Ͽ�,�Ƿ���
			OnClose(0);			
		}
		break;
	default:
		break;
	}

	CSocket::OnReceive(nErrorCode);
}

//TODO: ���ڵ�����,��telnet��������,Ҳ������������, ����������Ч�����޷����ӹ���
//���Թ����������ͻ�����ô�����Ӳ�������
//��������Ͽ� �ָ� �� ���Ӳ���

//�����Ч����,����ɾ�� close