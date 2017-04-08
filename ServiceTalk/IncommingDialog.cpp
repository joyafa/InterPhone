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
	DDX_Control(pDX, IDACCEPT, m_btnAccept);
	DDX_Control(pDX, IDREJECT, m_btnReject);
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
	GetDlgItem(IDREJECT)->ShowWindow(SW_NORMAL);

	CRect rect;
	GetWindowRect(&rect);
	ScreenToClient(rect);

	CRect rectRejcetButton;
	GetDlgItem(IDREJECT)->GetWindowRect(&rectRejcetButton);
	ScreenToClient(&rectRejcetButton);
	int x = (rect.Width() - rectRejcetButton.Width() )  / 2;
	GetDlgItem(IDREJECT)->MoveWindow(x, rectRejcetButton.top, rectRejcetButton.Width(), rectRejcetButton.Height());
	
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
	KillTimer(TIME_EVENT);
	if (NULL != m_pServiceDlg)
	{
		if (m_pServiceDlg->m_callStatus == ACCEPTING)
		{
			SetEvent(m_pServiceDlg->m_hAcceptCallEvents[1]);
		}
		m_pServiceDlg->m_talk.End();		
	}
	CString strCallingTime;
	UpdataWindow(strCallingTime);
}

void CIncommingDialog::UpdataWindow(CString &strCallingTime)
{
	GetDlgItem(IDACCEPT)->ShowWindow(SW_NORMAL);
	GetDlgItem(IDREJECT)->ShowWindow(SW_NORMAL);
	KillTimer(TIME_EVENT);
	CRect rect;
	GetWindowRect(&rect);
	ScreenToClient(rect);

	CRect rectAcceptButton;
	GetDlgItem(IDACCEPT)->GetWindowRect(&rectAcceptButton);
	ScreenToClient(&rectAcceptButton);

	CRect rectRejectButton;
	GetDlgItem(IDREJECT)->GetWindowRect(&rectRejectButton);

	int x = rectAcceptButton.right + (rect.Width() - rectAcceptButton.left - rectAcceptButton.Width() - rectRejectButton.Width()) / 2;
	GetDlgItem(IDREJECT)->MoveWindow(x, rectAcceptButton.top, rectRejectButton.Width(), rectRejectButton.Height());

	Invalidate();
	GetDlgItemText(IDC_STATIC_TALKTIME, strCallingTime);
	ShowWindow(SW_HIDE);
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


BOOL CIncommingDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	short	shBtnColor = 30;
	//accept
	CImage imageAccept;
	imageAccept.Load(".\\res\\accept.png");
	HBITMAP hBitmap = imageAccept.Detach();
	m_btnAccept.SetBitmaps(hBitmap, RGB(255, 100, 100));
	m_btnAccept.SetTooltipText(_T("接听来电"));
	m_btnAccept.SetFlat();
	m_btnAccept.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shBtnColor);
	/*m_btnClose.SetIcon(IDI_NO3, (int)BTNST_AUTO_GRAY);
	m_btnClose.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shBtnColor);
	m_btnClose.SetTooltipText(_T("Close the application"));*/

	//reject & hangup
	CImage imageReject;
	imageReject.Load(".\\res\\reject.png");
	hBitmap = imageReject.Detach();
	m_btnReject.SetBitmaps(hBitmap, RGB(255, 100, 100));
	m_btnReject.SetTooltipText(_T("挂断电话"));
	m_btnReject.SetFlat();
	m_btnReject.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shBtnColor);
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
