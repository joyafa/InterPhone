// FriendList.cpp : 实现文件
//

#include "stdafx.h"
#include "FriendList.h"
#include "resource.h"
#include "conio.h"
// CFriendList
IMPLEMENT_DYNAMIC(CFriendList, CListCtrl)

CFriendList::CFriendList()
{
	m_nLastSel = -2;
	m_pToolTip = new CToolTipCtrlX; 
	m_nItem = -1;
	m_nSubItem = -1;
}

CFriendList::~CFriendList()
{
	m_bmpSel1.DeleteObject();
	m_bmpSel2.DeleteObject();
	m_bmpUnSel.DeleteObject();
	delete m_pToolTip;
}

BEGIN_MESSAGE_MAP(CFriendList, CListCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void CFriendList::PreSubclassWindow()
{
	CRect rectBig(6,10,36,22),rectSmall(7,7,15,15),
		rectBig2(6,0,36,12),rectAll(0,0,330,28);
	CDC dcMem;
	CClientDC dc(this);

	CBrush hBrushBack,hBrushGray;
	CPen hPen,hPenWrite;
	hBrushBack.CreateSolidBrush(RGB(255,255,255));
	hBrushGray.CreateSolidBrush(GRAY_BRUSH);
	hPen.CreatePen(PS_SOLID,1,RGB(0,0,0));
	hPenWrite.CreatePen(PS_SOLID,1,RGB(255,255,255));
	CFont font;
	font.CreatePointFont(90,TEXT("微软雅黑"),NULL);
	
#ifdef IDB_FNDLST_SEL
	m_bmpSel1.LoadBitmap(IDB_FNDLST_SEL);
#else
	dcMem.CreateCompatibleDC(&dc);
	m_bmpSel1.CreateCompatibleBitmap(&dcMem,330,22);

	dcMem.SelectObject(&m_bmpSel1);

	dcMem.SelectObject(&hBrushBack);
	dcMem.SelectObject(&hPenWrite);
	dcMem.SetBkColor(RGB(255,255,255));
	dcMem.Rectangle(rectAll);

	dcMem.SelectObject(hPen);
	dcMem.SelectObject(hBrushGray);

	dcMem.MoveTo(0,22);
	dcMem.LineTo(0,0);
	dcMem.LineTo(185,0);
	dcMem.LineTo(185,22);
	
	dcMem.Rectangle(rectBig);

	dcMem.SelectObject(&font);
	dcMem.SetBkMode(TRANSPARENT);
	CRect rectText1(9,15,40,22);
	dcMem.SetTextColor(RGB(255,255,255));
	dcMem.DrawText(TEXT("头像"),&rectText1,DT_LEFT);

	dcMem.DeleteDC();	
#endif
	
#ifdef IDB_FNDLST_SEL2
	m_bmpSel2.LoadBitmap(IDB_FNDLST_SEL2);
#else
	dcMem.CreateCompatibleDC(&dc);
	m_bmpSel2.CreateCompatibleBitmap(&dcMem,330,22);

	dcMem.SelectObject(&m_bmpSel2);
	
	dcMem.SelectObject(&hBrushBack);
	dcMem.SelectObject(&hPenWrite);
	dcMem.SetBkColor(RGB(255,255,255));
	dcMem.Rectangle(rectAll);

	dcMem.SelectObject(hBrushGray);
	dcMem.SelectObject(hPen);

	dcMem.MoveTo(0,0);
	dcMem.LineTo(0,20);
	dcMem.LineTo(185,20);
	dcMem.LineTo(185,-1);
	
	dcMem.Rectangle(rectBig2);

	dcMem.SelectObject(&font);
	dcMem.SetBkMode(TRANSPARENT);
	CRect rectText2(9,-6,40,22);
	dcMem.SetTextColor(RGB(255,255,255));
	dcMem.DrawText(TEXT("头像"),&rectText2,DT_LEFT);

	dcMem.DeleteDC();
#endif
	
#ifdef IDB_FNDLST_UNSEL
	m_bmpUnSel.LoadBitmap(IDB_FNDLST_UNSEL);
#else
	dcMem.CreateCompatibleDC(&dc);
	m_bmpUnSel.CreateCompatibleBitmap(&dcMem,330,22);

	dcMem.SelectObject(&m_bmpUnSel);
	
	dcMem.SelectObject(&hBrushBack);
	dcMem.SelectObject(&hPen);
	dcMem.SetBkColor(RGB(255,255,255));
	dcMem.Rectangle(rectAll);

	dcMem.SelectObject(hBrushGray);
	dcMem.SelectObject(hPen);
	
	dcMem.Rectangle(rectSmall);

	dcMem.DeleteDC();
#endif

	// 设置列表项高度
	m_bmpPortrait.CreateBitmap(1,22,0,0,NULL);
	m_imgPortrait.Create(1,28,ILC_COLOR24,1,1);
	m_imgPortrait.Add(&m_bmpPortrait,RGB(11,11,11));

	SetImageList(&m_imgPortrait,2);

	CRect rect;
	GetWindowRect(&rect);
	InsertColumn(0,TEXT("客户机"),LVCFMT_LEFT);
	SetColumnWidth(0,rect.right - rect.left-4);

	SetExtendedStyle( LVS_EX_FULLROWSELECT
		|LVS_OWNERDRAWFIXED 
		|LVS_EX_SUBITEMIMAGES
		|LBS_OWNERDRAWVARIABLE);

	CListCtrl::PreSubclassWindow();

	hBrushBack.DeleteObject();
	hBrushGray.DeleteObject();
	hPen.DeleteObject();
	hPenWrite.DeleteObject();
}

void CFriendList::Init()
{
	if (m_pToolTip->m_hWnd == NULL)
	{
		EnableToolTips(TRUE);
		// Create ToolTip control
		m_pToolTip->Create(this);
		m_pToolTip->SetMaxTipWidth(500);//若屏蔽这行提示信息的格式可能会出现问题，不能换行  
		m_pToolTip->ModifyStyle(0, TTS_NOPREFIX);
		m_pToolTip->SetDelayTime(TTDT_AUTOPOP, 2000);//弹出提示停留时间
		m_pToolTip->SetDelayTime(TTDT_INITIAL, 500); //鼠标停留 500ms 弹出提示
		// Create inactive
		m_pToolTip->Activate(FALSE);
		// Enable multiline
		m_pToolTip->SendMessage(TTM_SETMAXTIPWIDTH, 0, 283);
	} // if
}


void CFriendList::SetTooltipText(LPCTSTR lpszText, BOOL bActivate)
{
	// We cannot accept NULL pointer
	if (lpszText == NULL) return;

	// If there is no tooltip defined then add it
	if (m_pToolTip->GetToolCount() == 0)
	{
		CRect rectBtn;
		GetClientRect(rectBtn);
		m_pToolTip->AddTool(this, lpszText, rectBtn, 1);
	} // if

	  // Set text for tooltip
	m_pToolTip->UpdateTipText(lpszText, this, 1);
	m_pToolTip->Activate(bActivate);
} // End of SetTooltipText


void CFriendList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rectBottom,rectList;

	GetItemRect(GetItemCount()-1,&rectBottom,LVIR_BOUNDS);
	GetWindowRect(&rectList);

	LVHITTESTINFO   LVHTestInfo;    
	LVHTestInfo.pt = point;    
	HitTest(&LVHTestInfo);    
	int nRow=LVHTestInfo.iItem;   //行数 
	SubItemHitTest(&LVHTestInfo);    
	int nCol=LVHTestInfo.iSubItem;  //列数 

	if (nRow == -1)
	{
		CListCtrl::OnLButtonDblClk(nFlags, point);	
		return;
	}	
	
	CListCtrl::OnLButtonDblClk(nFlags, point);
}

