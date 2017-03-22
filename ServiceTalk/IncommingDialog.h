#pragma once


// CIncommingDialog 对话框
class CServiceTalkDlg;
class CIncommingDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CIncommingDialog)

public:
	CIncommingDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CIncommingDialog();

// 对话框数据
	enum { IDD = IDD_DIALOG_INCOMING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAccept();
	afx_msg void OnBnClickedReject();

private:
	CServiceTalkDlg *m_pServiceDlg;
	DWORD m_dwCounts;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
