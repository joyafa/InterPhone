#pragma once
#include "..\TalkDll\Interface.h"

class CServiceInterface :
	public CInterface
{
public:
	CServiceInterface(void);
	virtual ~CServiceInterface(void);

	virtual BOOL IncomingCall(CString ip);

};

