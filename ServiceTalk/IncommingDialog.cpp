// IncommingDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ServiceTalk.h"
#include "IncommingDialog.h"
#include "afxdialogex.h"
#include "ServiceTalkDlg.h"

// CIncommingDialog �Ի���

IMPLEMENT_DYNAMIC(CIncommingDialog, CDialogEx)

CIncommingDialog::CIncommingDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CIncommingDialog::IDD, pParent)
	, m_pServiceDlg (NULL)
{
	m_pServiceDlg = dynamic_cast<CServiceTalkDlg *>(pParent);
}

CIncommingDialog::~CIncommingDialog()
{
}

void CIncommingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CIncommingDialog, CDialogEx)
	ON_BN_CLICKED(IDACCEPT, &CIncommingDialog::OnBnClickedAccept)
	ON_BN_CLICKED(IDREJECT, &CIncommingDialog::OnBnClickedReject)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CIncommingDialog ��Ϣ�������


void CIncommingDialog::OnBnClickedAccept()
{
	if (NULL != m_pServiceDlg)
	{
		SetEvent(m_pServiceDlg->m_hAcceptCallEvents[0]);
	}
}


void CIncommingDialog::OnBnClickedReject()
{
	if (NULL != m_pServiceDlg)
	{
		SetEvent(m_pServiceDlg->m_hAcceptCallEvents[0]);
	}
}


BOOL CIncommingDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == WM_KEYDOWN)
	{
		if (VK_ESCAPE == pMsg->lParam)
		{
			return TRUE;
		}
	}
	
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CIncommingDialog::OnTimer(UINT_PTR nIDEvent)
{
	

	CDialogEx::OnTimer(nIDEvent);
}
