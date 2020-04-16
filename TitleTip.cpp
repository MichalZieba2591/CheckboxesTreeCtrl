#ifdef INCLUDE_VERID
static char verid[]="@(#)$RCSfile: TitleTip.cpp,v $$Revision: 1.10 $$Date: 2011/10/31 09:34:29Z $";
#endif
/***********************************************************************
* This module is part of the <SoftTest> system 
* Copyright (c) Soft Computer Consultants, Inc. <2002>
* All Rights Reserved
* This document contains unpublished, confidential and proprietary 
* information of Soft Computer Consultants, Inc. No disclosure or use of 
* any portion of the contents of these materials may be made without the 
* express written consent of Soft Computer Consultants, Inc.
* 
* Author: FName LName
* Description: put file description here 
*
* Caution: put precautions here
* 
***********************************************************************/

#include "stdafx.h"
//#include "gridctrl.h"

#include <windows.h>

#ifndef GRIDCONTROL_NO_TITLETIPS

#include "TitleTip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCmnToolTip

CCmnToolTip::CCmnToolTip():
DrawTimer(1),
HideTimer(2),
m_curDrawTim(0),
m_curHideTim(0),
m_lpszTitleText(NULL),
m_lpLogFont(NULL),
m_crBackClr(CLR_DEFAULT),
m_crTextClr(CLR_DEFAULT),
m_bListFlag(FALSE),
m_nSizeOfSortArrow(0)
{
  m_dwLastLButtonDown = ULONG_MAX;
  m_dwDblClickMsecs   = GetDoubleClickTime();
  m_bCreated          = FALSE;
  m_bEnableFloatingTooltips = FALSE;
  m_IsOwnToolTip = FALSE;
  m_rcTitle.SetRectEmpty();
  m_rcHover.SetRectEmpty();
}

CCmnToolTip::~CCmnToolTip()
{
}


BEGIN_MESSAGE_MAP(CCmnToolTip, CWnd)
	//{{AFX_MSG_MAP(CCmnToolTip)
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCmnToolTip message handlers

