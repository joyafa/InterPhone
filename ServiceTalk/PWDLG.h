#if !defined(AFX_PWDLG_H__C7132261_737F_4391_939D_F6A8788C988A__INCLUDED_)
#define AFX_PWDLG_H__C7132261_737F_4391_939D_F6A8788C988A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PWDLG.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PWDLG dialog

class PWDLG : public CDialog
{
// Construction
public:
	PWDLG(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(PWDLG)
	enum { IDD = IDD_DIALOG_PW };
	CString	m_pw;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PWDLG)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PWDLG)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PWDLG_H__C7132261_737F_4391_939D_F6A8788C988A__INCLUDED_)