//TODO:改成结构体方式吧
//
void CFriendList::AddNewUser( CString str )
{	
	CString strUserName = str;
	int nPos = strUserName.Find("\n");
	strUserName.SetAt(nPos, '\0');

	//挂断,更新结束时间和时长
	map<string, string>::iterator it = m_mapCallData.find(strUserName.GetString());
	if (m_mapCallData.end() != it)
	{
		it->second = str;
	}
	else
	{
		m_mapCallData[strUserName.GetString()] = str;
		InsertItem(0, strUserName.GetString(), 0);
	}


	m_nLastSel = -2;
}




void CFriendList::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = 28;
}

void CFriendList::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{	
	CDC *pDc = CDC::FromHandle(lpDrawItemStruct->hDC);
	pDc->SetBkMode(TRANSPARENT);

	CRect rect = lpDrawItemStruct->rcItem;

	if (GetItemState(lpDrawItemStruct->itemID,LVIS_SELECTED))
	{
		// 如果选中的是下半部分, 则画选择条下半部
		if (lpDrawItemStruct->itemID == m_nLastSel+1)
		{
			CDC dcMem;
			dcMem.CreateCompatibleDC(pDc);
			pDc->SetTextColor(RGB(77,77,77));

			CBitmap *pOldBitmap = dcMem.SelectObject(&m_bmpSel2);

			pDc->BitBlt(rect.left + ITEMOFFSET,rect.top,330,28,&dcMem,0,0,SRCCOPY);
			dcMem.SelectObject(pOldBitmap);

			return;
		}
		else
		{
			pDc->SetTextColor(RGB(77,77,77));

			CDC dcMem;
			dcMem.CreateCompatibleDC(pDc);

			CBitmap *pOldBitmap = dcMem.SelectObject(&m_bmpSel1);

			pDc->BitBlt(rect.left + ITEMOFFSET,rect.top+1,330,28,&dcMem,0,0,SRCCOPY);

			CString tmp;
			tmp = GetItemText(lpDrawItemStruct->itemID,0);
			if (tmp.GetLength()>16)
			{
				tmp = GetStringLeft(tmp,8) + TEXT("...");
			}
			rect.left+=52;
			rect.top+=6;
			pDc->DrawText(tmp,rect,DT_LEFT);

			dcMem.SelectObject(pOldBitmap);
		}		
	} 
	else	// 如果画的是没有选中的部分
	{

		// 如果画的是当前选中项目上半部
		if (lpDrawItemStruct->itemID == m_nLastSel)
		{
			pDc->SetTextColor(RGB(77,77,77));
			CDC dcMem;
			dcMem.CreateCompatibleDC(pDc);

			CBitmap *pOldBitmap = dcMem.SelectObject(&m_bmpSel1);

			pDc->BitBlt(rect.left + ITEMOFFSET,rect.top+1,330,28,&dcMem,0,0,SRCCOPY);

			CString tmp;
			tmp = GetItemText(lpDrawItemStruct->itemID,0);
			if (tmp.GetLength()>16)
			{
				tmp = GetStringLeft(tmp,8) + TEXT("...");
			}
			rect.left+= 52;
			rect.top+=6;
			pDc->DrawText(tmp,rect,DT_LEFT);

			return;
		}
		else if (lpDrawItemStruct->itemID == m_nLastSel + 1)	
		{	// 如果画的是选中项目的下半部
			CDC dcMem;
			dcMem.CreateCompatibleDC(pDc);
			pDc->SetTextColor(RGB(77,77,77));

			CBitmap *pOldBitmap = dcMem.SelectObject(&m_bmpSel2);

			pDc->BitBlt(rect.left + ITEMOFFSET,rect.top,330,28,&dcMem,0,0,SRCCOPY);
			dcMem.SelectObject(pOldBitmap);
			return;
		}
		else	// 如果画的是没有选中的部分
		{
			CDC dcMem;
			dcMem.CreateCompatibleDC(pDc);
			pDc->SetTextColor(RGB(77,77,77));

			CBitmap *pOldBitmap = dcMem.SelectObject(&m_bmpUnSel);

			pDc->BitBlt(rect.left + ITEMOFFSET,rect.top,330,28,&dcMem,0,0,SRCCOPY);

			CString tmp;
			tmp = GetItemText(lpDrawItemStruct->itemID,0);
			if (tmp.GetLength()>=24)
			{
				tmp = tmp.Left(22) + TEXT("...");
			}
			rect.left+=30;
			rect.top+=5;
			pDc->DrawText(tmp,rect,DT_LEFT);
		}	
	}
	// TODO:  添加您的代码以绘制指定项
}