BOOL CCmnToolTip::Create(CWnd * pParentWnd)
{
  // Register the window class if it has not already been registered.
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();
	if(!(::GetClassInfo(hInst, TITLETIP_CLASSNAME, &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style			= CS_SAVEBITS;
		wndcls.lpfnWndProc		= ::DefWindowProc;
		wndcls.cbClsExtra		= wndcls.cbWndExtra = 0;
		wndcls.hInstance		= hInst;
		wndcls.hIcon			= NULL;
		wndcls.hCursor			= LoadCursor( hInst, IDC_ARROW );
		wndcls.hbrBackground	= (HBRUSH)(COLOR_INFOBK +1);
		wndcls.lpszMenuName		= NULL;
		wndcls.lpszClassName	= TITLETIP_CLASSNAME;

		if (!AfxRegisterClass(&wndcls))
			AfxThrowResourceException();
	}

	ASSERT_VALID(pParentWnd);

    // Already created?
    if (m_bCreated)
        return TRUE;

	DWORD dwStyle = WS_BORDER | WS_POPUP; 
	DWORD dwExStyle = NULL;
	m_pParentWnd = pParentWnd;
	m_bCreated = CreateEx(dwExStyle, TITLETIP_CLASSNAME, NULL, dwStyle, 
                           0, 0, 0, 0, pParentWnd->GetSafeHwnd(), NULL, NULL );

	if (m_bCreated)
		SetOwner(pParentWnd);

    return m_bCreated;
}

BOOL CCmnToolTip::DestroyWindow() 
{
    m_bCreated = FALSE;
	
	return CWnd::DestroyWindow();
}

// Show 		 - Show the titletip if needed
// rectTitle	 - The rectangle within which the original 
//				    title is constrained - in client coordinates
// lpszTitleText - The text to be displayed
// xoffset		 - Number of pixel that the text is offset from
//				   left border of the cell
void CCmnToolTip::Show(CRect rectTitle, LPCTSTR lpszTitleText, int xoffset /*=0*/,
                     LPRECT lpHoverRect /*=NULL*/,
                     const LOGFONT* lpLogFont /*=NULL*/,
                     COLORREF crTextClr /* CLR_DEFAULT */,
                     COLORREF crBackClr /* CLR_DEFAULT */,
                     BOOL bListFlag /*FALSE*/,
                     int nSizeOfSortArrow /*0*/)
{
  
  
  if (!IsWindow(m_hWnd))
    Create(m_pParentWnd);
  
  ASSERT( ::IsWindow( GetSafeHwnd() ) );
  
  if (rectTitle.IsRectEmpty())
    return;
  
   
		//if tooltip is already displayed and no floating tooltip, don't do anything
	if (IsWindowVisible() && m_bEnableFloatingTooltips == FALSE )
      return;


    m_rectHover = (lpHoverRect != NULL)? lpHoverRect : rectTitle;
    m_rectHover.right++; m_rectHover.bottom++;
    
    m_pParentWnd->ClientToScreen( m_rectHover );
    
    // Do not display the titletip is app does not have focus
    if( GetFocus() == NULL )
      return;
    
    // Define the rectangle outside which the titletip will be hidden.
    // We add a buffer of one pixel around the rectangle
    m_rectTitle.top    = -1;
    m_rectTitle.left   = -xoffset-1;
    m_rectTitle.right  = rectTitle.Width()-xoffset;
    m_rectTitle.bottom = rectTitle.Height()+1;
    
    // Determine the width of the text
    m_pParentWnd->ClientToScreen( rectTitle );
    
    CClientDC dc(this);
    CString strTitle;
    strTitle = lpszTitleText; 

    CFont font, *pOldFont = NULL;
    if (lpLogFont)
    {
      font.CreateFontIndirect(lpLogFont);
      pOldFont = dc.SelectObject( &font );
    }
    else
    {
      // use same font as ctrl
      pOldFont = dc.SelectObject( m_pParentWnd->GetFont() );
    }
    
    CStringArray string_tab;
    CSize size = GetSizeText(strTitle ,dc,string_tab);
    
    TEXTMETRIC tm;
    dc.GetTextMetrics(&tm);
    size.cx += tm.tmOverhang;
    
    CRect rectDisplay = rectTitle;
    CRect rectMainWnd;

    ::AfxGetMainWnd()->GetWindowRect(rectMainWnd);
    rectDisplay.left += xoffset;
    rectDisplay.right = rectDisplay.left + size.cx + xoffset+4;
    
    // move window if does not fit the screen
    int nMovePix = rectDisplay.right - rectMainWnd.right;
    if( nMovePix > 0 )
    {
      rectDisplay.left -= nMovePix;
      rectDisplay.right -= nMovePix;
      if( rectDisplay.left < 0 )
        rectDisplay.left = 0;
    }
    
    rectDisplay.bottom = rectDisplay.top + size.cy + 4;
    nMovePix = rectDisplay.bottom - rectMainWnd.bottom;
    if( nMovePix > 0 )
    {
      rectDisplay.top -= nMovePix;
      rectDisplay.bottom -= nMovePix;
      if( rectDisplay.top < 0 )
        rectDisplay.top = 0;
    }

    //check lenght for each word in curent cell
    int dx = GetLengthText(dc, lpszTitleText);
    CSize sizeLetter = dc.GetTextExtent("a");   
    rectTitle.right -=nSizeOfSortArrow;

    // Do not display if the text fits within available space
    // additionally consider wrapping of text in grid cell
    if (TRUE == m_IsOwnToolTip || (dx + sizeLetter.cx/2 + 2>= rectTitle.Width() || bListFlag == TRUE || rectDisplay.bottom > rectTitle.bottom +4 ||
        ( (rectDisplay.right > rectTitle.right-xoffset + 4)
           && ( string_tab.GetSize() + rectDisplay.Width() / rectTitle.Width())*tm.tmHeight >
            rectTitle.Height() ) ))
    {
      POINT pt;
      GetCursorPos(&pt);
      
      // Show the titletip
      
      if ( m_bEnableFloatingTooltips == TRUE)
      {
        nMovePix = (pt.y + GetSystemMetrics(SM_CYCURSOR) + rectDisplay.Height()) - GetSystemMetrics(SM_CYFULLSCREEN);
        
        if ( nMovePix > 0 )
        {
          int nnMovePix = pt.y - GetSystemMetrics(SM_CYCURSOR)/2 - rectDisplay.Height();
          
          if( nnMovePix < 0 )
          {
            SetWindowPos( &wndTop, rectDisplay.left, 0, 
              rectDisplay.Width(), rectDisplay.Height(), 
              SWP_SHOWWINDOW | SWP_NOACTIVATE );
          }
          else
          {
            SetWindowPos( &wndTop, rectDisplay.left, pt.y - GetSystemMetrics(SM_CYCURSOR)/2 - rectDisplay.Height() , 
              rectDisplay.Width(), rectDisplay.Height(), 
              SWP_SHOWWINDOW | SWP_NOACTIVATE );
          }
        }
        else
        {
          SetWindowPos( &wndTop, rectDisplay.left, pt.y + GetSystemMetrics(SM_CYCURSOR), 
            rectDisplay.Width(), rectDisplay.Height(), 
            SWP_SHOWWINDOW | SWP_NOACTIVATE );
        }
      }
      else
      {
        
        nMovePix = rectTitle.bottom + 1 + rectDisplay.Height() - GetSystemMetrics(SM_CYFULLSCREEN);
        
        if (nMovePix > 0 )
        {
          int nnMovePix = rectTitle.top -1 - rectDisplay.Height();
          if( nnMovePix < 0 )
          {
            SetWindowPos( &wndTop, rectDisplay.left, 0, 
              rectDisplay.Width(), rectDisplay.Height(), 
              SWP_SHOWWINDOW | SWP_NOACTIVATE );
          }
          else
          {
            SetWindowPos(&wndTop, rectDisplay.left, rectTitle.top - 1 - rectDisplay.Height(), rectDisplay.Width(), rectDisplay.Height(), 
              SWP_SHOWWINDOW | SWP_NOACTIVATE );
          }
          
        }
        else
        {
          SetWindowPos(&wndTop, rectDisplay.left, rectTitle.bottom + 1 , rectDisplay.Width(), rectDisplay.Height(), 
            SWP_SHOWWINDOW | SWP_NOACTIVATE );
        }
      }

      CBrush backBrush(crBackClr);
      CBrush* pOldBrush = dc.SelectObject(&backBrush);
      CRect rect;
      dc.GetClipBox(&rect);     // Erase the area needed 
      dc.PatBlt(rect.left, rect.top, rect.Width(), rect.Height(),  PATCOPY);
      dc.SelectObject(pOldBrush);
      
      // Set color
      dc.SetTextColor(crTextClr);
      dc.SetBkMode( TRANSPARENT );
      SetText(0,0,dc,string_tab);
      SetHideTimer();
    }
    else
    {
		  if (lpszTitleText != "Reset Quick Filter")
      {
			  Hide();
      }
    }
    
    dc.SelectObject( pOldFont );
    font.DeleteObject();
}

void CCmnToolTip::Hide()
{
  	if (!::IsWindow(GetSafeHwnd()))
        return;

	ShowWindow( SW_HIDE );
}

BOOL CCmnToolTip::PreTranslateMessage(MSG* pMsg) 
{
    // Used to qualify WM_LBUTTONDOWN messages as double-clicks
    DWORD dwTick=0;
    BOOL bDoubleClick=FALSE;
	
    CWnd *pWnd;
	int hittest;
	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
       // Get tick count since last LButtonDown
        dwTick = GetTickCount();
        bDoubleClick = ((dwTick - m_dwLastLButtonDown) <= m_dwDblClickMsecs);
        m_dwLastLButtonDown = dwTick;
        // NOTE: DO NOT ADD break; STATEMENT HERE! Let code fall through

	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		{
		POINTS pts = MAKEPOINTS( pMsg->lParam );
		POINT  point;
		point.x = pts.x;
		point.y = pts.y;

		ClientToScreen( &point );
        Hide();

		pWnd = WindowFromPoint( point );
		if (!pWnd)
			return CWnd::PreTranslateMessage(pMsg);

		if( pWnd->GetSafeHwnd() == GetSafeHwnd()) 
			pWnd = m_pParentWnd;

		hittest = (int)pWnd->SendMessage(WM_NCHITTEST,0,MAKELONG(point.x,point.y));

		if (hittest == HTCLIENT) 
		{
			pWnd->ScreenToClient( &point );
			pMsg->lParam = MAKELONG(point.x,point.y);
		}
		else 
		{
			switch (pMsg->message) {
			case WM_LBUTTONDOWN: 
				pMsg->message = WM_NCLBUTTONDOWN;
				break;
			case WM_RBUTTONDOWN: 
				pMsg->message = WM_NCRBUTTONDOWN;
				break;
			case WM_MBUTTONDOWN: 
				pMsg->message = WM_NCMBUTTONDOWN;
				break;
			}
			pMsg->wParam = hittest;
			pMsg->lParam = MAKELONG(point.x,point.y);
		}


        // If this is the 2nd WM_LBUTTONDOWN in x milliseconds,
        // post a WM_LBUTTONDBLCLK message instead of a single click.
        pWnd->PostMessage(  bDoubleClick ? WM_LBUTTONDBLCLK : pMsg->message,
                            pMsg->wParam,
                            pMsg->lParam);
		return TRUE;
		}
		
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
        Hide();
		m_pParentWnd->PostMessage( pMsg->message, pMsg->wParam, pMsg->lParam );
		return TRUE;
	}

	if( GetFocus() == NULL )
	{
    KillTimer(HideTimer);
    Hide();
		return TRUE;
	}

	return CWnd::PreTranslateMessage(pMsg);
}



int CCmnToolTip::SelectString(CString &string,CStringArray & string_tab)
{
	int position = 1;
	
	while(position>-1)
	{
		position = string.Find("\n");
		if(position > -1)
		{
			CString str = _T(" ");		
			str+= string.Left(position);
			str.TrimRight();
			str +=" ";
			string_tab.Add(str);		
			string = string.Mid(position+1);
		}
		else
		{
			CString str = _T(" ");
			str += string;
			str.TrimRight();
			str +=" ";
			string_tab.Add(str);
		}
	}

	return string_tab.GetSize();
}

CSize CCmnToolTip::GetSizeText(CString & str,CClientDC &dc,CStringArray & string_tab)
{
	CSize ret_size;
	int tmp_cx = 0,tmp_cy = 0;
	int counter_string = SelectString(str,string_tab);
	int i = 0;

	while(i< string_tab.GetSize())
	{
		CSize size = dc.GetTextExtent(string_tab[i] );
		
		if( size.cx > GetSystemMetrics(SM_CXFULLSCREEN))
		{
			CString str = string_tab[i];
			string_tab.InsertAt(i, str.Left(str.GetLength()/2));
			string_tab.SetAt(i+1, CString(" " + str.Right(str.GetLength()-str.GetLength()/2)));
		}
		else
			i++;
	}
	
	for(i = 0; i< string_tab.GetSize(); i++)
	{
		CSize size = dc.GetTextExtent(string_tab[i] );
		
		if(tmp_cx < size.cx)
		{
				tmp_cx = size.cx;
				tmp_cy = size.cy;	
		}
	}

	ret_size.cx = tmp_cx;
	ret_size.cy = string_tab.GetSize() *  tmp_cy;

	return ret_size;
}

void CCmnToolTip::SetText(int x,int y,CClientDC &dc,CStringArray & string_tab)
{
	CSize size = dc.GetTextExtent( string_tab[0]);

	for(int i = 0; i< string_tab.GetSize(); i++)
		dc.TextOut( 0, i* size.cy, string_tab[i] );
}

void CCmnToolTip::OnTimer(UINT nIDEvent) 
{
  if (HideTimer == nIDEvent)
  {
    POINT pt;
    GetCursorPos(&pt);
  
    if (!m_rectHover.PtInRect(pt) || FALSE == IsWindowVisible()) 
    {
      KillTimer(HideTimer);
      Hide();
    }
  }
  else if (DrawTimer == nIDEvent)
  {
    KillTimer(DrawTimer);
    Show(m_rcTitle, m_lpszTitleText, m_xOffset, m_rcHover.IsRectEmpty()?NULL:&m_rcHover, m_lpLogFont, m_crTextClr, m_crBackClr,
      m_bListFlag, m_nSizeOfSortArrow);
    
  }
  
  CWnd::OnTimer(nIDEvent);
}


/**************************************************************************
* Function name  : CCmnToolTip::GetLengthText
*
* Description    :  check lenght for each word in curent cell
*
* Parameters	 :
*		[in] CString szText :
*
* Return		 : int 
*
* Author		 : Tomasz Szmuc
*
* Date           : 8/31/2005
**************************************************************************/
int CCmnToolTip::GetLengthText(CClientDC &dc, CString szText)
{
  CString szTmp;
  CString str;
	CSize   sz;
	int     dx = 0;
  int nSave = dc.SaveDC();
  dc.SelectObject(GetFont());
  CString sGridText = szText;
  int nTextLength = sGridText.GetLength();
  szTmp = sGridText;

  int nCntBegin = 0;
  int nCntEndSpace = szTmp.Find(" ");
  int nCntEndEnter = szTmp.Find("\r\n");
  int nCntEnd = 0;
  BOOL bSpacEnt = FALSE;
  if (nCntEndSpace > nCntEndEnter)
  {
    nCntEnd = nCntEndEnter;
    bSpacEnt = TRUE;
  }
  else
  {
    nCntEnd = nCntEndSpace;
    bSpacEnt = FALSE;
  }
  if (nCntEndSpace == -1)
  {
    nCntEnd = nCntEndEnter;
    bSpacEnt = TRUE;
  }
  else if (nCntEndEnter == -1)
  {
    nCntEnd = nCntEndSpace;
    bSpacEnt = FALSE;
  }
  //calculate length of words in cell
  while(-1 != nCntEnd)
  {
    if (nCntBegin == 0)
    {
      str = szTmp.Mid(nCntBegin, nCntEnd - nCntBegin);
    }
    else
    {
      if (TRUE == bSpacEnt)
      {
        str = szTmp.Mid(nCntBegin + 1, nCntEnd - nCntBegin - 2);
      }
      else
      {
        str = szTmp.Mid(nCntBegin + 1, nCntEnd - nCntBegin - 1);
      }
    }
    sz = dc.GetTextExtent(str);

    if (sz.cx > dx)
				dx = sz.cx;
  
    nCntBegin = nCntEnd;
    nCntEndSpace = szTmp.Find(" ",nCntBegin + 1);
    nCntEndEnter = szTmp.Find("\r\n",nCntBegin + 1);
    if (nCntEndSpace > nCntEndEnter)
    {
      nCntEnd = nCntEndEnter;
      bSpacEnt = TRUE;
    }
    else
    {
      nCntEnd = nCntEndSpace;
      bSpacEnt = FALSE;
    }
    if (nCntEndSpace == -1)
    {
      nCntEnd = nCntEndEnter;
      bSpacEnt = TRUE;
    }
    else if (nCntEndEnter == -1)
    {
      nCntEnd = nCntEndSpace;
      bSpacEnt = FALSE;
    }
  }
  //calculate length of last word in cell
  if (nTextLength > nCntBegin)
  {
    if (TRUE == bSpacEnt)
    {
      str = szTmp.Right(nTextLength - nCntBegin - 2);
    }
    else
    {
      str = szTmp.Right(nTextLength - nCntBegin - 1);
    }
    sz = dc.GetTextExtent(str);

    if (sz.cx > dx)
				dx = sz.cx;
  }
	dc.RestoreDC(nSave);  
  
  return dx;
}

void CCmnToolTip::SetFloatingToolTip(BOOL bSet)
{
  m_bEnableFloatingTooltips = bSet;
}

//! Set timer in order to hide tooltip
/*! \author Grzegorz Oronowicz
*/
void CCmnToolTip::SetHideTimer()
{
  if (0 != m_curHideTim)
  {
    KillTimer(HideTimer);
  }
  m_curHideTim = SetTimer(HideTimer, 200, NULL);
}

//! Set timer in order to draw ToolTip
/*! \author Grzegorz Oronowicz
*/

void CCmnToolTip::SetDrawTimer()
{
  if (0 != m_curDrawTim)
  {
    KillTimer(DrawTimer);
  }
  m_curDrawTim = SetTimer(DrawTimer, 200, NULL);
}

//! Prepare tooltip to display
/*! \author Grzegorz Oronowicz
    \param  same as in Show method
*/

void CCmnToolTip::PrepareShow(CRect rectTitle, LPCTSTR lpszTitleText, int xoffset /*= 0*/,
				LPRECT lpHoverRect /*= NULL*/, const LOGFONT* lpLogFont /*= NULL*/,
        COLORREF crTextClr /*= CLR_DEFAULT*/,
				COLORREF crBackClr /*= CLR_DEFAULT*/, BOOL bListFlag /*= FALSE*/, int nSizeOfSortArrow /*= 0*/)
{
  if (!IsWindow(m_hWnd))
  {
    Create(m_pParentWnd);
  }
  
  ASSERT(::IsWindow(GetSafeHwnd()));

  m_rcTitle = rectTitle;
  if (NULL != lpHoverRect)
  {
    m_rcHover = *lpHoverRect;
  }
  else
  {
    m_rcHover.SetRectEmpty();
  }

  m_lpszTitleText = lpszTitleText;
  m_lpLogFont = (LOGFONT*)lpLogFont;
  m_xOffset = xoffset;
  m_crBackClr = crBackClr;
  m_crTextClr = crTextClr;
  m_bListFlag = bListFlag;
  m_nSizeOfSortArrow = nSizeOfSortArrow;
  SetDrawTimer();
}

void CCmnToolTip::SetOwnToolTip(BOOL bShow /*= FALSE*/)
{
  m_IsOwnToolTip = bShow;
}

#endif // GRIDCONTROL_NO_TITLETIPS