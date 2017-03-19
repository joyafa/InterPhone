// TalkCallDlg.h : header file
//

#if !defined(AFX_TALKCALLDLG_H__3BB882D4_255E_4002_A514_25896E860019__INCLUDED_)
#define AFX_TALKCALLDLG_H__3BB882D4_255E_4002_A514_25896E860019__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "..\TalkDll\Interface.h"
#include ".\hardware\UsbDevice.h"
#include ".\MCIPlayMusic\MCIPlayMusic.h"

enum CallStatus
{
	INITIAL    = 0, //初始状态     
	DIALING    = 1, //呼叫ing
	ACCEPTING     , //接听ing: 被叫方,来电铃声提醒,未接听,可以挂断(Reject),可以接听
	ONLINE        , //通话中,可以挂断(Hangup),或者等对方挂断(收到对方什么消息)
	BUSYING         //呼叫超时,忙音
};

/////////////////////////////////////////////////////////////////////////////
// CTalkCallDlg dialog

class CClientTalkDlg : public CDialog
{
// Construction
public:
	CClientTalkDlg(CWnd* pParent = NULL);	// standard constructor
	~CClientTalkDlg()
	{
		if (m_pUsbDevice)
		{
			delete m_pUsbDevice;
		}
	}
	 int MSG(int type,char *p);
	 //呼叫类型,TRUE:呼叫;FALSE:呼入;
	bool m_bCall;
	CString	m_name;
// Dialog Data
	//{{AFX_DATA(CTalkCallDlg)
	enum { IDD = IDD_TALKCALL_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTalkCallDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTalkCallDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg LRESULT OnHandlePhone( WPARAM wParam, LPARAM lParam );
	void PlaySound(const CString &strSonndPath);
	CString GetMoudleConfigFilePath();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnMCINotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStopMusic(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CInterface m_talk;

	//结束现有的通话
	void EndCall() ;

	void GetConfigInfo();
private:
	CUsbDevice *m_pUsbDevice;
	//硬件PID,VID
	DWORD m_dwPID;
	DWORD m_dwVID;
	CString m_strServiceIP;
	//拨号铃声
	CString m_strPathDialingBell;
	//来电铃声
	CString m_strPathIncommingBell;
	//忙音
	CString m_strPathBusyBell;

	CMCIPlayMusic m_mciMusic;

public:
	afx_msg void OnBnClickedBtnCall();

	//主叫事件: 0:接听事件;1:挂断事件
	HANDLE m_hDialEvents[2];


	//TODO:简单处理,1 客户端都是通过开关呼叫方,做呼叫相关的操作状态;
	//              2 前台服务端,被叫方,只能做接听电话操作,考虑前台呼叫客户端的情况--> 因为有界面,采取界面按钮方式,而非事件方式
	//记录当前状态: 主叫 还是 接听
	CallStatus m_callStatus;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TALKCALLDLG_H__3BB882D4_255E_4002_A514_25896E860019__INCLUDED_)
