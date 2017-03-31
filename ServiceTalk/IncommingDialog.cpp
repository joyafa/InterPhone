// IncommingDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "ServiceTalk.h"
#include "IncommingDialog.h"
#include "afxdialogex.h"
#include "ServiceTalkDlg.h"

// CIncommingDialog 对话框

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


// CIncommingDialog 消息处理程序
#define TIME_EVENT 1000

void CIncommingDialog::OnBnClickedAccept()
{
	GetDlgItem(IDACCEPT)->ShowWindow(SW_HIDE);
	CRect rect;
	GetClientRect(&rect);

	CRect rectRejcetButton;
	GetDlgItem(IDREJECT)->GetClientRect(&rectRejcetButton);
	int x = (rect.Width() - rectRejcetButton.Width() )  / 2;
	GetDlgItem(IDREJECT)->GetWindowRect(&rect);
	//ClientToScreen(&rectRejcetButton);
	//TODO:top值不对
	//GetDlgItem(IDREJECT)->MoveWindow(x, rect.top, rect.Width(), rect.Height());
	Invalidate();
	SetTimer(TIME_EVENT, 1000, NULL);
	m_dwCounts = GetTickCount();
	if (NULL != m_pServiceDlg)
	{
		SetEvent(m_pServiceDlg->m_hAcceptCallEvents[0]);
	}
}


void CIncommingDialog::OnBnClickedReject()
{
	GetDlgItem(IDACCEPT)->ShowWindow(SW_NORMAL);
	//GetDlgItem(IDREJECT)->MoveWindow(x, rectRejcetButton.top, rectRejcetButton.Width(), rectRejcetButton.Height());

	if (NULL != m_pServiceDlg)
	{
		SetEvent(m_pServiceDlg->m_hAcceptCallEvents[1]);
		m_pServiceDlg->m_talk.End();
		ShowWindow(SW_HIDE);
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
	if (TIME_EVENT == nIDEvent)
	{
		DWORD dwTickCount = GetTickCount();
		DWORD dwEscape    = (dwTickCount - m_dwCounts) / 1000;
		CString strTime;
		strTime.Format("%02d:%02d:%02d", dwEscape / 3600, dwEscape % 3600 / 60, dwEscape % 60);
		SetDlgItemText(IDC_STATIC_TALKTIME, strTime);
	}

	CDialogEx::OnTimer(nIDEvent);
}
