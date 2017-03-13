// TalkDlg.h : header file
//

#if !defined(AFX_TALKDLG_H__854B900F_3565_4694_8B0B_4BDB621CEB31__INCLUDED_)
#define AFX_TALKDLG_H__854B900F_3565_4694_8B0B_4BDB621CEB31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTalkDlg dialog
#include "..\TalkDll\Interface.h"
class CServiceTalkDlg : public CDialog
{
// Construction
public:
	CServiceTalkDlg(CWnd* pParent = NULL);	// standard constructor
CInterface m_talk;

// Dialog Data
	//{{AFX_DATA(CTalkDlg)
	enum { IDD = IDD_TALK_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTalkDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTalkDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnMenuitem32772();
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TALKDLG_H__854B900F_3565_4694_8B0B_4BDB621CEB31__INCLUDED_)
