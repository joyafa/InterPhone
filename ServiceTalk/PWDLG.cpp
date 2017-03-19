// PWDLG.cpp : implementation file
//

#include "stdafx.h"
#include "ServiceTalk.h"
#include "PWDLG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PWDLG dialog


PWDLG::PWDLG(CWnd* pParent /*=NULL*/)
	: CDialog(PWDLG::IDD, pParent)
{
	//{{AFX_DATA_INIT(PWDLG)
	m_pw = _T("");
	//}}AFX_DATA_INIT
}


void PWDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PWDLG)
	DDX_Text(pDX, IDC_EDIT1, m_pw);
	DDV_MaxChars(pDX, m_pw, 20);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PWDLG, CDialog)
	//{{AFX_MSG_MAP(PWDLG)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PWDLG message handlers

void PWDLG::OnOK() 
{
	// TODO: Add extra validation here
	this->UpdateData(TRUE);
#if 0
	if(strcmp(m_pw,"123!@#")!=0)
	{
		m_pw="";
		this->UpdateData(FALSE);
		return;
	}
#endif
	CDialog::OnOK();
}
