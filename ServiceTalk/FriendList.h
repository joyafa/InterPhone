// *************************************************
// CFriendList v 1.0
//
// �����3��Bitmap��Դ, ��������ID����:
//
// IDB_FNDLST_SEL	��ͷ���ϰ벿��
// IDB_FNDLST_SEL2	��ͷ���°벿��
// IDB_FNDLST_UNSEL	Сͷ��
//

#pragma once
#include "ToolTipCtrlX.h"
#include <map>
#include <string>
using namespace std;
// CFriendList
#define ITEMOFFSET 1

class CFriendList : public CListCtrl
{
	DECLARE_DYNAMIC(CFriendList)

public:
	CFriendList();
	virtual ~CFriendList();

	// ������б���
	void AddNewUser(CString str);

	void Init();
	void SetTooltipText(LPCTSTR lpszText, BOOL bActivate);
protected:
	CString GetStringLeft(CString &str, int nLeft);
	unsigned int m_nLastSel;
	CImageList m_imgPortrait;
	CBitmap m_bmpPortrait, 
		m_bmpSel1, m_bmpSel2, m_bmpUnSel;

	virtual void PreSubclassWindow();

	CToolTipCtrlX *m_pToolTip;

	int m_nItem;
	int m_nSubItem;
	map<string, string> m_mapCallData;
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};