// TalkDlg.h : header file
//

#if !defined(AFX_TALKDLG_H__854B900F_3565_4694_8B0B_4BDB621CEB31__INCLUDED_)
#define AFX_TALKDLG_H__854B900F_3565_4694_8B0B_4BDB621CEB31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTalkDlg dialog
#include "..\ClientTalk\hardware\UsbDevice.h"
#include "..\ClientTalk\MCIPlayMusic\MCIPlayMusic.h"
#include "friendlist.h"
#include "ServiceInterface.h"
#include "IncommingDialog.h"
#include <afxdialogex.h>
#include <vector>
#include "stream\stream.h"
#include "ClientList.h"
enum CallStatus
{
	INITIAL    = 0, //初始状态     
	DIALING    = 1, //呼叫ing
	ACCEPTING     , //接听ing: 被叫方,来电铃声提醒,未接听,可以挂断(Reject),可以接听
	ONLINE        , //通话中,可以挂断(Hangup),或者等对方挂断(收到对方什么消息)
	BUSYING         //呼叫超时,忙音
};

struct _tagCallFrom
{
	CString ip;
	CWnd *pWnd;
	bool bAcceptCall;
};

struct CallingInfo
{
	char szClientIpAddress[64]; //IP地址
	char szClientName[512];     //机器名称
	char szBeginTime;           //通话开始时间 hh:mm:ss'ms
	int  nLastTime;             //通话持续时长
};

class CServiceTalkDlg : public CDialogEx
{
	// Construction
public:
	CServiceTalkDlg(CWnd* pParent = NULL);	// standard constructor
	~CServiceTalkDlg();
	CServiceInterface m_talk;

	// Dialog Data
	//{{AFX_DATA(CTalkDlg)
	enum { IDD = IDD_TALK_DIALOG };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTalkDlg)
public:
	virtual BOOL DestroyWindow();
	void KillWindowsManager();
	bool AcceptCallFrom(const char* ip);
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
	bool ReadHistoryCallInfo(vector<CallingInfo> &vecIinfo);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

	afx_msg LRESULT OnHandlePhone( WPARAM wParam, LPARAM lParam );
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnMenuitem32772();
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnMCINotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStopMusic(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void GetConfigInfo();
	void PlaySound(const CString &strSonndPath);
	CString GetMoudleConfigFilePath();
private:

	//主叫事件: 0:接听事件;1:挂断事件
	HANDLE m_hDialEvents[2];



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
	CIncommingDialog *m_pIncommingDlg;
	CFriendList m_listClient;
	//被叫事件: 0:接听事件;1:挂断事件
	HANDLE m_hAcceptCallEvents[2];
	//TODO:简单处理,1 客户端都是通过开关呼叫方,做呼叫相关的操作状态;
	//              2 前台服务端,被叫方,只能做接听电话操作,考虑前台呼叫客户端的情况--> 因为有界面,采取界面按钮方式,而非事件方式
	//记录当前状态: 主叫 还是 接听
	CallStatus m_callStatus;
	CString	m_name;
	//ip name time  list显示使用
	CString m_strCallingInfo;
private:
	//文件操作
	FileStream m_fs;
	//记录通话信息,新数据进行更新,最后一次性写入文件中
	vector<CallingInfo> m_vecInfo;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TALKDLG_H__854B900F_3565_4694_8B0B_4BDB621CEB31__INCLUDED_)
