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
	INITIAL    = 0, //��ʼ״̬     
	DIALING    = 1, //����ing
	ACCEPTING     , //����ing: ���з�,������������,δ����,���ԹҶ�(Reject),���Խ���
	ONLINE        , //ͨ����,���ԹҶ�(Hangup),���ߵȶԷ��Ҷ�(�յ��Է�ʲô��Ϣ)
	BUSYING         //���г�ʱ,æ��
};

struct _tagCallFrom
{
	CString ip;
	CWnd *pWnd;
	bool bAcceptCall;
};

struct CallingInfo
{
	char szClientIpAddress[64]; //IP��ַ
	char szClientName[512];     //��������
	char szBeginTime;           //ͨ����ʼʱ�� hh:mm:ss'ms
	int  nLastTime;             //ͨ������ʱ��
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

	//�����¼�: 0:�����¼�;1:�Ҷ��¼�
	HANDLE m_hDialEvents[2];



	CUsbDevice *m_pUsbDevice;
	//Ӳ��PID,VID
	DWORD m_dwPID;
	DWORD m_dwVID;
	CString m_strServiceIP;
	//��������
	CString m_strPathDialingBell;
	//��������
	CString m_strPathIncommingBell;
	//æ��
	CString m_strPathBusyBell;

	CMCIPlayMusic m_mciMusic;

	
public:
	CIncommingDialog *m_pIncommingDlg;
	CFriendList m_listClient;
	//�����¼�: 0:�����¼�;1:�Ҷ��¼�
	HANDLE m_hAcceptCallEvents[2];
	//TODO:�򵥴���,1 �ͻ��˶���ͨ�����غ��з�,��������صĲ���״̬;
	//              2 ǰ̨�����,���з�,ֻ���������绰����,����ǰ̨���пͻ��˵����--> ��Ϊ�н���,��ȡ���水ť��ʽ,�����¼���ʽ
	//��¼��ǰ״̬: ���� ���� ����
	CallStatus m_callStatus;
	CString	m_name;
	//ip name time  list��ʾʹ��
	CString m_strCallingInfo;
private:
	//�ļ�����
	FileStream m_fs;
	//��¼ͨ����Ϣ,�����ݽ��и���,���һ����д���ļ���
	vector<CallingInfo> m_vecInfo;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TALKDLG_H__854B900F_3565_4694_8B0B_4BDB621CEB31__INCLUDED_)
