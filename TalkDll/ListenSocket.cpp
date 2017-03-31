// ListenSocket.cpp : implementation file
//

#include "stdafx.h"
#include "ListenSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListenSocket

CListenSocket::CListenSocket(CInterface *pTemp,CMyWaveIn *pIn,CUdpSocket *pUdp):
	m_sopClient (NULL),
	m_pIn(NULL),
	m_pUdp(NULL)
{
	m_sopClient = new CClientSocket(pTemp,pIn,pUdp);
	m_pInterface = pTemp;
	m_pIn = pIn;
	m_pUdp = pUdp;
}

CListenSocket::~CListenSocket()
{
	delete m_sopClient;
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CListenSocket, CSocket)
	//{{AFX_MSG_MAP(CListenSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CListenSocket member functions

void CListenSocket::OnAccept(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	SOCKADDR add;
	memset(&add, 0, sizeof(add));
	add.sa_family = AF_INET;
	//TODO: add��Ҫ���,���쳣
	int iLen;
	iLen = sizeof(SOCKADDR);
	CSocket soTemp;
	if (m_sopClient->m_bConnect )
	{
		TRACE("Someone connect but i have talk with someon.\n");
		//TODO: �����Ѿ�����,����������ͨ��sotemp������ʾ��Ϣ���ͻ���
		Accept (soTemp);
		soTemp.Close ();
		return ;
	}
	//TODO: �� m_sopClient   �������,��Ҫ�����,����ʱ�䲻����Ϣ,������ ���Ĵ����,Ҫǿ�йص�,��telnet����֮��,���������ľ��޷����ӹ�����.
	if (!Accept(*m_sopClient,&add,&iLen))
	{
		TRACE("Accetp error.\n");
		return ;
	}

	m_sopClient->m_bConnect = TRUE;

	CSocket::OnAccept(nErrorCode);
}

