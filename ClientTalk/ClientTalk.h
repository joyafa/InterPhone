// ClientTalk.h : main header file for the ClientTalk application
//

#if !defined(AFX_ClientTalk_H__66B6F248_F7CE_4B2B_94BD_A46C85907633__INCLUDED_)
#define AFX_ClientTalk_H__66B6F248_F7CE_4B2B_94BD_A46C85907633__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CClientTalkApp:
// See ClientTalk.cpp for the implementation of this class
//

class CClientTalkApp : public CWinApp
{
public:
	CClientTalkApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientTalkApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CClientTalkApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ClientTalk_H__66B6F248_F7CE_4B2B_94BD_A46C85907633__INCLUDED_)
