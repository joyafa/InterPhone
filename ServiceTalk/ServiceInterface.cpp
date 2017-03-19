#include "stdafx.h"
#include "ServiceInterface.h"


CServiceInterface::CServiceInterface(void)
{
}


CServiceInterface::~CServiceInterface(void)
{
}

BOOL CServiceInterface::IncomingCall(CString ip)
{
	int nRet = 0;
	if(m_CallBackFun!=0)
	{
		nRet = m_CallBackFun(MSG_CallIn, (char *)ip.GetString());
	}

	return nRet;
}
