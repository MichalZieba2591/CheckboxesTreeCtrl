#if !defined(AFX_TITLETIP_H__C7165DA1_187F_11D1_992F_895E185F9C72__INCLUDED_)
#define AFX_TITLETIP_H__C7165DA1_187F_11D1_992F_895E185F9C72__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define TITLETIP_CLASSNAME _T("ZTitleTip")

//
// This struct is used by the EnumChildWindows function in the OnTimer Handler
//
typedef struct tagTTWndStruct 
{
	HWND	hWndToolTip;
	HWND	hWndToolTipParent;
} TTWNDSTRUCT;

typedef struct tagNM_INFO {
    NMHDR hdr;
    CString szID;
    CString szInfo;
} NM_USER_INFO;

/////////////////////////////////////////////////////////////////////////////
// CCmnToolTip window/

class CCmnToolTip : public CWnd
{
// Construction
public:
	CCmnToolTip();
	virtual ~CCmnToolTip();
	virtual BOOL Create( CWnd *pParentWnd);

// Attributes
public:
    void SetFloatingToolTip(BOOL bSet);
    void SetParentWnd(CWnd* pParentWnd)  { m_pParentWnd = pParentWnd; }
    CWnd* GetParentWnd()                 { return m_pParentWnd;       }

// Operations
public:
	CString m_szProduct;
	int SelectString(CString & string,CStringArray & string_tab);
    void PrepareShow(CRect rectTitle, LPCTSTR lpszTitleText, int xoffset = 0,
		LPRECT lpHoverRect = NULL, const LOGFONT* lpLogFont = NULL,
        COLORREF crTextClr = CLR_DEFAULT,
		COLORREF crBackClr = CLR_DEFAULT, BOOL bListFlag = FALSE, int nSizeOfSortArrow = 0);
	void Show(CRect rectTitle, LPCTSTR lpszTitleText, int xoffset = 0,
		LPRECT lpHoverRect = NULL, const LOGFONT* lpLogFont = NULL,
        COLORREF crTextClr = CLR_DEFAULT,
		COLORREF crBackClr = CLR_DEFAULT, BOOL bListFlag = FALSE, int nSizeOfSortArrow = 0);
	void Hide();
	void SetOwnToolTip(BOOL bShow = FALSE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCmnToolTip)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
protected:
  CWnd  *m_pParentWnd;
  CRect  m_rectTitle;
  CRect  m_rectHover;
  DWORD  m_dwLastLButtonDown;
  DWORD  m_dwDblClickMsecs;
  BOOL   m_bCreated;
  BOOL   m_IsOwnToolTip;

  // Values to show Title Tip

  CRect m_rcTitle;
  CRect m_rcHover;
  LPCTSTR m_lpszTitleText;
  LOGFONT* m_lpLogFont;
  int m_xOffset;
  COLORREF m_crTextClr;
  COLORREF m_crBackClr;
  BOOL m_bListFlag;
  int m_nSizeOfSortArrow;


  // Timers

  const UINT DrawTimer;
  const UINT HideTimer;

  UINT m_curDrawTim;
  UINT m_curHideTim;

// Generated message map functions
protected:
	void SetDrawTimer();
	void SetHideTimer();
	int GetLengthText(CClientDC &dc, CString szText);
	//{{AFX_MSG(CCmnToolTip)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CSize GetSizeText(CString & str,CClientDC & dc,CStringArray & string_tab);
	BOOL m_bEnableFloatingTooltips;
	void SetText(int x,int y,CClientDC &dc,CStringArray & string_tab);

};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TITLETIP_H__C7165DA1_187F_11D1_992F_895E185F9C72__INCLUDED_)
