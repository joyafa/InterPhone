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
	INITIAL    = 0, //��ʼ״̬     
	DIALING    = 1, //����ing
	ACCEPTING     , //����ing: ���з�,������������,δ����,���ԹҶ�(Reject),���Խ���
	ONLINE        , //ͨ����,���ԹҶ�(Hangup),���ߵȶԷ��Ҷ�(�յ��Է�ʲô��Ϣ)
	BUSYING         //���г�ʱ,æ��
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
	 //��������,TRUE:����;FALSE:����;
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

	//�������е�ͨ��
	void EndCall() ;

	void GetConfigInfo();
private:
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
	afx_msg void OnBnClickedBtnCall();

	//�����¼�: 0:�����¼�;1:�Ҷ��¼�
	HANDLE m_hDialEvents[2];


	//TODO:�򵥴���,1 �ͻ��˶���ͨ�����غ��з�,��������صĲ���״̬;
	//              2 ǰ̨�����,���з�,ֻ���������绰����,����ǰ̨���пͻ��˵����--> ��Ϊ�н���,��ȡ���水ť��ʽ,�����¼���ʽ
	//��¼��ǰ״̬: ���� ���� ����
	CallStatus m_callStatus;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TALKCALLDLG_H__3BB882D4_255E_4002_A514_25896E860019__INCLUDED_)