void CFriendList::OnLButtonDown(UINT nFlags, CPoint point)
{
	LVHITTESTINFO   LVHTestInfo;    
	LVHTestInfo.pt = point;    
	HitTest(&LVHTestInfo);    
	unsigned int nRow=LVHTestInfo.iItem;   //行数 
	SubItemHitTest(&LVHTestInfo);    
	unsigned int nCol=LVHTestInfo.iSubItem;  //列数 

	if (nRow == -1 && m_nLastSel != -1)
	{
		DeleteItem(m_nLastSel+1);
		m_nLastSel = -2;
		SetItemState(-1, LVIS_SELECTED,LVIS_SELECTED);
		CListCtrl::OnLButtonDown(nFlags, point);
		Invalidate();
		return;
	}
	

	if (m_nLastSel == -2)
	{
		InsertItem(nRow + 1, GetItemText(nRow,0));
	}
	else if (m_nLastSel != nRow 
		&& m_nLastSel + 1 != nRow
		&& m_nLastSel != -1)
	{
		DeleteItem(m_nLastSel+1);
		InsertItem(nRow + 1, GetItemText(nRow,0));		
		Invalidate();
	}
	

	if (m_nLastSel == -2)
	{
		m_nLastSel = nRow;
		SetItemState(nRow , LVIS_SELECTED,LVIS_SELECTED);
	}
	else if(nRow > m_nLastSel)
	{
		m_nLastSel = nRow - 1;
		SetItemState(nRow , LVIS_SELECTED,LVIS_SELECTED);
	}
	else
	{
		m_nLastSel = nRow;
		SetItemState(nRow, LVIS_SELECTED,LVIS_SELECTED);
	}

	CListCtrl::OnLButtonDown(nFlags, point);
}

void CFriendList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CFriendList::OnRButtonDown(UINT nFlags, CPoint point)
{
	//return;

	CListCtrl::OnRButtonDown(nFlags, point);
}

CString CFriendList::GetStringLeft( CString &str, int nLeft )
{
	CString   sRet=_T("");   
	int   nPos=0;   
	while(nPos<str.GetLength()   &&   nLeft)   
	{   
		if(str.GetAt(nPos)<0){   
			sRet=sRet+str.GetAt(nPos)+str.GetAt(nPos+1);   
			nPos++;   
		}else   
			sRet+=str.GetAt(nPos);   
        nPos++;   
        nLeft--;   
	}   
	return   sRet;   
}


BOOL CFriendList::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	m_pToolTip->RelayEvent(pMsg);

	return CListCtrl::PreTranslateMessage(pMsg);
}


void CFriendList::OnMouseMove(UINT nFlags, CPoint point)
{
	CString str;
	LVHITTESTINFO lvhti;
	// 判断鼠标当前所在的位置(行, 列)
	lvhti.pt = point;
	SubItemHitTest(&lvhti);

	// 如果鼠标移动到另一个单元格内, 则进行处理; 否则, 不做处理
	if ((lvhti.iItem != m_nItem) || (lvhti.iSubItem != m_nSubItem))
	{
		// 保存当前鼠标所在的(行,列)
		m_nItem = lvhti.iItem;
		m_nSubItem = lvhti.iSubItem;

		// 如果鼠标移动到一个合法的单元格内,则显示新的提示信息
		// 否则, 不显示提示
		if ((m_nItem != -1) && (m_nSubItem != -1))
		{
			// @@@@@@@@ 在这里修改要显示的提示信息
			// 这里仅仅是一个例子---获得当前单元格的文字信息, 并设置为新的提示信息
			str = GetItemText(m_nItem, m_nSubItem);
		}
		m_mapCallData.find("111111");
		SetTooltipText(m_mapCallData[str.GetString()].c_str(), TRUE);
	}

	CListCtrl::OnMouseMove(nFlags, point);
}
