#include "stdafx.h"
#include "CheckboxesTreeCtrl.h"
//#include "FlatPopupMenu.h"
#include "resource.h"
#include <afxadv.h> 
//#include "graphics.h"
//#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW 
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CCheckboxesTreeCtrl::CCheckboxesTreeCtrl()
{
  m_hDragSourceItem = NULL;
  m_hDragTargetItem = NULL;
  m_pDragImageList  = NULL;
	
  m_defaultCursor = ::LoadCursor(NULL,IDC_ARROW);
  m_noCursor = ::LoadCursor(NULL,IDC_NO);
  m_bIsBandingON = false;
  m_pImgList = NULL;
  m_hRClickItem = NULL;
  m_bMoveItem = true;
  m_cMaskColor = RGB(0,0,0);
  m_bContinueScan = true;
  m_nNumberOfCheckboxes = NULL;
  m_bDescriptionTS = false;
  m_hItemPrev = NULL;
  m_ToolTip.SetParentWnd(this);
  m_bMouseOver = FALSE;
  m_bContextMenu = TRUE;
  m_arrStructItemsTree.RemoveAll();
}

CCheckboxesTreeCtrl::~CCheckboxesTreeCtrl()
{
  
	if( m_pImgList )
	{
		m_pImgList->DeleteImageList();
		m_pImgList = NULL;
		delete m_pImgList;
	}

	if( m_pDragImageList )
	{
		m_pDragImageList->DeleteImageList();
		m_pDragImageList = NULL;
		delete m_pDragImageList;
	}

  for (int i = m_arrStructItemsTree.GetSize()-1; i >= 0 ; i-- )
  {
    m_arrStructItemsTree.GetAt(i).lpInsertStruct = NULL;
    delete m_arrStructItemsTree.GetAt(i).lpInsertStruct;
    m_arrStructItemsTree.GetAt(i).pCustomData = NULL;;
    delete m_arrStructItemsTree.GetAt(i).pCustomData;
    m_arrStructItemsTree.RemoveAt(i);
  }
  m_arrStructItemsTree.RemoveAll();
  m_arrStructItemsTree.FreeExtra();

  m_ToolTip.DestroyWindow();
  // Destroy the cursor (if any)
  if (m_noCursor)
  { 
    ::DestroyCursor(m_noCursor);
    m_noCursor = NULL;
  }
  // Destroy the cursor (if any)
  if (m_defaultCursor)
  { 
    ::DestroyCursor(m_defaultCursor);
    m_defaultCursor = NULL;
  }
}

BEGIN_MESSAGE_MAP(CCheckboxesTreeCtrl, CTreeCtrl)
//{{AFX_MSG_MAP(CCheckboxesTreeCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemexpanding)
	ON_WM_LBUTTONUP()
	ON_NOTIFY_REFLECT(NM_CLICK, OnClickLButton)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_NOTIFY_REFLECT(TVN_SELCHANGING, OnSelchanging)
	ON_NOTIFY_REFLECT(TVN_KEYDOWN, OnKeydown)
	ON_MESSAGE(TVN_ITEM_CHECK_ONE, SetChkBoxOne)	
	ON_MESSAGE(TVN_ITEM_CHECK_TWO, SetChkBoxTwo)	
	ON_MESSAGE(TVN_ITEM_CHECK_THREE, SetChkBoxThree)	
	ON_MESSAGE(TVN_ITEM_CHECK_FOUR, SetChkBoxFour)	
	ON_WM_PAINT()
	ON_MESSAGE (WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE (WM_MOUSEHOVER, OnMouseHover)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckboxesTreeCtrl message handlers


static int CALLBACK MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{  
   CCheckboxesTreeCtrl* pmyTreeCtrla = (CCheckboxesTreeCtrl*) lParamSort;
   PCUSTOMITEMDATA pCustData1 = reinterpret_cast<PCUSTOMITEMDATA>(lParam1);
   PCUSTOMITEMDATA pCustData2 = reinterpret_cast<PCUSTOMITEMDATA>(lParam2);
   if ( NULL == pCustData1->m_llCode)
   {
     pCustData1->m_llCode = 999999999999;
   }
   if ( NULL == pCustData2->m_llCode)
   {
     pCustData2->m_llCode = 999999999999;
   }
   
   return (pCustData1->m_llCode != NULL && pCustData2->m_llCode != NULL) 
     ? (pCustData1->m_llCode > pCustData2->m_llCode) : 0;
}

int CCheckboxesTreeCtrl::SelectItems( LOOPINFO* pLoopInfo )
{
	if( !pLoopInfo->hItem )
		return -1;

	if( !pLoopInfo->pParent )
		return -1;

	//Get the tree control from the looping info
	CCheckboxesTreeCtrl* pCustomTree = pLoopInfo->pTree;
	CString str = pCustomTree->GetItemText(pLoopInfo->hItem);

	bool bFlag = false;
	if( pLoopInfo->m_param1 )
		bFlag = * ( (bool*)(pLoopInfo->m_param1) );
	
	if( bFlag )
	{
		pCustomTree->SetItemState( pLoopInfo->hItem,TVIS_SELECTED,TVIS_SELECTED );
	}
	else
	{
		pCustomTree->SetItemState( pLoopInfo->hItem,~TVIS_SELECTED,TVIS_SELECTED );
	}
	return 0;
} 


int CCheckboxesTreeCtrl::FindItemsInRect( LOOPINFO* pLoopInfo )
{
	if( !pLoopInfo->hItem )
		return -1;

	if( !pLoopInfo->pParent )
		return -1;

	CCheckboxesTreeCtrl* pCustomTree = pLoopInfo->pTree;
	CRect* rc = (CRect*)(pLoopInfo->m_param1);
	CRect itemRect;
	pCustomTree->GetItemRect( pLoopInfo->hItem ,&itemRect,false);

	CString str = pCustomTree->GetItemText( pLoopInfo->hItem );
	pCustomTree->ClientToScreen( &itemRect );

	CRect resRect;

	bool bFound = false;
	if( itemRect.top >= rc->top &&  itemRect.bottom <= rc->bottom )
	{
		bFound = true;
	}
	if( bFound )
	{
		pCustomTree->m_aItemsInRect.Add( pLoopInfo->hItem  );	
	}
	return 0;
}


int	CCheckboxesTreeCtrl::DrawItem( LOOPINFO* pLoopInfo )
{
	if( !pLoopInfo->hItem )
		return -1;

	CCheckboxesTreeCtrl* pCustomTree = pLoopInfo->pTree;

	CRect labelRect;
	pCustomTree->GetItemRect( pLoopInfo->hItem,&labelRect,true );

	if( labelRect.left < 0 || labelRect.right < 0 || labelRect.top < 0 || labelRect.bottom < 0 )
		return -1;
	
	COLORREF cItemColor = RGB(0,0,0 );
	bool bItemBold = false;
	PCUSTOMITEMDATA pCustomData  = (PCUSTOMITEMDATA) pCustomTree->GetItemData( pLoopInfo->hItem );
	if( pCustomData )
	{
		if( pCustomData->m_bEnable )
		{
			cItemColor = pCustomData->m_cItemColor;
		}
		else
		{
			cItemColor	=	::GetSysColor(COLOR_GRAYTEXT);
		}
		bItemBold  = pCustomData->m_bIsBold;
	}

	CClientDC pDC( pCustomTree );
	pDC.SetTextColor( cItemColor );
	UINT selflag				= TVIS_DROPHILITED | TVIS_SELECTED;
	UINT uItemSelState	  = pCustomTree->GetItemState(pLoopInfo->hItem,selflag);

	CFont* pFont = pCustomTree->GetFont();
	LOGFONT lf;
	pFont->GetLogFont(&lf );

	if( bItemBold )
	{
		lf.lfWeight = FW_BOLD;
	}

	CFont newFont;
	newFont.CreateFontIndirect( &lf );
	pDC.SelectObject(&newFont );

	CString sItem = pCustomTree->GetItemText( pLoopInfo->hItem );

	if( !pCustomData->m_bChecked )
	{
		int imgW,imgH;
		ImageList_GetIconSize( pCustomTree->GetImageList(TVSIL_NORMAL)->GetSafeHandle(),&imgW,&imgH);
		labelRect.left = labelRect.left - 10*(imgW/pLoopInfo->m_nNumberOfCheckboxes); 
	}

	CRect bkRect = labelRect;
	bkRect.right = bkRect.right +  bkRect.Width()/3;
	COLORREF bkColor = pCustomTree->GetBkColor();
	CBrush bkbrush( bkColor );
	CGdiObject* pOldBrush = pDC.SelectObject(&bkbrush);
	
	CPen bkPen( PS_SOLID,1,bkColor);
	CPen* pOldpen = pDC.SelectObject(&bkPen);

	pDC.Rectangle( labelRect );
	pDC.SelectObject(pOldBrush);
	pDC.SelectObject(pOldpen);

	if( uItemSelState & selflag )
	{
			pDC.SetTextColor( ::GetSysColor(COLOR_HIGHLIGHTTEXT) );	
			CSize sz = pDC.GetTextExtent(sItem);			
			pCustomTree->ClientToScreen( &labelRect );

			CRect rc;
			rc.left = labelRect.left;
			rc.top  = labelRect.top;

			rc.right = rc.left + labelRect.Width()  + labelRect.Width()/4;
			rc.bottom = rc.top + labelRect.Height() ;

			CPen highlightPen;
			highlightPen.CreatePen(PS_SOLID,1,::GetSysColor( COLOR_HIGHLIGHT ) );
			pDC.SelectObject( &highlightPen);
			
			CBrush highlightBrush( ::GetSysColor( COLOR_HIGHLIGHT ) );

			if( pCustomTree->GetFocus( ) == pCustomTree->GetParent() )
			{
				 pOldBrush = pDC.SelectObject( &highlightBrush);
			}
			else
			{
				pDC.SetTextColor( cItemColor );			
			}				 
			pCustomTree->ScreenToClient( &rc );
			pDC.Rectangle(rc);

			pCustomTree->ScreenToClient( &labelRect );

			pDC.SetBkMode(TRANSPARENT);

			bool bIgnoreDrawText = false;
			if( pCustomTree->GetEditControl( ) )
			{
					bIgnoreDrawText = true;
			}

			if( !bIgnoreDrawText )
			{
				CString s = "****";
				s += sItem;
					labelRect.right +=labelRect.Width()/4;
				pDC.DrawText( sItem,labelRect,DT_SINGLELINE | DT_LEFT | DT_VCENTER);
			}
			pDC.SelectObject( &pOldpen);
			pDC.SelectObject( &pOldBrush);

	}
	else
	{
		labelRect.right +=labelRect.Width()/4;
		CString s = "+++";
		s += sItem;
		pDC.DrawText( sItem,labelRect,DT_SINGLELINE | DT_LEFT | DT_VCENTER);		
	}
	
	return 0;
}

int CCheckboxesTreeCtrl::ExpandItem( LOOPINFO* pLoopInfo )
{
	if( !pLoopInfo->hItem )
		return -1;

	CCheckboxesTreeCtrl* pCustomTree = pLoopInfo->pTree;

	bool bExpand = false;
	if( pLoopInfo->m_param1 )
		bExpand = * ( (bool*)(pLoopInfo->m_param1) );

	if( bExpand )
	{
		HTREEITEM hItem = GetRootItem();
		while (NULL != hItem)
		{
			this->Expand(hItem, TVE_EXPAND);
			hItem = GetNextItemSmart(hItem);
		}
	}
	else
	{
		HTREEITEM hItem = GetRootItem();
		while (NULL != hItem)
		{
			this->Expand(hItem, TVE_COLLAPSE);
			hItem = GetNextItemSmart(hItem);
		}
	}
	return 0;
}


//DESCRIPTION:
//		To set the background color for the tree control
//PARAM
//		bkColor[IN]	-	New background color to be set
void CCheckboxesTreeCtrl::SetBackgroundColor( COLORREF bkColor )
{
	TreeView_SetBkColor( GetSafeHwnd(),bkColor );    
}


//DESCRIPTION:
//		To set the bold font for the label and expanded
//PARAM
//		hItem[in]	-	Item to which the font to be set as bold
void CCheckboxesTreeCtrl::SetBoldFont(HTREEITEM hItem )
{
	if( !hItem )
		return;
	SetItemState( hItem,TVIS_BOLD,TVIS_BOLD );
	Expand(hItem,TVE_EXPAND); 
}

//DESCRIPTION:
//		To set the color for the item text
//PARAM
//		hItem[in]	-	Item to which the color to be set
//		col[in]		-	New color of the item

void CCheckboxesTreeCtrl::SetItemColor( HTREEITEM hItem,	COLORREF col )
{
	if( !hItem )
		return;

	//Get the item data
	PCUSTOMITEMDATA pCustomData  = (PCUSTOMITEMDATA) CTreeCtrl::GetItemData( hItem );
	if( pCustomData )
		pCustomData->m_cItemColor = col;
}


//DESCRIPTION:
//			To enable/disable the item
//PARAM
//		hItem[in]		-	Item to be enabled/disabled
//		bEnable[in]		-	True for enable,false for disable
void	CCheckboxesTreeCtrl::EnableItem( HTREEITEM hItem,bool	bEnable	)
{
	if( !hItem )
		return;
	
	PCUSTOMITEMDATA pCustomData  = (PCUSTOMITEMDATA) CTreeCtrl::GetItemData( hItem );
	if( pCustomData->m_bEnable == bEnable )
		return;

	pCustomData->m_bEnable = bEnable;

	if( !bEnable )
	{
		//pCustomData->m_bExpStateBefDisable =(  GetItemState( hItem, TVIS_EXPANDED ) & TVIS_EXPANDED );
		Expand( hItem,TVE_COLLAPSE );
		
		if( pCustomData->m_iDisableIndex == -1 )
		{
      std::map<UINT, std::vector<int> >::iterator it = m_mapBitmapID.find(this->m_nNumberOfCheckboxes);

			int normalImgIndex,selImageIndex;
			GetItemImage(hItem,normalImgIndex,selImageIndex);

			CBitmap disableBmp;
			bool res = GetBitampFromImageList( normalImgIndex,disableBmp );

			GenerateDisableImage( disableBmp);

			int index = m_pImgList->Add( &disableBmp,m_cMaskColor );
			pCustomData->m_iDisableIndex = index;		
			it->second.push_back( index );
		}		
	}
	else
	{
		if( pCustomData->m_bExpStateBefDisable )
		{
			Expand(hItem,TVE_EXPAND);
		}				
	}

	SetImage( hItem );
}


//DESCRIPTION:
//		To expand the given item [ All the items under the given items are expanded]
//PARAM
//		hItem[in]	-	Item to be expanded
void CCheckboxesTreeCtrl::ExpandTree(HTREEITEM hItem,bool bExpand /*= true*/ )
{
  HTREEITEM hStart =	GetRootItem(); 
	if( !hItem )
		hItem = hStart;

	m_bContinueScan = true;
	m_bStartFound = false;

  LOOPINFO lInfo;
	lInfo.pTree =  this;
	lInfo.hItem = hItem;
	lInfo.pParent = GetParent();
	lInfo.m_param1   = &bExpand;
  lInfo.m_nNumberOfCheckboxes = m_nNumberOfCheckboxes;
  ExpandItem(&lInfo);
}

void CCheckboxesTreeCtrl::ExpandTreeRoot(bool bExpand/*=true*/)
{
  //fix bug, Tree not expand
  SendMessage(WM_LBUTTONDOWN); //Programmatically set focus to control
  SendMessage(WM_LBUTTONUP);   //otherwise tree not expanded
  //
  HTREEITEM hItem = GetRootItem();
  while (NULL != hItem)
  {
    this->Expand(hItem, bExpand?TVE_EXPAND:TVE_COLLAPSE);
    hItem = GetNextItemSmart(hItem);
  }
}

CWnd* CCheckboxesTreeCtrl::SetFocus()
{
  SendMessage(WM_LBUTTONDOWN); //Programmatically set focus to control
  SendMessage(WM_LBUTTONUP);   
  return CTreeCtrl::SetFocus();
}



//DESCRIPTION:
//		Called when the user starts its label editing
//PARAM
//		pNMHDR[in]		-	Contains the data about the item edited
//		pResult[in/out]	-	Result of the edition
void CCheckboxesTreeCtrl::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	HTREEITEM hItem = pTVDispInfo->item.hItem;

	if( hItem )
	{
		PCUSTOMITEMDATA pCustomData  = (PCUSTOMITEMDATA) CTreeCtrl::GetItemData( hItem );

		if( pCustomData )
		{
			if( !pCustomData->m_bEditable || !pCustomData->m_bEnable )
			{
	   		  *pResult = 1;
				return;
			} 
		}
	}
	Invalidate( );
	
	*pResult = 0;
}

//DESCRIPTION:
//		Called when the user completes its label editing
//PARAM
//		pNMHDR[in]		-	Contains the data about the item edited
//		pResult[in/out]	-	Result of the edition
void CCheckboxesTreeCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	if( pTVDispInfo->item.pszText != NULL)
	{
		SetItemText( pTVDispInfo->item.hItem,pTVDispInfo->item.pszText);
		m_vecSelectedItems.clear( );
		m_vecSelectedItems.push_back( pTVDispInfo->item.hItem );
		GetParent()->SendMessage( TVN_SELECTION_CHANGED,(WPARAM)(&m_vecSelectedItems),NULL ); 
	}

	*pResult = 0;
}

//DESCRIPTION:
//		To insert a item in the tree using the custom data
//PARAM
//		lpInsertStruct[in]		-	Custom data of item to be inserted
HTREEITEM CCheckboxesTreeCtrl::InsertItem(PCUSTOMINSERTSTRUCT lpInsertStruct )
{
	int res = AddImageToList( lpInsertStruct );	

	HTREEITEM hItem = CTreeCtrl::InsertItem( &lpInsertStruct->m_tvIS );
  if(NULL != hItem)
  {
    CTreeCtrl::SetItemData( hItem,(DWORD)(lpInsertStruct->m_pCustomData));
    structData strTmp;
    strTmp.hItem = hItem;
    strTmp.bEmptyChild = FALSE;
    m_arrStructData.Add(strTmp);
  }

	SetImage(hItem);
	return hItem;
}


HTREEITEM CCheckboxesTreeCtrl::InsertItem(UINT nMask, LPCTSTR lpszItem, int nImage, int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
  DeleteEmptyItem(hParent);
  PCUSTOMINSERTSTRUCT lpInsertStruct = new CUSTOMINSERTSTRUCT;
  lpInsertStruct->m_tvIS.item.pszText = (LPSTR)(LPCTSTR)lpszItem;
  lpInsertStruct->m_tvIS.hParent      = hParent;
  lpInsertStruct->m_tvIS.hInsertAfter = hInsertAfter;
  PCUSTOMITEMDATA pCustomData = new CUSTOMITEMDATA;
  lpInsertStruct->m_pCustomData = pCustomData;
  AddImageToList( lpInsertStruct );	

  HTREEITEM hItem = CTreeCtrl::InsertItem(nMask, lpszItem, nImage, nSelectedImage, nState, nStateMask, lParam, hParent, hInsertAfter);
  if(NULL != hItem)
  {
    CTreeCtrl::SetItemData( hItem,(DWORD)(lpInsertStruct->m_pCustomData));
    structData strTmp;
    strTmp.hItem = hItem;
    strTmp.bEmptyChild = FALSE;
    m_arrStructData.Add(strTmp);
  }
  structItemsTree structitemsTree;
  structitemsTree.hItem = hItem;
  structitemsTree.pCustomData = &pCustomData;
  structitemsTree.lpInsertStruct = &lpInsertStruct;
  m_arrStructItemsTree.Add(structitemsTree);
  SetImage(hItem);
  return hItem;
}

HTREEITEM CCheckboxesTreeCtrl::InsertItem(LPCTSTR lpszItem, CTreeItem tiItem, HTREEITEM hParent /*= TVI_ROOT*/, HTREEITEM hInsertAfter /*= TVI_LAST*/, BOOL bEmptyChild /*=FALSE*/)
{
  DeleteEmptyItem(hParent);
  PCUSTOMINSERTSTRUCT lpInsertStruct = new CUSTOMINSERTSTRUCT;
  lpInsertStruct->m_tvIS.item.pszText = (LPSTR)(LPCTSTR)lpszItem;
  lpInsertStruct->m_tvIS.hParent      = hParent;
  lpInsertStruct->m_tvIS.hInsertAfter = hInsertAfter;
  PCUSTOMITEMDATA pCustomData = new CUSTOMITEMDATA;
  lpInsertStruct->m_pCustomData = pCustomData;
  lpInsertStruct->m_pCustomData->m_llCode = tiItem.m_llCode;
  lpInsertStruct->m_pCustomData->m_ll_ID = tiItem.m_ID;
  lpInsertStruct->m_pCustomData->m_bEnable;
  if ("N" == tiItem.m_strActive)
  {
    lpInsertStruct->m_pCustomData->m_bEnable = FALSE;
  }
  AddImageToList( lpInsertStruct );
  HTREEITEM hItem = CTreeCtrl::InsertItem(lpszItem, hParent, hInsertAfter);
  if(NULL != hItem)
  {
    CTreeCtrl::SetItemData( hItem,(DWORD)(lpInsertStruct->m_pCustomData));
    structData strTmp;
    strTmp.hItem = hItem;
    strTmp.bEmptyChild = FALSE;
    m_arrStructData.Add(strTmp);
    if (TRUE == bEmptyChild)
    {
      AddEmptyChild(hItem);
    }
  }
  structItemsTree structitemsTree;
  structitemsTree.hItem = hItem;
  structitemsTree.llData = tiItem.m_ID;
  structitemsTree.pCustomData = &pCustomData;
  structitemsTree.lpInsertStruct = &lpInsertStruct;
  m_arrStructItemsTree.Add(structitemsTree);
  SetImage(hItem);
  return hItem;
}

HTREEITEM CCheckboxesTreeCtrl::InsertItem(LPCTSTR lpszItem, int nImage, int nSelectedImage, HTREEITEM hParent /*= TVI_ROOT*/, HTREEITEM hInsertAfter /*= TVI_LAST*/)
{
  DeleteEmptyItem(hParent);
  PCUSTOMINSERTSTRUCT lpInsertStruct = new CUSTOMINSERTSTRUCT;
  lpInsertStruct->m_tvIS.item.pszText = (LPSTR)(LPCTSTR)lpszItem;
  lpInsertStruct->m_tvIS.hParent      = hParent;
  lpInsertStruct->m_tvIS.hInsertAfter = hInsertAfter;
  PCUSTOMITEMDATA pCustomData = new CUSTOMITEMDATA;
  lpInsertStruct->m_pCustomData = pCustomData;
  AddImageToList( lpInsertStruct );

  HTREEITEM hItem = CTreeCtrl::InsertItem(lpszItem, nImage, nSelectedImage, hParent, hInsertAfter);
  if(NULL != hItem)
  {
    CTreeCtrl::SetItemData( hItem,(DWORD)(lpInsertStruct->m_pCustomData));
    structData strTmp;
    strTmp.hItem = hItem;
    strTmp.bEmptyChild = FALSE;
    m_arrStructData.Add(strTmp);
  }
  structItemsTree structitemsTree;
  structitemsTree.hItem = hItem;
  structitemsTree.pCustomData = &pCustomData;
  structitemsTree.lpInsertStruct = &lpInsertStruct;
  m_arrStructItemsTree.Add(structitemsTree);
  SetImage(hItem);
  return hItem;
}


void CCheckboxesTreeCtrl::AddEmptyChild(HTREEITEM hItem)
{
  HTREEITEM hItemChild = CTreeCtrl::InsertItem("", hItem);
  if(NULL != hItemChild)
  {
    structData strTmp;
    strTmp.hItem = hItemChild;
    strTmp.bEmptyChild = TRUE;
    m_arrStructData.Add(strTmp);
  }
}


void CCheckboxesTreeCtrl::DeleteEmptyItem(HTREEITEM hItem)
{
  HTREEITEM hItemChild = CTreeCtrl::GetChildItem(hItem);
  if (NULL != hItemChild)
  {
    for(int i = 0; i < m_arrStructData.GetSize(); i++)
    {
      if((hItemChild == m_arrStructData.GetAt(i).hItem)&&
        (TRUE == m_arrStructData[i].bEmptyChild))
      {
        DeleteItem(hItemChild);
        break;
      }
    }
  }
}

BOOL CCheckboxesTreeCtrl::DeleteAllItems()
{
  CTreeCtrl::SelectItem(NULL);
  BOOL bDel = CTreeCtrl::DeleteAllItems();
  if (TRUE == bDel)
  {
    m_ArrayDesc.RemoveAll();
    m_arrStructData.RemoveAll();
    for (int i = m_arrStructItemsTree.GetSize()-1; i >= 0 ; i-- )
    {
      m_arrStructItemsTree.GetAt(i).lpInsertStruct = NULL;
      delete m_arrStructItemsTree.GetAt(i).lpInsertStruct;
      m_arrStructItemsTree.GetAt(i).pCustomData = NULL;;
      delete m_arrStructItemsTree.GetAt(i).pCustomData;
      m_arrStructItemsTree.RemoveAt(i);
    }
    m_arrStructItemsTree.RemoveAll();
  }
  return bDel;
}


//DESCRIPTION:
///		Called when the item is expanding
//PARAM
///		pNMHDR[in]		-	Contains the data about the item edited
///		pResult[in/out]	-	Result of the edition
void CCheckboxesTreeCtrl::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	//If the click is on the checkbox then dont allow expand/collpase
	if( !m_bAllowExpand )
	{
		*pResult = 1;
    
		CUSTNMHDR chdr;
		chdr.m_hdr.hwndFrom	=  m_hWnd;
		chdr.m_hdr.idFrom		=  ::GetDlgCtrlID(m_hWnd);
		chdr.m_hdr.code			=  TVN_STATEICON_CLICKED;
		chdr.m_hItem					=  pNMTreeView->itemNew.hItem;
		chdr.m_data 					=  (PCUSTOMITEMDATA) CTreeCtrl::GetItemData(pNMTreeView->itemNew.hItem);
		SendMessage(TVN_STATEICON_CLICKED, (WPARAM)&chdr, (LPARAM)chdr.m_hdr.idFrom );			
		return;
	}

	//Get the item data
	PCUSTOMITEMDATA pData = (PCUSTOMITEMDATA) CTreeCtrl::GetItemData(pNMTreeView->itemNew.hItem);

  //////////////////////////////////////////////////////// 
  /*
	//Similarly if the item is disabled dont allow expand
	if( !pData->m_bEnable && pNMTreeView->action == TVE_EXPAND)
	{
		*pResult = 1;
		return;
	}*/

	bool bItemExpanded = false;
	if( pNMTreeView->action == TVE_EXPAND )
		bItemExpanded = true;


	//Set image based on expand/collpase state
  HTREEITEM hSelectedItem = GetSelectedItem();
  if ( NULL != hSelectedItem)
	  SetItemState(hSelectedItem, 0, UINT(TVIS_SELECTED));

  UpdateWindow();
	*pResult = 0;
}

//DESCRIPTION:
///		To copy an item to another item
//PARAM
///		hSourceItem[in]		-	Item to be copied
///		hTargetItem[in]			-	Target where the source to be copied
///		bMoveSibling[in]		-	Whether the sibling has to be moved or not
void CCheckboxesTreeCtrl::CopyItem(HTREEITEM hSourceItem,HTREEITEM hTargetItem,bool bMoveSibling /*=false*/)
{
	if( !hSourceItem || !hTargetItem )
		return;

	if( hSourceItem == hTargetItem )
		return;

		//Get the info about the child item
		TVITEM tvItem;
		tvItem.mask = TVIF_IMAGE | TVIF_STATE | TVIF_PARAM | TVIF_SELECTEDIMAGE | TVIF_TEXT |  TVIF_HANDLE;

		char buf[128];
		tvItem.pszText = buf;
		tvItem.cchTextMax = 128;
		tvItem.hItem = hSourceItem;
	
		GetItem(&tvItem);

		CUSTOMINSERTSTRUCT tvIS;
		tvIS.m_tvIS.hParent			=	hTargetItem;
		tvIS.m_tvIS.hInsertAfter	=	TVI_LAST; 
		tvIS.m_tvIS.item					 =	tvItem;

		//Get the item data of the hSourceItem
		PCUSTOMITEMDATA pData = (PCUSTOMITEMDATA) CTreeCtrl::GetItemData(hSourceItem);
		tvIS.m_pCustomData    = pData;

		CString str1 = GetItemText( hSourceItem );
		CString str2 = GetItemText( hTargetItem );
		
		HTREEITEM hInsertedItem     =	InsertItem(&tvIS);

		if( GetItemState( hSourceItem,TVIS_SELECTED ) & TVIS_SELECTED )
		{
			SetItemState( hInsertedItem,TVIS_SELECTED,TVIS_SELECTED );

			//Add the inserted item to the m_vecSelectedItems as well
			m_vecSelectedItems.push_back(hInsertedItem);
		}

		//This is quite important.Say When source item becomes the parent to the target item
		//the copy will be called recursively.Comment this line and try to drag a item onto its
		//one of the child item.
		if( hSourceItem == m_hOrigTgt )
			return;

		HTREEITEM hChildItem, hSiblingItem ;
		hChildItem = GetChildItem(hSourceItem);
		while( hChildItem )
		{
			//Get the sibling of the given child item
			hSiblingItem = GetNextSiblingItem( hChildItem );

			str1 = GetItemText( hChildItem );
			str2 = GetItemText( hSiblingItem );

			CopyItem( hChildItem, hInsertedItem );
			hChildItem = hSiblingItem;
				
		} 

}

//DESCRIPTION:
//		To delete an items and all its children recursively and remove the item to 
//		be deleted from the vector holding the selected items and the vector holding
//	  the checked items
//PARAM
//		hTreeItem[in]	-	Item to be deleted
void CCheckboxesTreeCtrl::DeleteItem( HTREEITEM hTreeItem )
{
	if( !hTreeItem )
		return;

	//If it is there in selected items array then remove it
	size_t iSelItemCnt;
	for( iSelItemCnt = 0; iSelItemCnt < m_vecSelectedItems.size(); iSelItemCnt++ )
	{
		if( m_vecSelectedItems[iSelItemCnt] == hTreeItem )
		{
			m_vecSelectedItems.erase( m_vecSelectedItems.begin( ) + iSelItemCnt );

			//Inform the dialog
			GetParent()->SendMessage( TVN_SELECTION_CHANGED,(WPARAM)(&m_vecSelectedItems),NULL ); 
		}
	}

	//Similarly from the checked items array
	std::set<HTREEITEM>::iterator it = m_vecCheckedItems.find( hTreeItem );
	if( it != m_vecCheckedItems.end( ) )
	{
		m_vecCheckedItems.erase( it );

		//Inform the dialog
		GetParent()->SendMessage( TVN_ITEM_CHECK_TOGGLE,(WPARAM)(&m_vecCheckedItems),NULL ); 
	}
	CTreeCtrl::DeleteItem( hTreeItem );
}

void CCheckboxesTreeCtrl::DeleteChildItem( HTREEITEM hItem )
{
	if( !hItem )
		return;
  HTREEITEM hTreeItem = this->GetNextItem(hItem, TVGN_CHILD);

	//If it is there in selected items array then remove it
	size_t iSelItemCnt;
	for( iSelItemCnt = 0; iSelItemCnt < m_vecSelectedItems.size(); iSelItemCnt++ )
	{
		if( m_vecSelectedItems[iSelItemCnt] == hTreeItem )
		{
			m_vecSelectedItems.erase( m_vecSelectedItems.begin( ) + iSelItemCnt );

			//Inform the dialog
			GetParent()->SendMessage( TVN_SELECTION_CHANGED,(WPARAM)(&m_vecSelectedItems),NULL ); 
		}
	}

	//Similarly from the checked items array
	std::set<HTREEITEM>::iterator it = m_vecCheckedItems.find( hTreeItem );
	if( it != m_vecCheckedItems.end( ) )
	{
		m_vecCheckedItems.erase( it );

		//Inform the dialog
		GetParent()->SendMessage( TVN_ITEM_CHECK_TOGGLE,(WPARAM)(&m_vecCheckedItems),NULL ); 
	}
	CTreeCtrl::DeleteItem( hTreeItem );
}


void CCheckboxesTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	TV_HITTESTINFO tvHitInfo;
	tvHitInfo.pt = point;
	HTREEITEM hClickedItem = HitTest(&tvHitInfo );

	m_bAllowExpand = true;

	if(  hClickedItem )
	{
		PCUSTOMITEMDATA pCustData = (PCUSTOMITEMDATA) CTreeCtrl::GetItemData(hClickedItem);

    if (NULL == pCustData->m_ll_ID)
      int aa=2;

		if( pCustData && pCustData->m_bChecked && pCustData->m_bEnable && NULL != nFlags && NULL != point.x && NULL != point.y)
		{			
			if( tvHitInfo.flags & TVHT_ONITEMICON )
			{
				CPoint pt = point;
				ClientToScreen(&pt );
				CRect fullRect;
				GetItemRect( hClickedItem,&fullRect,FALSE );
				ClientToScreen( &fullRect );
				CRect labelRect;
				GetItemRect( hClickedItem,&labelRect,TRUE );
				ClientToScreen( &labelRect );

				CRect imgRect;
				imgRect.left   = fullRect.left;
				imgRect.top    = fullRect.top;
				imgRect.right  = imgRect.left + ( labelRect.left - fullRect.left );
				imgRect.bottom = fullRect.bottom;

				int imgW,imgH;
				ImageList_GetIconSize( m_pImgList->GetSafeHandle(),&imgW,&imgH);
                imgRect.right -= (m_nNumberOfCheckboxes-1)*(imgW/m_nNumberOfCheckboxes);

				//Now check the click point is there in this rect. If so the click is on the
				//check box of the item
                //first checkbox
				if( imgRect.PtInRect( pt ))
				{ //ExpandTreeBranch() not works after send this messege
					m_bAllowExpand = false;
					CUSTNMHDR chdr;
					chdr.m_hdr.hwndFrom	=  m_hWnd;
					chdr.m_hdr.idFrom		=  ::GetDlgCtrlID(m_hWnd);
					chdr.m_hdr.code			=  TVN_ITEM_CHECK_ONE;
					chdr.m_hItem					=  hClickedItem;
					chdr.m_data 					=  pCustData;
					SelectFocusItem(hClickedItem);
					SendMessage(TVN_ITEM_CHECK_ONE, (WPARAM)&chdr, (LPARAM)chdr.m_hdr.idFrom );		
		      /*
		            pCustData->m_bCheckState1 = !pCustData->m_bCheckState1;	
				    SetImage(hClickedItem );
		            UpdateWindow();
		         
		            NM_TREEVIEW nmtvMessage;
		            nmtvMessage.hdr.hwndFrom = m_hWnd;
		            nmtvMessage.hdr.idFrom   = GetDlgCtrlID();
			        nmtvMessage.hdr.code     = TVN_ITEM_CHECK_ONE;
			        nmtvMessage.itemNew.hItem = hClickedItem;
		          
		            CWnd* pWnd = GetOwner();
		            CTreeCtrl::OnLButtonDown(nFlags, point);
					if( pWnd && pWnd->m_hWnd )
		              pWnd->SendMessage(WM_NOTIFY, nmtvMessage.hdr.idFrom, (LPARAM)&nmtvMessage);*/
					return;					
				}
				//secund checkbox
        imgRect.right += imgW/m_nNumberOfCheckboxes;
        if( imgRect.PtInRect( pt ) && m_nNumberOfCheckboxes > 1)
				{ //ExpandTreeBranch() not works after send this messege									
					m_bAllowExpand = false;
					CUSTNMHDR chdr;
					chdr.m_hdr.hwndFrom	=  m_hWnd;
					chdr.m_hdr.idFrom		=  ::GetDlgCtrlID(m_hWnd);
					chdr.m_hdr.code			=  TVN_ITEM_CHECK_TWO;
					chdr.m_hItem					=  hClickedItem;
					chdr.m_data 					=  pCustData;
				SelectFocusItem(hClickedItem);
					SendMessage(TVN_ITEM_CHECK_TWO, (WPARAM)&chdr, (LPARAM)chdr.m_hdr.idFrom );		
          /*
			        pCustData->m_bCheckState2 = !pCustData->m_bCheckState2;	
				    SetImage( hClickedItem );
			
			        NM_TREEVIEW nmtvMessage;
			        nmtvMessage.hdr.hwndFrom = m_hWnd;
			        nmtvMessage.hdr.idFrom   = GetDlgCtrlID();
				    nmtvMessage.hdr.code     = TVN_ITEM_CHECK_TWO;
				    nmtvMessage.itemNew.hItem = hClickedItem;
			
			        CWnd* pWnd = GetOwner();
			        CTreeCtrl::OnLButtonDown(nFlags, point);
					if( pWnd && pWnd->m_hWnd )
			          pWnd->SendMessage(WM_NOTIFY, nmtvMessage.hdr.idFrom, (LPARAM)&nmtvMessage);	*/
					return;
				}
        //three checkbox
        imgRect.right += imgW/m_nNumberOfCheckboxes;
        if( imgRect.PtInRect( pt ) && m_nNumberOfCheckboxes > 2 )
				{/* //ExpandTreeBranch() not works after send this messege					
					m_bAllowExpand = false;
					CUSTNMHDR chdr;
					chdr.m_hdr.hwndFrom	=  m_hWnd;
					chdr.m_hdr.idFrom		=  ::GetDlgCtrlID(m_hWnd);
					chdr.m_hdr.code			=  TVN_ITEM_CHECK_THREE;
					chdr.m_hItem					=  hClickedItem;
					chdr.m_data 					=  pCustData;
					SendMessage(TVN_ITEM_CHECK_THREE, (WPARAM)&chdr, (LPARAM)chdr.m_hdr.idFrom );	*/

		            pCustData->m_bCheckState3 = !pCustData->m_bCheckState3;	
		            SetImage( hClickedItem );
		
		            NM_TREEVIEW nmtvMessage;
		            nmtvMessage.hdr.hwndFrom = m_hWnd;
		            nmtvMessage.hdr.idFrom   = GetDlgCtrlID();
			        nmtvMessage.hdr.code     = TVN_ITEM_CHECK_THREE;
			        nmtvMessage.itemNew.hItem = hClickedItem;
		
		            CWnd* pWnd = GetOwner();
		            CTreeCtrl::OnLButtonDown(nFlags, point);
					if( pWnd && pWnd->m_hWnd )
		              pWnd->SendMessage(WM_NOTIFY, nmtvMessage.hdr.idFrom, (LPARAM)&nmtvMessage);
		            
					return;
				}
        //four checkbox
        imgRect.right += imgW/m_nNumberOfCheckboxes;
        if( imgRect.PtInRect( pt ) && m_nNumberOfCheckboxes > 3 )
				{/* //ExpandTreeBranch() not works after send this messege											
					m_bAllowExpand = false;
					CUSTNMHDR chdr;
					chdr.m_hdr.hwndFrom	=  m_hWnd;
					chdr.m_hdr.idFrom		=  ::GetDlgCtrlID(m_hWnd);
					chdr.m_hdr.code			=  TVN_ITEM_CHECK_FOUR;
					chdr.m_hItem					=  hClickedItem;
					chdr.m_data 					=  pCustData;
					SendMessage(TVN_ITEM_CHECK_FOUR, (WPARAM)&chdr, (LPARAM)chdr.m_hdr.idFrom );		*/
          
		            pCustData->m_bCheckState4 = !pCustData->m_bCheckState4;	
		            SetImage( hClickedItem );
		
		            NM_TREEVIEW nmtvMessage;
		            nmtvMessage.hdr.hwndFrom = m_hWnd;
		            nmtvMessage.hdr.idFrom   = GetDlgCtrlID();
			        nmtvMessage.hdr.code     = TVN_ITEM_CHECK_FOUR;
			        nmtvMessage.itemNew.hItem = hClickedItem;
		
		            CWnd* pWnd = GetOwner();
		            CTreeCtrl::OnLButtonDown(nFlags, point);
					if( pWnd && pWnd->m_hWnd )
		              pWnd->SendMessage(WM_NOTIFY, nmtvMessage.hdr.idFrom, (LPARAM)&nmtvMessage);	
					
		            return;
				}
			}		
		}
	}

  //loop functions and consequently display stack overflow error 
  //SetFocus(); 
  CTreeCtrl::SetFocus();

	//If the click is not on the item and not on the item button means enable the
	//banding
	if( !(tvHitInfo.flags & TVHT_ONITEM) && !(tvHitInfo.flags & TVHT_ONITEMBUTTON) )
	{
		if( GetEditControl( ) )
		{
			CTreeCtrl::OnLButtonDown(nFlags, point);
			return;
		}

		//if there is no selected item then ON the banding
		m_bIsBandingON = true;

		//Store the starting point
		m_startPt = m_endPt = point;

		//Remove the selection
        size_t i = 0;
		for( ; i < m_vecSelectedItems.size(); i++ )
		{
			HTREEITEM hItem	=	m_vecSelectedItems[i];
			SetItemState( m_vecSelectedItems[i],~TVIS_SELECTED,TVIS_SELECTED );
		}

		//This is important.If you dont select the NULL item, the GetSelectedItem will
		//always return the previous selected item.For example,select item1.Click the mouse
		//button somewhere on the tree and not on any of the tree item. So in previous loop
		//we have removed the selection flag of item1 [ highlighting of the items goes].
		//Now again click on item1.But the tree gives the item1 as selected [eventhough it is
		//not highlighted ]item1 and because of this we are not able to select item1.So make
		//forcefully that no item is selected by calling SelectItem(NULL ).
		SelectItem( NULL );
			
		//Send the notification to the parent dialog about the selected items.
		//Collect the selected items. May be parent may seek this data.
		CollectSelectedItems();
		GetParent()->SendMessage( TVN_SELECTION_CHANGED,(WPARAM)(&m_vecSelectedItems),NULL ); 
		SetCapture();
		return;		
	}

	
	//Capture the mouse
	SetCapture();
	Invalidate();
	CTreeCtrl::OnLButtonDown(nFlags, point);
}

void CCheckboxesTreeCtrl::ShowItemInfo(HTREEITEM hItem )
{
	if( !hItem )
		return;
	CString str = GetItemText( hItem );

	TVITEM infoItem;
	infoItem.hItem = hItem;
	infoItem.mask = TVIF_TEXT ;

	char label[128];
	infoItem.pszText = label;
	infoItem.cchTextMax = 128;
	GetItem( &infoItem );
	MessageBox( infoItem.pszText);
}

void CCheckboxesTreeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
  if (!m_bMouseOver)
  {
    m_bMouseOver = TRUE;
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof (tme);
    tme.dwFlags = TME_HOVER | TME_LEAVE;
    tme.hwndTrack = m_hWnd;
    tme.dwHoverTime = 0;
    _TrackMouseEvent (&tme);
  } 

	CTreeCtrl::OnMouseMove(nFlags, point);
}

void CCheckboxesTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	//if the banding is ON then remove the banding flag and rectangle drawn
	if( m_bIsBandingON )
	{
		m_bIsBandingON = false;
		CClientDC dc(this);
		InvertRectangle( &dc,m_startPt,m_endPt );

		if( m_startPt != m_endPt )
		{
				CRect rect;
			if( m_startPt.y < m_endPt.y )
			{
				rect.left = m_startPt.x;
				rect.top  = m_startPt.y;
				rect.right = m_endPt.x;
				rect.bottom = m_endPt.y;
				
			}
			else
			{
				rect.left = m_endPt.x;
				rect.top  = m_endPt.y;
				rect.right = m_startPt.x;
				rect.bottom = m_startPt.y;				

			}
			//Find the items inside the rectangle
			ItemsInRect( rect );
		}	
	}
	
	if( m_pDragImageList )
			m_pDragImageList->DragShowNolock(FALSE );
				
	SelectDropTarget(NULL);

	//Release the mouse
	ReleaseCapture();
	ShowCursor( true );
	CTreeCtrl::OnLButtonUp(nFlags, point);
}


//DESCRIPTION:
///			To invert the rectangle being drawn
//PARAM
///		pDC[in]		-	Device context used for drawing
///		startPt[in]	-	Start point of the rectangle
///		endPt[in]	-	End point of the rectangle
void	CCheckboxesTreeCtrl::InvertRectangle( CDC* pDC,CPoint startPt,CPoint endPt )	
{
	CGdiObject* pOldBrush = pDC->SelectStockObject(NULL_BRUSH );

	CPen pen (PS_DOT, 1, RGB (49, 106, 197));
	CPen* pOldPen = pDC->SelectObject (&pen);

	int nOldMode = pDC->SetROP2( R2_NOTXORPEN );
	pDC->Rectangle( startPt.x,startPt.y,endPt.x,endPt.y);
	pDC->SetROP2(nOldMode);

	pDC->SelectObject(pOldBrush );
	pDC->SelectObject (pOldPen);
}

//DESCRIPTION:
///			To find the items inside the given rectangle
//PARAM
///		rect[in]		-	Bounding rectangle area
bool	CCheckboxesTreeCtrl::ItemsInRect(CRect rect)
{
	//First deselect all the items
	bool bFlag = false;
  HTREEITEM hStart =	GetRootItem(); 
	m_bContinueScan = true;
	m_bStartFound = false;
  LOOPINFO lInfo;
  lInfo.pTree =  this;
	lInfo.hItem = hStart;
	lInfo.pParent = GetParent();
	lInfo.m_param1   = &bFlag;
  lInfo.m_nNumberOfCheckboxes = m_nNumberOfCheckboxes; 
  SelectItems( &lInfo );
	UpdateWindow();

	ClientToScreen(&rect);
	m_aItemsInRect.RemoveAll();

  hStart =	GetRootItem(); 
	m_bContinueScan = true;
	m_bStartFound = false;
  LOOPINFO lInfo2;
  lInfo2.pTree =  this;
	lInfo2.hItem = hStart;
	lInfo2.pParent = GetParent();
	lInfo2.m_param1   = &rect;
  lInfo2.m_nNumberOfCheckboxes = m_nNumberOfCheckboxes; 
  FindItemsInRect( &lInfo2 );

	int i = 0;
	for( ; i < m_aItemsInRect.GetSize(); i++ )
	{
		SetItemState( m_aItemsInRect[i],TVIS_SELECTED,TVIS_SELECTED);
	}
	//Send the notification to the parent dialog about the selected items
	CollectSelectedItems();
	GetParent()->SendMessage( TVN_SELECTION_CHANGED,(WPARAM)(&m_vecSelectedItems),NULL ); 

	return true;
}

//DESCRIPTION:
///			To get the first selected item in the tree
//PARAM
///		void
HTREEITEM CCheckboxesTreeCtrl::GetFirstSelectedItem() const 
{
	for(HTREEITEM hItem = GetRootItem(); hItem; hItem = GetNextVisibleItem(hItem))
		if(GetItemState(hItem, UINT(TVIS_SELECTED)) & TVIS_SELECTED)
			return hItem;
	return 0;
}

//DESCRIPTION:
///			To get the next selected item in the tree
//PARAM
///		void
HTREEITEM CCheckboxesTreeCtrl::GetNextSelectedItem(HTREEITEM hItem) const 
{
	if( !hItem )
		return 0;

	for( hItem = GetNextVisibleItem(hItem); hItem; hItem = GetNextVisibleItem(hItem))
	{
		if(GetItemState(hItem, UINT(TVIS_SELECTED)) & TVIS_SELECTED)
			return hItem;
	}
	return 0;
}

//DESCRIPTION:
///			To get the next item in the tree
//PARAM
///		hItem - prev item
HTREEITEM CCheckboxesTreeCtrl::GetNextItemSmart(HTREEITEM hItem)
{
  if( NULL == hItem )
    return NULL;
  
  HTREEITEM hItemReturn = NULL;

  if( NULL != GetNextItem(hItem, TVGN_CHILD) )
  {
    hItemReturn = GetNextItem(hItem, TVGN_CHILD);
  }
  else if( NULL != GetNextItem(hItem, TVGN_NEXT) )
  {
    hItemReturn = GetNextItem(hItem, TVGN_NEXT);
  }
  else 
  {
    while( NULL != GetNextItem(hItem, TVGN_PARENT) )
    {
      hItem = GetNextItem(hItem, TVGN_PARENT);
      if( NULL != GetNextItem(hItem, TVGN_NEXT) )
      {
        hItemReturn = GetNextItem(hItem, TVGN_NEXT);
        break;
      }
    }
  }

  return hItemReturn;
}

//DESCRIPTION:
///			To get the next item from tree based of item name
//PARAM
///		szText - item name
HTREEITEM CCheckboxesTreeCtrl::GetItemWithText(CString szText)
{
  HTREEITEM hItem = GetRootItem();
  while (NULL != hItem)
  {
    CString szTmp = this->GetItemText(hItem);
    if ( szText == szTmp )
    {
      return hItem;
    }
    hItem = GetNextItemSmart(hItem);
  }
  return NULL;
}

HTREEITEM CCheckboxesTreeCtrl::GetItemByData(LONGLONG llAAID)
{
  HTREEITEM hItem = NULL;
  for (int i = m_arrStructItemsTree.GetSize()-1; i >= 0 ; i-- )
  {
    if (m_arrStructItemsTree.GetAt(i).llData == llAAID)
    {
      hItem = m_arrStructItemsTree.GetAt(i).hItem;
      break;
    }
  }
  return hItem;
}

LONGLONG CCheckboxesTreeCtrl::GetItemDataID(HTREEITEM hItem)
{
  LONGLONG llData = NULL;
  for (int i = m_arrStructItemsTree.GetSize()-1; i >= 0 ; i-- )
  {
    if (m_arrStructItemsTree.GetAt(i).hItem == hItem)
    {
      llData = m_arrStructItemsTree.GetAt(i).llData;
      break;
    }
  }
  return llData;
}

HTREEITEM CCheckboxesTreeCtrl::AddFolder(CString& lpszItem, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
  HTREEITEM Item = InsertItem(lpszItem, 0, 5, hParent, hInsertAfter);      
  SetItemData(Item, 3);      

  return Item;
}


//DESCRIPTION:
///			To Collect the selected items in an array
void	CCheckboxesTreeCtrl::CollectSelectedItems()
{
	m_vecSelectedItems.clear();
	for(HTREEITEM hItem = GetFirstSelectedItem(); hItem != 0; hItem = GetNextSelectedItem(hItem)) 
	{
		m_vecSelectedItems.push_back(hItem);		
	}

}

//DESCRIPTION:
//			To generate the imagelist for the dragging. If multiple items are being dragged
//			we have to pile up all the image of the selected items as single drag image
CImageList* CCheckboxesTreeCtrl::GetImageListForDrag(std::vector<HTREEITEM>& cArrDragItems )
{
	//If there is no selected item for the drag return NULL
	CImageList* pNewImageList = NULL;

	if( cArrDragItems.size() == 0 )
		return NULL;

	//Get the imagelist of the tree
	CImageList* pTreeImgList = GetImageList(TVSIL_NORMAL );

	CClientDC dc(this);
	CDC		  cMemDC;
	CBitmap   cBitmap;

	int ImgWidth,ImgHeight;

	//Loop through all the selected items and get the size of the bitmap to be created
	size_t iDragItemCount = 0;
	CRect rc;
	for( ; iDragItemCount <  cArrDragItems.size(); iDragItemCount++ )
	{
		HTREEITEM hSelItem = cArrDragItems[iDragItemCount];
		CRect itemRect;
		GetItemRect( hSelItem ,&itemRect,false);

		CImageList* pImgList = GetImageList( TVSIL_NORMAL );
		ImageList_GetIconSize( pImgList->GetSafeHandle(),&ImgWidth,&ImgHeight );

		if( iDragItemCount == 0 )
		{
			rc.left = 0;
			rc.top  = 0;
			rc.right = ImgWidth + itemRect.Width();
			rc.bottom = ImgHeight;
		}
		else
		{
			if( rc.Width() < (itemRect.Width()+ImgWidth) )	
			{
                rc.right = itemRect.Width()+32;
			}
			rc.bottom += itemRect.Height();
		}
	}

	//Create the memory dc
	if(!cMemDC.CreateCompatibleDC(&dc))
		return NULL;

	//Create the bitmap of the required size
	if(!cBitmap.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height()))
		return NULL;

	CBitmap* pOldMemDCBitmap = cMemDC.SelectObject( &cBitmap );

	//Fill the bitmap with green color which is used as the mask
	cMemDC.FillSolidRect(0,0,rc.Width(), rc.Height(), RGB(0, 255, 0));

	CFont * pFontOld = cMemDC.SelectObject(GetFont());

	//Loop through all the selected items and get their image
	iDragItemCount = 0;
	CImageList* pSelImageList = NULL;
	CImageList * pImageList = GetImageList(TVSIL_NORMAL);

	CRect imgRect,textRect,itemRect;

	for( ; iDragItemCount <  cArrDragItems.size(); iDragItemCount++ )
	{
		HTREEITEM hSelItem = cArrDragItems[iDragItemCount];
		PCUSTOMITEMDATA pData = (PCUSTOMITEMDATA) GetItemData(hSelItem);

		CString str;
		str = GetItemText( hSelItem );

	    GetItemRect( hSelItem,&itemRect,true ); 

		if( iDragItemCount == 0 )
		{
			imgRect.left = 0;
			imgRect.top  = 0;
			imgRect.right = ImgWidth;
			imgRect.bottom = ImgHeight;

			//If the item has check box 
			if( pData->m_bChecked )
			{
				textRect.left = ImgWidth;				
			}
			else
			{
				//Left shift the textRect by half the width of the imageRect width as 
				//there is an empty space between the image of the item and the text
				textRect.left = ImgWidth/m_nNumberOfCheckboxes;
			}
			textRect.top  = 0;
			textRect.right = textRect.left  + itemRect.Width();
			textRect.bottom = itemRect.Height();
		}
		else
		{
			imgRect.top += ImgHeight;
			imgRect.bottom += ImgHeight;

			//If the item has check box 
			if( pData->m_bChecked )
			{
				textRect.left = ImgWidth;				
			}
			else
			{
				//Left shift the textRect by half the width of the imageRect width as 
				//there is an empty space between the image of the item and the text
				textRect.left = ImgWidth/m_nNumberOfCheckboxes;
			}
			textRect.top  = textRect.bottom;
			textRect.right = textRect.left + itemRect.Width();
			textRect.bottom = textRect.top + itemRect.Height();

		}
	
		if( hSelItem )
		{
			//Get the image of the selected item
			int nImg, nSelImg;
			GetItemImage(hSelItem,nImg,nSelImg);
			HICON hIcon = pImageList->ExtractIcon(nImg);
			
			//Draw this icon on to the memory dc
			::DrawIconEx(cMemDC.m_hDC, imgRect.left, imgRect.top, hIcon, ImgWidth, ImgHeight, 0, 0, DI_NORMAL);
		
			//Next draw the text
			cMemDC.DrawText(str, textRect, DT_LEFT| DT_VCENTER | DT_SINGLELINE|DT_NOPREFIX);
			DestroyIcon(hIcon);
							
		}
	}

	cMemDC.SelectObject( pOldMemDCBitmap );

	pNewImageList = new CImageList;
	BITMAP bm;
	cBitmap.GetBitmap(&bm);

	CBitmap bmp;
	pNewImageList->Create( bm.bmWidth,bm.bmHeight,ILC_COLOR32 | ILC_MASK,1,1);

	// Here green is used as mask color
	pNewImageList->Add(&cBitmap, RGB(0, 255, 0));

	return pNewImageList;
}

void CCheckboxesTreeCtrl::OnClickLButton(NMHDR* pNMHDR, LRESULT* pResult) 
{
	  *pResult = 0;
	  NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

		CPoint pt;
		::GetCursorPos(& pt );
		ScreenToClient(&pt );
		HTREEITEM hClickItem = HitTest(pt);

		if( !hClickItem )
			return;

	//Get the control and shift key states
		bool bCtrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
		bool bShift = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;
		
		HTREEITEM hSelItem = GetSelectedItem() ;

	if( bCtrl )
	{
		if( GetItemState( hClickItem,TVIS_SELECTED) & TVIS_SELECTED )
		{
			SetItemState( hClickItem,~TVIS_SELECTED,TVIS_SELECTED);
			*pResult = 1;
			CollectSelectedItems();
			GetParent()->SendMessage( TVN_SELECTION_CHANGED,(WPARAM)(&m_vecSelectedItems),NULL );
		}
	}
  else
  {
    ClearSelection();
   
    if (NULL != hClickItem)
    {
      AddSelected(hClickItem);
      SetItemState(hClickItem,TVIS_SELECTED,TVIS_SELECTED);
			SelectItem(hClickItem);
    }
    //when click on tree to expand item then loop functions and consequently display stack overflow error 
    //SetFocus();
    CTreeCtrl::SetFocus();
  }
  UpdateWindow();
}

void CCheckboxesTreeCtrl::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	//Get the old selected item and new item
	HTREEITEM hOldSelItem = pNMTreeView->itemOld.hItem;
	HTREEITEM hNewSelItem = pNMTreeView->itemNew.hItem;

	CString str1,str2;
	str1 = GetItemText( hOldSelItem );
	str2= GetItemText( hNewSelItem );

	//Get the action by which the selection is generated
	UINT& action = pNMTreeView->action;

	//Get the shift and control key state
	bool bCtrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
	bool bShift = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;

	if((action == TVC_BYMOUSE && bCtrl) ) 
	{
		//Select the old item
		if( hOldSelItem && m_bOldItemSelected )
		{
			SetItemState(hOldSelItem, UINT(TVIS_SELECTED), UINT(TVIS_SELECTED));			
		}

	}
	else if ( (action == TVC_BYKEYBOARD) && bShift )
	{
		if( hOldSelItem && m_bOldItemSelected )
		{
			SetItemState(hOldSelItem, UINT(TVIS_SELECTED), UINT(TVIS_SELECTED));
		}
	}
	else if( (action == TVC_BYMOUSE) && bShift )
	{
		bool bSelect = true;

		CRect rc1 ,rc2;
		GetItemRect(hOldSelItem,&rc1,true );
		GetItemRect(hNewSelItem,&rc2,true );

		if( rc2.top < rc1.top )
        {
          HTREEITEM hStart =	GetRootItem(); 
	      if( !hNewSelItem )
		    hNewSelItem = hStart;

          LOOPINFO lInfo;
		  lInfo.pTree =  this;
		  lInfo.hItem = hStart;
		  lInfo.pParent = GetParent();
		  lInfo.m_param1   = &bSelect;
          lInfo.m_nNumberOfCheckboxes = m_nNumberOfCheckboxes;

          int bRes = SelectItems(&lInfo);
		  if( bRes == 1 )
		  {
			  m_bContinueScan = false;
			  return;
		  }

		  if( hStart == hOldSelItem )
			  m_bStartFound = false;

		  HTREEITEM hNext;
		  hNext = GetNextSiblingItem( hStart );
		  hStart = hNext;
        }
		else
        {
          HTREEITEM hStart =	GetRootItem(); 
	      if( !hOldSelItem )
	        hOldSelItem = hStart;

          LOOPINFO lInfo;
		  lInfo.pTree =  this;
		  lInfo.hItem = hStart;
		  lInfo.pParent = GetParent();
		  lInfo.m_param1   = &bSelect;
          lInfo.m_nNumberOfCheckboxes = m_nNumberOfCheckboxes;

          int bRes = SelectItems(&lInfo);
		  if( bRes == 1 )
		  {
			  m_bContinueScan = false;
			  return;
		  }

		  if( hStart == hNewSelItem )
			  m_bStartFound = false;

		  HTREEITEM hNext;
		  hNext = GetNextSiblingItem( hStart );
		  hStart = hNext;
	    }
	}

	if( !bCtrl && !bShift && hNewSelItem )
	{
		bool bSelect = false;
          HTREEITEM hStart =	GetRootItem(); 
	    if( !hOldSelItem )
		   hOldSelItem = hStart;

      LOOPINFO lInfo;
		  lInfo.pTree =  this;
		  lInfo.hItem = NULL;
		  lInfo.pParent = GetParent();
		  lInfo.m_param1   = &bSelect;
      lInfo.m_nNumberOfCheckboxes = m_nNumberOfCheckboxes;

      SelectItems(&lInfo);

		SetItemState(hNewSelItem, UINT(TVIS_SELECTED), UINT(TVIS_SELECTED));

	}
	if( action == TVC_UNKNOWN && m_bOldItemSelected && hOldSelItem  ) 
	{
		SetItemState(hOldSelItem, UINT(TVIS_SELECTED), UINT(TVIS_SELECTED));		
	}
	UpdateWindow() ;
	*pResult = 0;

	CollectSelectedItems();
//	GetParent()->SendMessage( TVN_SELECTION_CHANGED,(WPARAM)(&m_vecSelectedItems),NULL ); 
  NMTREEVIEW nmtree;
  nmtree.hdr.code = TVN_SELECTION_CHANGED;
  nmtree.hdr.hwndFrom = pNMTreeView->hdr.hwndFrom;
  nmtree.hdr.idFrom = pNMTreeView->hdr.idFrom;
  nmtree.action = pNMTreeView->action;
  nmtree.itemOld = pNMTreeView->itemOld;
  nmtree.itemNew = pNMTreeView->itemOld;

  GetParent()->SendMessage(WM_NOTIFY, (WPARAM)pNMHDR->idFrom, (LPARAM)&nmtree);
}

void CCheckboxesTreeCtrl::OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hOldSelItem = pNMTreeView->itemOld.hItem;
	HTREEITEM hNewSelItem = pNMTreeView->itemNew.hItem;

	m_bOldItemSelected = hOldSelItem && (GetItemState(hOldSelItem, UINT(TVIS_SELECTED)) & TVIS_SELECTED);

	bool bCtrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
	bool bShift = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;

	UINT& action = pNMTreeView->action;

	if((action == TVC_BYMOUSE && bCtrl) ) 
	{
		//If the new item was already in selected state, then deselect it.This may happen say for example
		//item1 is first selected and by pressing ctrl key item2 is selected.Now we have 2 items in selection
		//Now if the user presses ctrl key and clicks on item1.Now our olditem is item2[TVIS_SELECTED] and new item is
		//item1[TVIS_SELECTED] and at the same time item1 is in the selected state and our ctrl key is ON
		if( hNewSelItem )
		{
			if( (GetItemState(hNewSelItem,TVIS_SELECTED)&TVIS_SELECTED) )
			{
				//Deselect the new item
				SetItemState(hNewSelItem, UINT(~TVIS_SELECTED), UINT(TVIS_SELECTED));
				UpdateWindow() ;

				//abort change of selection.Otherwise TVN_SELCHANGED message will be generated.
				*pResult = 0 ;	 
			}
		
		}
	}

	if( (action== TVC_BYKEYBOARD) && bShift )
	{
		if( GetItemState(hNewSelItem,TVIS_SELECTED)&TVIS_SELECTED )
			m_bOldItemSelected = false;
	}
	
	*pResult = 0;
}

void CCheckboxesTreeCtrl::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here

	CollectSelectedItems();

	if( pTVKeyDown->wVKey == VK_DELETE )
	{
		//Delete the selected items
        /*
		size_t iSelItemCnt = 0;
		for( ; iSelItemCnt < m_vecSelectedItems.size(); iSelItemCnt++ )
		{
			SetItemData( m_vecSelectedItems[iSelItemCnt],0 );
			DeleteItem( m_vecSelectedItems[iSelItemCnt] );
		}*/
	}
	if( pTVKeyDown->wVKey == VK_F2 )
	{
		if( m_vecSelectedItems.size() == 1 )
			EditLabel( m_vecSelectedItems[0] );
	}

  if( pTVKeyDown->wVKey == VK_RETURN && 1 == m_nNumberOfCheckboxes)
  {
    HTREEITEM hClickedItem = GetSelectedItem();
    PCUSTOMITEMDATA pCustData = (PCUSTOMITEMDATA) CTreeCtrl::GetItemData(hClickedItem);
    this->SetCheckOne(hClickedItem, !pCustData->m_bCheckState1);
  }

	UpdateWindow();

	*pResult = 0;
}

//DESCRIPTION:
//		To add the image prepared with the checkbox into the list
//PARAM
//		lpInsertStruct[in]		-	Custom data of item to be inserted
int	CCheckboxesTreeCtrl::AddImageToList( PCUSTOMINSERTSTRUCT lpInsertStruct )
{		
	bool	bNeedChkBox	= lpInsertStruct->m_pCustomData->m_bChecked;
  bool  bInActive = lpInsertStruct->m_pCustomData->m_bEnable;
	int index = 0;
	int ChkBmpID,unChkBmpID;

	if( bNeedChkBox )
	{
    ChkBmpID			= IDB_BITMAP_CHECK;
		unChkBmpID		= IDB_BITMAP_UNCHECK;
	}
	else
	{
		ChkBmpID = unChkBmpID =   IDB_BITMAP_EMPTY;
    return 0;
	}

  if ( false == bInActive )
  {
		ChkBmpID = unChkBmpID =   IDB_BITMAP_EMPTY;
	}
	//Add the normal state image
	index = AddNoramlStateImages(lpInsertStruct);
	if( index && bInActive)
		return 0;

	//Add the images for the check state
	AddCheckStateImages( ChkBmpID,unChkBmpID,lpInsertStruct->m_pCustomData );

	return 0;
}

//DESCRIPTION:
///			To add the images for normal and expanded state [ WITHOUT CHECKBOX ]
//PARAM
///		uBmpID[in]			-	ID for the normal image
///		uExpBmpID[in]		-	ID for the expanded image
///		lpInsertStruct[in]	-	Custom Insert structure
int	CCheckboxesTreeCtrl::AddNoramlStateImages(PCUSTOMINSERTSTRUCT lpInsertStruct )
{
	if( !lpInsertStruct )
		return -1;

	//if already the image is there
	int iBmpFound = -1;
  m_mapBitmapID.clear();
	iBmpFound = GetBitmapIndexFromImgList( m_nNumberOfCheckboxes );

	//If bitmap is found then return
	int addIndex = 0;
	if( iBmpFound > -1 )
	{
		std::vector<int>  &vecExistingImages	= m_mapBitmapID[m_nNumberOfCheckboxes];
    if ( 1 == m_nNumberOfCheckboxes )
    {
		  lpInsertStruct->m_pCustomData->m_iUnChecked			=	vecExistingImages[0];
		  lpInsertStruct->m_pCustomData->m_iChecked		    =	vecExistingImages[1];	
    }
		else if ( 2 == m_nNumberOfCheckboxes )
    {
		  lpInsertStruct->m_pCustomData->m_iUnCheckedUnChecked			=	vecExistingImages[0];
		  lpInsertStruct->m_pCustomData->m_iCheckedUnChecked		    =	vecExistingImages[1];	
		  lpInsertStruct->m_pCustomData->m_iUnCheckedChecked		    =	vecExistingImages[2];		
		  lpInsertStruct->m_pCustomData->m_iCheckedChecked          =	vecExistingImages[3];
    }
    else if ( 3 == m_nNumberOfCheckboxes )
    {
		  lpInsertStruct->m_pCustomData->m_iUnCheckedUnCheckedUnChecked			=	vecExistingImages[0];
		  lpInsertStruct->m_pCustomData->m_iCheckedUnCheckedUnChecked		    =	vecExistingImages[1];	
		  lpInsertStruct->m_pCustomData->m_iUnCheckedCheckedUnChecked		    =	vecExistingImages[2];		
		  lpInsertStruct->m_pCustomData->m_iCheckedCheckedUnChecked         =	vecExistingImages[3];
      lpInsertStruct->m_pCustomData->m_iUnCheckedUnCheckedChecked			  =	vecExistingImages[4];
		  lpInsertStruct->m_pCustomData->m_iCheckedUnCheckedChecked		      =	vecExistingImages[5];	
		  lpInsertStruct->m_pCustomData->m_iUnCheckedCheckedChecked		      =	vecExistingImages[6];		
		  lpInsertStruct->m_pCustomData->m_iCheckedCheckedChecked           =	vecExistingImages[7];
    }
    else if ( 4 == m_nNumberOfCheckboxes )
    {
		  lpInsertStruct->m_pCustomData->m_iUnChkUnChkUnChkUnChk	  	=	vecExistingImages[0];
		  lpInsertStruct->m_pCustomData->m_iChkUnChkUnChkUnChk	      =	vecExistingImages[1];	
		  lpInsertStruct->m_pCustomData->m_iUnChkChkUnChkUnChk		    =	vecExistingImages[2];		
		  lpInsertStruct->m_pCustomData->m_iChkChkUnChkUnChk          =	vecExistingImages[3];
      lpInsertStruct->m_pCustomData->m_iUnChkUnChkChkUnChk			  =	vecExistingImages[4];
		  lpInsertStruct->m_pCustomData->m_iChkUnChkChkUnChk		      =	vecExistingImages[5];	
		  lpInsertStruct->m_pCustomData->m_iUnChkChkChkUnChk		      =	vecExistingImages[6];		
		  lpInsertStruct->m_pCustomData->m_iChkChkChkUnChk            =	vecExistingImages[7];

      lpInsertStruct->m_pCustomData->m_iUnChkUnChkUnChkChk	  	  =	vecExistingImages[8];
		  lpInsertStruct->m_pCustomData->m_iChkUnChkUnChkChk	        =	vecExistingImages[9];	
		  lpInsertStruct->m_pCustomData->m_iUnChkChkUnChkChk		      =	vecExistingImages[10];		
		  lpInsertStruct->m_pCustomData->m_iChkChkUnChkChk            =	vecExistingImages[11];
      lpInsertStruct->m_pCustomData->m_iUnChkUnChkChkChk			    =	vecExistingImages[12];
		  lpInsertStruct->m_pCustomData->m_iChkUnChkChkChk		        =	vecExistingImages[13];	
		  lpInsertStruct->m_pCustomData->m_iUnChkChkChkChk		        =	vecExistingImages[14];		
		  lpInsertStruct->m_pCustomData->m_iChkChkChkChk              =	vecExistingImages[15];
    }
    
		return 1;		
	}

	std::vector<int> vecImages;

	m_mapBitmapID.insert( make_pair(m_nNumberOfCheckboxes,vecImages) );

	return 0;
}									  

//DESCRIPTION:
///			To add the images for the check,uncheck images for normal state
//PARAM
///		uBmpID[in]			-	ID for the normal image
///		uCheckBmpID[in]		-	ID of the check image
///		uUnCheckBmpID[in]	-	ID of the uncheck image
///		pCustData[in]		-	Custom data where the image index is stored
int	CCheckboxesTreeCtrl::AddCheckStateImages(UINT uCheckBmpID,UINT uUnCheckBmpID,PCUSTOMITEMDATA pCustData )
{
	if( !pCustData )
		return -1;

	//Get the index of the normal image
	int iBmpFound = -1;
	iBmpFound = GetBitmapIndexFromImgList( m_nNumberOfCheckboxes);
	if( iBmpFound == -1 )
		return -1;

	std::vector<int>  &vecExistingImages = m_mapBitmapID[m_nNumberOfCheckboxes];
	if( !pCustData->m_bChecked )
	{
		vecExistingImages.push_back( -1 );
		vecExistingImages.push_back( -1 );
		return -1;
	}
	
  if (0 == m_nNumberOfCheckboxes)
  {
		int newBmpW = 0,newBmpH = 0;
    vecExistingImages.push_back( -1 );
		vecExistingImages.push_back( -1 );

	  //If there is no image list create empty one
	  if( !m_pImgList )
	  {
		  m_pImgList = new CImageList;
		  m_pImgList->Create( newBmpW,newBmpH,ILC_COLOR32 | ILC_MASK,5,1);
		  SetImageList(m_pImgList,TVSIL_NORMAL);
	  }

    return -1;
  }
  else if (1 == m_nNumberOfCheckboxes)
  {
    int iIndex = AddImage1Chk( uUnCheckBmpID);
		pCustData->m_iUnChecked		= iIndex;

		iIndex = AddImage1Chk( uCheckBmpID);
		pCustData->m_iChecked		= iIndex;

		vecExistingImages.push_back( 	pCustData->m_iUnChecked );
		vecExistingImages.push_back( 	pCustData->m_iChecked );

		return iIndex;
  }
	else if (2 == m_nNumberOfCheckboxes)
	{
		//uncheck, uncheck
		int iIndex = AddImage2Chk( uUnCheckBmpID, uUnCheckBmpID);
		pCustData->m_iUnCheckedUnChecked		= iIndex;
		//check, uncheck
		iIndex = AddImage2Chk( uCheckBmpID, uUnCheckBmpID );
		pCustData->m_iCheckedUnChecked		= iIndex;
		//uncheck, check
		iIndex = AddImage2Chk( uUnCheckBmpID,uCheckBmpID );
		pCustData->m_iUnCheckedChecked		= iIndex;
		//check, check
		iIndex = AddImage2Chk( uCheckBmpID,uCheckBmpID );
		pCustData->m_iCheckedChecked		= iIndex;

		vecExistingImages.push_back( 	pCustData->m_iUnCheckedUnChecked );
		vecExistingImages.push_back( 	pCustData->m_iCheckedUnChecked );
		vecExistingImages.push_back( 	pCustData->m_iUnCheckedChecked );
		vecExistingImages.push_back( 	pCustData->m_iCheckedChecked );

		return iIndex;
	}
  else if (3 == m_nNumberOfCheckboxes)
	{
    int iIndex = AddImage3Chk( uUnCheckBmpID, uUnCheckBmpID, uUnCheckBmpID);
		pCustData->m_iUnCheckedUnCheckedUnChecked		= iIndex;

		iIndex = AddImage3Chk( uCheckBmpID, uUnCheckBmpID, uUnCheckBmpID );
		pCustData->m_iCheckedUnCheckedUnChecked		= iIndex;

		iIndex = AddImage3Chk( uUnCheckBmpID,uCheckBmpID, uUnCheckBmpID );
		pCustData->m_iUnCheckedCheckedUnChecked		= iIndex;

		iIndex = AddImage3Chk( uCheckBmpID,uCheckBmpID, uUnCheckBmpID );
		pCustData->m_iCheckedCheckedUnChecked		= iIndex;

    iIndex = AddImage3Chk( uUnCheckBmpID, uUnCheckBmpID, uCheckBmpID);
		pCustData->m_iUnCheckedUnCheckedChecked		= iIndex;

		iIndex = AddImage3Chk( uCheckBmpID, uUnCheckBmpID, uCheckBmpID );
		pCustData->m_iCheckedUnCheckedChecked		= iIndex;

		iIndex = AddImage3Chk( uUnCheckBmpID,uCheckBmpID, uCheckBmpID );
		pCustData->m_iUnCheckedCheckedChecked		= iIndex;

		iIndex = AddImage3Chk( uCheckBmpID,uCheckBmpID, uCheckBmpID );
		pCustData->m_iCheckedCheckedChecked		= iIndex;

		vecExistingImages.push_back( 	pCustData->m_iUnCheckedUnCheckedUnChecked );
		vecExistingImages.push_back( 	pCustData->m_iCheckedUnCheckedUnChecked );
		vecExistingImages.push_back( 	pCustData->m_iUnCheckedCheckedUnChecked );
		vecExistingImages.push_back( 	pCustData->m_iCheckedCheckedUnChecked );
    vecExistingImages.push_back( 	pCustData->m_iUnCheckedUnCheckedChecked );
		vecExistingImages.push_back( 	pCustData->m_iCheckedUnCheckedChecked );
		vecExistingImages.push_back( 	pCustData->m_iUnCheckedCheckedChecked );
		vecExistingImages.push_back( 	pCustData->m_iCheckedCheckedChecked );

		return iIndex;
  }
	else if (4 == m_nNumberOfCheckboxes)
	{
		int iIndex = AddImage4Chk( uUnCheckBmpID ,uUnCheckBmpID,uUnCheckBmpID ,uUnCheckBmpID);
		pCustData->m_iUnChkUnChkUnChkUnChk		= iIndex;

		iIndex = AddImage4Chk( uCheckBmpID,uUnCheckBmpID ,uUnCheckBmpID ,uUnCheckBmpID);
		pCustData->m_iChkUnChkUnChkUnChk		= iIndex;

		iIndex = AddImage4Chk( uUnCheckBmpID,uCheckBmpID ,uUnCheckBmpID ,uUnCheckBmpID);
		pCustData->m_iUnChkChkUnChkUnChk		= iIndex;

		iIndex = AddImage4Chk( uCheckBmpID,uCheckBmpID ,uUnCheckBmpID ,uUnCheckBmpID);
		pCustData->m_iChkChkUnChkUnChk		= iIndex;

    iIndex = AddImage4Chk( uUnCheckBmpID ,uUnCheckBmpID,uCheckBmpID ,uUnCheckBmpID);
		pCustData->m_iUnChkUnChkChkUnChk		= iIndex;

		iIndex = AddImage4Chk( uCheckBmpID,uUnCheckBmpID ,uCheckBmpID ,uUnCheckBmpID);
		pCustData->m_iChkUnChkChkUnChk		= iIndex;

		iIndex = AddImage4Chk( uUnCheckBmpID,uCheckBmpID ,uCheckBmpID ,uUnCheckBmpID);
		pCustData->m_iUnChkChkChkUnChk		= iIndex;

		iIndex = AddImage4Chk( uCheckBmpID,uCheckBmpID ,uCheckBmpID ,uUnCheckBmpID);
		pCustData->m_iChkChkChkUnChk		= iIndex;

    iIndex = AddImage4Chk( uUnCheckBmpID ,uUnCheckBmpID,uUnCheckBmpID ,uCheckBmpID);
		pCustData->m_iUnChkUnChkUnChkChk		= iIndex;

		iIndex = AddImage4Chk( uCheckBmpID,uUnCheckBmpID ,uUnCheckBmpID ,uCheckBmpID);
		pCustData->m_iChkUnChkUnChkChk		= iIndex;

		iIndex = AddImage4Chk( uUnCheckBmpID,uCheckBmpID ,uUnCheckBmpID ,uCheckBmpID);
		pCustData->m_iUnChkChkUnChkChk		= iIndex;

		iIndex = AddImage4Chk( uCheckBmpID,uCheckBmpID ,uUnCheckBmpID ,uCheckBmpID);
		pCustData->m_iChkChkUnChkChk		= iIndex;

    iIndex = AddImage4Chk( uUnCheckBmpID ,uUnCheckBmpID,uCheckBmpID ,uCheckBmpID);
		pCustData->m_iUnChkUnChkChkChk		= iIndex;

		iIndex = AddImage4Chk( uCheckBmpID,uUnCheckBmpID ,uCheckBmpID ,uCheckBmpID);
		pCustData->m_iChkUnChkChkChk		= iIndex;

		iIndex = AddImage4Chk( uUnCheckBmpID,uCheckBmpID ,uCheckBmpID ,uCheckBmpID);
		pCustData->m_iUnChkChkChkChk		= iIndex;

		iIndex = AddImage4Chk( uCheckBmpID,uCheckBmpID ,uCheckBmpID ,uCheckBmpID);
		pCustData->m_iChkChkChkChk		= iIndex;

		vecExistingImages.push_back( 	pCustData->m_iUnChkUnChkUnChkUnChk );
		vecExistingImages.push_back( 	pCustData->m_iChkUnChkUnChkUnChk );
		vecExistingImages.push_back( 	pCustData->m_iUnChkChkUnChkUnChk );
		vecExistingImages.push_back( 	pCustData->m_iChkChkUnChkUnChk );
    vecExistingImages.push_back( 	pCustData->m_iUnChkUnChkChkUnChk );
		vecExistingImages.push_back( 	pCustData->m_iChkUnChkChkUnChk );
		vecExistingImages.push_back( 	pCustData->m_iUnChkChkChkUnChk );
		vecExistingImages.push_back( 	pCustData->m_iChkChkChkUnChk );
    vecExistingImages.push_back( 	pCustData->m_iUnChkUnChkUnChkChk );
		vecExistingImages.push_back( 	pCustData->m_iChkUnChkUnChkChk );
		vecExistingImages.push_back( 	pCustData->m_iUnChkChkUnChkChk );
		vecExistingImages.push_back( 	pCustData->m_iChkChkUnChkChk );	  
    vecExistingImages.push_back( 	pCustData->m_iUnChkUnChkChkChk );
		vecExistingImages.push_back( 	pCustData->m_iChkUnChkChkChk );
		vecExistingImages.push_back( 	pCustData->m_iUnChkChkChkChk );
		vecExistingImages.push_back( 	pCustData->m_iChkChkChkChk );

		return iIndex;
	}
	return -1;
}						

//DESCRIPTION:
//		To prepare the bitmap along with the check box
//PARAM
//		nBmpID[in]			-	ID of the bitmap checkbox 1
//		resBmp[in]			-	Prepared Bitmap object
//		newBmpW[out]		-	Width of the new bitmap
//		newBmpH[out]		-	Height of the newbitmap
//		nBmpID2[in]			-	ID of the bitmap checkbox 2
//   nBmpID3[in]			-	ID of the bitmap checkbox 3
//   nBmpID4[in]			-	ID of the bitmap checkbox 4
int	CCheckboxesTreeCtrl::PrepareImageWithPrefixBmp( UINT nBmpID1,CBitmap& resBmp,int& newBmpW,int& newBmpH,UINT nBmpID2 /*= -1*/,UINT nBmpID3 /*= -1*/,UINT nBmpID4 /*= -1*/ )
{
	CBitmap bmp1,bmp2,bmp3,bmp4;
  BITMAP bm1,bm2,bm3,bm4;
  int res;
	//Create the dc
	CClientDC dc( this );

  if ( 0 < m_nNumberOfCheckboxes)
  {
	  //Load the given bimap resource
	  bmp1.LoadBitmap( nBmpID1 );
	  //res is Nonzero if successful; otherwise 0.
	  res = bmp1.GetBitmap(&bm1);
  }

	//Get the size of the checkbox bitmap
  if ( 1 < m_nNumberOfCheckboxes)
  {
	  bmp2.LoadBitmap( nBmpID2 );
	  res = bmp2.GetBitmap(&bm2);
  }

  if ( 2 < m_nNumberOfCheckboxes)
  {
	  bmp3.LoadBitmap( nBmpID3 );
	  res = bmp3.GetBitmap(&bm3);
  }

  if ( 3 < m_nNumberOfCheckboxes)
  {
	  bmp4.LoadBitmap( nBmpID4 );
	  res = bmp4.GetBitmap(&bm4);
  }

	newBmpW = m_nNumberOfCheckboxes* bm1.bmWidth; 
	newBmpH = bm1.bmHeight;

  if (1 == m_nNumberOfCheckboxes)
    AddBitamps1Chk( &bmp1,&resBmp,newBmpW,newBmpH );	
  else if (2 == m_nNumberOfCheckboxes)
    AddBitamps2Chk( &bmp1,&bmp2,&resBmp,newBmpW,newBmpH );	
  else if (3 == m_nNumberOfCheckboxes)
    AddBitamps3Chk( &bmp1,&bmp2,&bmp3,&resBmp,newBmpW,newBmpH );	
  else if(4 == m_nNumberOfCheckboxes)
    AddBitamps4Chk( &bmp1,&bmp2,&bmp3,&bmp4,&resBmp,newBmpW,newBmpH );	

	return -1;
	
}

//DESCRIPTION:
///		To scale a given bitmp
//PARAM
///		bmp[in]			-	Bitmap to be scaled
///		Width[in]		-	width of the  bitmap
///		Height[in]		-	Height of the bitmap
///		newWidth[in]	-	New width of the  bitmap
///		newHeight[in]	-	New Height of the bitmap
///		resBmp[out]		-	Resulting scaled bitmap
void	CCheckboxesTreeCtrl::ScaleBitmap( CBitmap& bmp,int Width,int Height,int newWidth,int newHeight,CBitmap& resBmp  )
{
	CClientDC dc( this );

	//Create the memory dc
	CDC cMemDC1,cMemDC2;
	cMemDC1.CreateCompatibleDC(&dc );
	cMemDC2.CreateCompatibleDC(&dc );

	//Select the bitmap into the memdc
	CBitmap* pOldBmp1 = cMemDC1.SelectObject(&bmp);

	//Create a bitmap to the new size and width
	resBmp.CreateCompatibleBitmap(&dc,newWidth,newHeight );
	CBitmap* pOldBmp2 = cMemDC2.SelectObject(&resBmp );

	//Now copy the bitmap from memdc1 to memdc2
	cMemDC2.StretchBlt(0,0,newWidth,newHeight,&cMemDC1,0,0,Width,Height,SRCCOPY);

	cMemDC1.SelectObject(pOldBmp1);
	cMemDC2.SelectObject(pOldBmp2);
	
}

//DESCRIPTION:
///		To combine to given bitmaps for 4 checkboxes on item
//PARAM
///		pBmp1[in]	-	First bitmap Reference
///		pBmp2[in]	-	Second bitmap Reference
///		pBmp3[in]	-	Third bitmap Reference
///		pBmp4[in]	-	Fourth bitmap Reference
///		resBmp[in]	-	Resulting bitmap
///		W[in]		-	New height of the bitmap.If -1 then autocomputation is done
///		H[in]		-	New height of the bitmap.If -1 then autocomputation is done
bool CCheckboxesTreeCtrl:: AddBitamps4Chk( CBitmap* pBmp1,CBitmap* pBmp2,CBitmap* pBmp3,CBitmap*  pBmp4,CBitmap* resBmp,int W /*=-1*/,int H/*=-1*/ )
{
  int newBmpW = W;
  int newBmpH = H;

  CClientDC dc(this);

  BITMAP bm1,bm2,bm3,bm4;
  if (NULL != pBmp1->m_hObject)
  {
	  pBmp1->GetBitmap(&bm1);
  }
  if (NULL != pBmp2->m_hObject)
  {
	  pBmp2->GetBitmap(&bm2);
  }
  if (NULL != pBmp3->m_hObject)
  {
    pBmp3->GetBitmap(&bm3);
  }
  if (NULL != pBmp4->m_hObject)
  {
	  pBmp4->GetBitmap(&bm4);
  }
	
  CDC cMemDC1,cMemDC2,cMemDC3,cMemDC4,CMemDCTgt;
  CDC* cMemDC;
  CDC  cMemDCgt;
  cMemDC = new CDC[m_nNumberOfCheckboxes];

  if(!cMemDC->CreateCompatibleDC(&dc))
		return false;
  if(!cMemDCgt.CreateCompatibleDC(&dc))
		return false;

  CBitmap* oldBmp; 
  oldBmp = new CBitmap[m_nNumberOfCheckboxes];

  //This memory dc is used to copy the checkbox bitmap
  if(!cMemDC1.CreateCompatibleDC(&dc))
    return false;

  if(!cMemDC2.CreateCompatibleDC(&dc))
    return false;

  if(!cMemDC3.CreateCompatibleDC(&dc))
    return false;

  if(!cMemDC4.CreateCompatibleDC(&dc))
    return false;

  //This dc is used to create and copy the combined bitmap [ checkbox bmp + image bmp ]
  if(!CMemDCTgt.CreateCompatibleDC(&dc))
    return false;

  //First copy the check box bitmap into memDC1
  CBitmap* oldBmp1 = cMemDC1.SelectObject( pBmp1 );
  CBitmap* oldBmp2 = cMemDC2.SelectObject( pBmp2 );
  CBitmap* oldBmp3 = cMemDC3.SelectObject( pBmp3 );
  CBitmap* oldBmp4 = cMemDC4.SelectObject( pBmp4 );

  //Create the bitmap of the required size [ Combine both the bitmaps ]	
  if(!resBmp->CreateCompatibleBitmap(&dc, newBmpW, newBmpH ))
	return false;
  
  //Now select the combined bitmap into the CMemDCTgt
  CBitmap* oldBmp5 = CMemDCTgt.SelectObject( resBmp );
	
  if ( NULL != oldBmp1)
  {
	  //Then first copy the checkbox bitmap from the position 0,0 on the target
	  CMemDCTgt.BitBlt( 0,0,bm1.bmWidth,bm1.bmHeight,&cMemDC1,0,0,SRCCOPY );
  }
  if ( NULL != oldBmp2)
  {
	  //Next copy the image bitmap from the position (bm2.bmWidth,bm2.bmHeight )
	  CMemDCTgt.BitBlt( bm1.bmWidth,0,bm2.bmWidth,bm2.bmHeight,&cMemDC2,0,0,SRCCOPY );
  }
  if ( NULL != oldBmp3)
  {
    CMemDCTgt.BitBlt( (bm1.bmWidth + bm2.bmWidth),0,bm3.bmWidth,bm3.bmHeight,&cMemDC3,0,0,SRCCOPY );
  }
  if ( NULL != oldBmp3)
  {
    CMemDCTgt.BitBlt( (bm1.bmWidth + bm2.bmWidth + bm3.bmWidth),0,bm4.bmWidth,bm4.bmHeight,&cMemDC4,0,0,SRCCOPY );
  }
  //Now restore the oldbitmaps
  cMemDC1.SelectObject( oldBmp1 );
  cMemDC2.SelectObject( oldBmp2 );
  cMemDC3.SelectObject( oldBmp3 );
  cMemDC4.SelectObject( oldBmp4 );
  CMemDCTgt.SelectObject( oldBmp5 );

  //delete cMemDC;
  //cMemDC = NULL;
  return true;
}

//DESCRIPTION:
///		To combine to given bitmaps for 3 checkboxes on item
//PARAM
///		pBmp1[in]	-	First bitmap Reference
///		pBmp2[in]	-	Second bitmap Reference
//		pBmp3[in]	-	Third bitmap Reference
//		resBmp[in]	-	Resulting bitmap
//		W[in]		-	New height of the bitmap.If -1 then autocomputation is done
//		H[in]		-	New height of the bitmap.If -1 then autocomputation is done
bool CCheckboxesTreeCtrl:: AddBitamps3Chk( CBitmap* pBmp1,CBitmap* pBmp2,CBitmap* pBmp3,CBitmap* resBmp,int W /*=-1*/,int H/*=-1*/ )
{
	int newBmpW = W;
	int newBmpH = H;

	CClientDC dc(this);

	BITMAP bm1,bm2,bm3;
  if (NULL != pBmp1->m_hObject)
  {
	  pBmp1->GetBitmap(&bm1);
  }
  if (NULL != pBmp2->m_hObject)
  {
	  pBmp2->GetBitmap(&bm2);
  }
  if (NULL != pBmp3->m_hObject)
  {
    pBmp3->GetBitmap(&bm3);
  }
	
	//Create the 3 memory DCs.
	CDC cMemDC1,cMemDC2,cMemDC3,CMemDCTgt;

	//This memory dc is used to copy the checkbox bitmap
	if(!cMemDC1.CreateCompatibleDC(&dc))
		return false;

	if(!cMemDC2.CreateCompatibleDC(&dc))
		return false;

	if(!cMemDC3.CreateCompatibleDC(&dc))
		return false;


	//This dc is used to create and copy the combined bitmap [ checkbox bmp + image bmp ]
	if(!CMemDCTgt.CreateCompatibleDC(&dc))
		return false;

	//First copy the check box bitmap into memDC1
	 CBitmap* oldBmp1 = cMemDC1.SelectObject( pBmp1 );
	 CBitmap* oldBmp2 = cMemDC2.SelectObject( pBmp2 );
   CBitmap* oldBmp3 = cMemDC3.SelectObject( pBmp3 );

	//Create the bitmap of the required size [ Combine both the bitmaps ]
	if(!resBmp->CreateCompatibleBitmap(&dc, newBmpW, newBmpH ))
		return false;

	//Now select the combined bitmap into the CMemDCTgt
	CBitmap* oldBmp5 = CMemDCTgt.SelectObject( resBmp );
	
  if ( NULL != oldBmp1)
  {
	  //Then first copy the checkbox bitmap from the position 0,0 on the target
	  CMemDCTgt.BitBlt( 0,0,bm1.bmWidth,bm1.bmHeight,&cMemDC1,0,0,SRCCOPY );
  }
  if ( NULL != oldBmp2)
  {
	  //Next copy the image bitmap from the position (bm2.bmWidth,bm2.bmHeight )
	  CMemDCTgt.BitBlt( bm1.bmWidth,0,bm2.bmWidth,bm2.bmHeight,&cMemDC2,0,0,SRCCOPY );
  }
  if ( NULL != oldBmp3)
  {
    CMemDCTgt.BitBlt( (bm1.bmWidth + bm2.bmWidth),0,bm3.bmWidth,bm3.bmHeight,&cMemDC3,0,0,SRCCOPY );
  }

	//Now restore the oldbitmaps
	cMemDC1.SelectObject( oldBmp1 );
	cMemDC2.SelectObject( oldBmp2 );
  cMemDC3.SelectObject( oldBmp3 );
	CMemDCTgt.SelectObject( oldBmp5 );

	return true;
}

//DESCRIPTION:
//		To combine to given bitmaps for 2 checkboxes on item
//PARAM
//		pBmp1[in]	-	First bitmap Reference
//		pBmp2[in]	-	Second bitmap Reference
//		resBmp[in]	-	Resulting bitmap
//		W[in]		-	New height of the bitmap.If -1 then autocomputation is done
//		H[in]		-	New height of the bitmap.If -1 then autocomputation is done
bool CCheckboxesTreeCtrl:: AddBitamps2Chk( CBitmap* pBmp1,CBitmap* pBmp2,CBitmap* resBmp,int W /*=-1*/,int H/*=-1*/ )
{
	int newBmpW = W;
	int newBmpH = H;

	CClientDC dc(this);

	BITMAP bm1,bm2;
  if (NULL != pBmp1->m_hObject)
  {
	  pBmp1->GetBitmap(&bm1);
  }
  if (NULL != pBmp2->m_hObject)
  {
	  pBmp2->GetBitmap(&bm2);
  }


	//If the user doesnt know the new size of the bitamp then compute it
	if( newBmpW == -1 || newBmpH == - 1)
	{   
		newBmpW = bm1.bmWidth + bm2.bmWidth;
		newBmpH = bm1.bmHeight + bm2.bmHeight;
	}
	
	//Create the 3 memory DCs.
	CDC cMemDC1,cMemDC2,CMemDCTgt;

	//This memory dc is used to copy the checkbox bitmap
	if(!cMemDC1.CreateCompatibleDC(&dc))
		return false;

	if(!cMemDC2.CreateCompatibleDC(&dc))
		return false;

	//This dc is used to create and copy the combined bitmap
	if(!CMemDCTgt.CreateCompatibleDC(&dc))
		return false;

	//First copy the check box bitmap into memDC1
	 CBitmap* oldBmp1 = cMemDC1.SelectObject( pBmp1 );
	 CBitmap* oldBmp2 = cMemDC2.SelectObject( pBmp2 );

	//Create the bitmap of the required size [ Combine both the bitmaps ]
	if(!resBmp->CreateCompatibleBitmap(&dc, newBmpW, newBmpH ))
		return false;

	//Now select the combined bitmap into the CMemDCTgt
	CBitmap* oldBmp5 = CMemDCTgt.SelectObject( resBmp );
	
  if ( NULL != oldBmp1)
  {
	  //Then first copy the checkbox bitmap from the position 0,0 on the target
	  CMemDCTgt.BitBlt( 0,0,bm1.bmWidth,bm1.bmHeight,&cMemDC1,0,0,SRCCOPY );
  }
  if ( NULL != oldBmp2)
  {
	  //Next copy the image bitmap from the position (bm2.bmWidth,bm2.bmHeight )
	  CMemDCTgt.BitBlt( bm1.bmWidth,0,bm2.bmWidth,bm2.bmHeight,&cMemDC2,0,0,SRCCOPY );
  }

	//Now restore the oldbitmaps
	cMemDC1.SelectObject( oldBmp1 );
	cMemDC2.SelectObject( oldBmp2 );
	CMemDCTgt.SelectObject( oldBmp5 );

	return true;
}

//DESCRIPTION:
//		To combine to given bitmaps for 1 checkboxes on item
//PARAM
//		pBmp1[in]	-	First bitmap Reference
//		resBmp[in]	-	Resulting bitmap
//		W[in]		-	New height of the bitmap.If -1 then autocomputation is done
//		H[in]		-	New height of the bitmap.If -1 then autocomputation is done
bool CCheckboxesTreeCtrl:: AddBitamps1Chk( CBitmap* pBmp1,CBitmap* resBmp,int W /*=-1*/,int H/*=-1*/ )
{
	int newBmpW = W;
	int newBmpH = H;

	CClientDC dc(this);

	BITMAP bm1;
  if (NULL != pBmp1->m_hObject)
  {
	  pBmp1->GetBitmap(&bm1);
  }
	
	//Create the 3 memory DCs.
	CDC cMemDC1,CMemDCTgt;

	//This memory dc is used to copy the checkbox bitmap
	if(!cMemDC1.CreateCompatibleDC(&dc))
		return false;

	//This dc is used to create and copy the combined bitmap [ checkbox bmp + image bmp ]
	if(!CMemDCTgt.CreateCompatibleDC(&dc))
		return false;

	//First copy the check box bitmap into memDC1
	 CBitmap* oldBmp1 = cMemDC1.SelectObject( pBmp1 );

	//Create the bitmap of the required size [ Combine both the bitmaps ]
	if(!resBmp->CreateCompatibleBitmap(&dc, newBmpW, newBmpH ))
		return false;

	//Now select the combined bitmap into the CMemDCTgt
	CBitmap* oldBmp5 = CMemDCTgt.SelectObject( resBmp );
	
  if ( NULL != oldBmp1)
  {
	  //Then first copy the checkbox bitmap from the position 0,0 on the target
	  CMemDCTgt.BitBlt( 0,0,bm1.bmWidth,bm1.bmHeight,&cMemDC1,0,0,SRCCOPY );
  }


	//Now restore the oldbitmaps
	cMemDC1.SelectObject( oldBmp1 );
	CMemDCTgt.SelectObject( oldBmp5 );

	//SaveBitmap( dc.GetSafeHdc(),*resBmp,"D:\\chk.bmp");
	return true;
}

//DESCRIPTION:
//		Message handler for the checkbox one click
//PARAM
//		pNMHDR[in]	-	Custom data structure
LRESULT CCheckboxesTreeCtrl::SetChkBoxOne(  WPARAM wp, LPARAM lp)
{
	LPCUSTNMHDR pNMHDR = ( LPCUSTNMHDR)wp;
	PCUSTOMITEMDATA pCustData = (PCUSTOMITEMDATA)( pNMHDR->m_data );

	if( !pCustData )
		return 1;
	if( pCustData->m_bChecked  )
	{
		pCustData->m_bCheckState1 = !pCustData->m_bCheckState1;	
		SetImage( pNMHDR->m_hItem );
    UpdateWindow();

    NM_TREEVIEW nmtvMessage;
    nmtvMessage.hdr.hwndFrom = m_hWnd;
    nmtvMessage.hdr.idFrom   = GetDlgCtrlID();
	  nmtvMessage.hdr.code     = TVN_ITEM_CHECK_ONE;
	  nmtvMessage.itemNew.hItem = pNMHDR->m_hItem;

    CWnd* pWnd = GetOwner();
    if( pWnd && pWnd->m_hWnd )
      pWnd->SendMessage(WM_NOTIFY, nmtvMessage.hdr.idFrom, (LPARAM)&nmtvMessage);
	}	
	return 0;
	
}

//DESCRIPTION:
//		Message handler for the checkbox two click
//PARAM
//		pNMHDR[in]	-	Custom data structure
LRESULT CCheckboxesTreeCtrl::SetChkBoxTwo(  WPARAM wp, LPARAM lp)
{
  LPCUSTNMHDR pNMHDR = ( LPCUSTNMHDR)wp;
  PCUSTOMITEMDATA pCustData = (PCUSTOMITEMDATA)( pNMHDR->m_data );

  if( !pCustData )
	return 1;
  if( pCustData->m_bChecked  )
  {		
    
	  pCustData->m_bCheckState2 = !pCustData->m_bCheckState2;	
	  SetImage( pNMHDR->m_hItem );

    NM_TREEVIEW nmtvMessage;
    nmtvMessage.hdr.hwndFrom = m_hWnd;
    nmtvMessage.hdr.idFrom   = GetDlgCtrlID();
	  nmtvMessage.hdr.code     = TVN_ITEM_CHECK_TWO;
	  nmtvMessage.itemNew.hItem = pNMHDR->m_hItem;

    CWnd* pWnd = GetOwner();
    if( pWnd && pWnd->m_hWnd )
      pWnd->SendMessage(WM_NOTIFY, nmtvMessage.hdr.idFrom, (LPARAM)&nmtvMessage);		
  }	
  return 0;
}

//DESCRIPTION:
//		Message handler for the checkbox three click
//PARAM
//		pNMHDR[in]	-	Custom data structure
LRESULT CCheckboxesTreeCtrl::SetChkBoxThree(  WPARAM wp, LPARAM lp)
{
  LPCUSTNMHDR pNMHDR = ( LPCUSTNMHDR)wp;
  PCUSTOMITEMDATA pCustData = (PCUSTOMITEMDATA)( pNMHDR->m_data );

  if( !pCustData )
    return 1;
  if( pCustData->m_bChecked  )
  {		
	  pCustData->m_bCheckState3 = !pCustData->m_bCheckState3;	
    SetImage( pNMHDR->m_hItem );

    NM_TREEVIEW nmtvMessage;
    nmtvMessage.hdr.hwndFrom = m_hWnd;
    nmtvMessage.hdr.idFrom   = GetDlgCtrlID();
	  nmtvMessage.hdr.code     = TVN_ITEM_CHECK_THREE;
	  nmtvMessage.itemNew.hItem = pNMHDR->m_hItem;

    CWnd* pWnd = GetOwner();
    if( pWnd && pWnd->m_hWnd )
      pWnd->SendMessage(WM_NOTIFY, nmtvMessage.hdr.idFrom, (LPARAM)&nmtvMessage);		
  }
  return 0;
}

//DESCRIPTION:
//		Message handler for the checkbox four click
//PARAM
//		pNMHDR[in]	-	Custom data structure
LRESULT CCheckboxesTreeCtrl::SetChkBoxFour(  WPARAM wp, LPARAM lp)
{
  LPCUSTNMHDR pNMHDR = ( LPCUSTNMHDR)wp;
  PCUSTOMITEMDATA pCustData = (PCUSTOMITEMDATA)( pNMHDR->m_data );

  if( !pCustData )
    return 1;
  if( pCustData->m_bChecked  )
  {		
	  pCustData->m_bCheckState4 = !pCustData->m_bCheckState4;	
    SetImage( pNMHDR->m_hItem );

    NM_TREEVIEW nmtvMessage;
    nmtvMessage.hdr.hwndFrom = m_hWnd;
    nmtvMessage.hdr.idFrom   = GetDlgCtrlID();
	  nmtvMessage.hdr.code     = TVN_ITEM_CHECK_FOUR;
	  nmtvMessage.itemNew.hItem = pNMHDR->m_hItem;

    CWnd* pWnd = GetOwner();
    if( pWnd && pWnd->m_hWnd )
      pWnd->SendMessage(WM_NOTIFY, nmtvMessage.hdr.idFrom, (LPARAM)&nmtvMessage);		
  }
  return 0;
}


//DESCRIPTION:
//		To check or uncheck first checkbox on item
//PARAM
//		hClickedItem     - handle of clicked item
//   bChecked         - check or uncheck first checkbox
void CCheckboxesTreeCtrl::SetCheckOne(HTREEITEM hClickedItem, bool bChecked)
{
  PCUSTOMITEMDATA pCustData = (PCUSTOMITEMDATA) CTreeCtrl::GetItemData(hClickedItem);
  pCustData->m_bCheckState1 = bChecked;
  SetCheckboxesState(hClickedItem);
}

//DESCRIPTION:
//		To check or uncheck secound checkbox on item
//PARAM
//		hClickedItem     - handle of clicked item
//   bChecked         - check or uncheck secound checkbox
void CCheckboxesTreeCtrl::SetCheckTwo(HTREEITEM hClickedItem, bool bChecked)
{
  PCUSTOMITEMDATA pCustData = (PCUSTOMITEMDATA) CTreeCtrl::GetItemData(hClickedItem);
  pCustData->m_bCheckState2 = bChecked;
  SetCheckboxesState(hClickedItem);
}

//DESCRIPTION:
//		To check or uncheck third checkbox on item
//PARAM
//		hClickedItem     - handle of clicked item
//   bChecked         - check or uncheck third checkbox
void CCheckboxesTreeCtrl::SetCheckThree(HTREEITEM hClickedItem, bool bChecked)
{
  PCUSTOMITEMDATA pCustData = (PCUSTOMITEMDATA) CTreeCtrl::GetItemData(hClickedItem);
  pCustData->m_bCheckState3 = bChecked;
  SetCheckboxesState(hClickedItem);
}

//DESCRIPTION:
//		To check or uncheck fourth checkbox on item
//PARAM
//		hClickedItem     - handle of clicked item
//   bChecked         - check or uncheck fourth checkbox
void CCheckboxesTreeCtrl::SetCheckFour(HTREEITEM hClickedItem, bool bChecked)
{
  PCUSTOMITEMDATA pCustData = (PCUSTOMITEMDATA) CTreeCtrl::GetItemData(hClickedItem);
  pCustData->m_bCheckState4 = bChecked;
  SetCheckboxesState(hClickedItem);
}

//DESCRIPTION:
//		Return state of checkbox
//PARAM
//		hClickedItem     - handle of clicked item
bool CCheckboxesTreeCtrl::GetCheckOne(HTREEITEM hClickedItem)
{
  PCUSTOMITEMDATA pCustData = (PCUSTOMITEMDATA) CTreeCtrl::GetItemData(hClickedItem);
  return pCustData->m_bCheckState1;
}

//DESCRIPTION:
//		Return state of checkbox
//PARAM
//		hClickedItem     - handle of clicked item
bool CCheckboxesTreeCtrl::GetCheckTwo(HTREEITEM hClickedItem)
{
  PCUSTOMITEMDATA pCustData = (PCUSTOMITEMDATA) CTreeCtrl::GetItemData(hClickedItem);
  return pCustData->m_bCheckState2;
}

//DESCRIPTION:
//		Return state of checkbox
//PARAM
//		hClickedItem     - handle of clicked item
bool CCheckboxesTreeCtrl::GetCheckThree(HTREEITEM hClickedItem)
{
  PCUSTOMITEMDATA pCustData = (PCUSTOMITEMDATA) CTreeCtrl::GetItemData(hClickedItem);
  return pCustData->m_bCheckState3;
}

//DESCRIPTION:
//		Return state of checkbox
//PARAM
//		hClickedItem     - handle of clicked item
bool CCheckboxesTreeCtrl::GetCheckFour(HTREEITEM hClickedItem)
{
  PCUSTOMITEMDATA pCustData = (PCUSTOMITEMDATA) CTreeCtrl::GetItemData(hClickedItem);
  return pCustData->m_bCheckState4;
}

//DESCRIPTION:
//			To get the index of the bitmap from the image list
//PARAM
//		uBitmapID[in]	-	ID of the bitmap
int	CCheckboxesTreeCtrl::GetBitmapIndexFromImgList( UINT uBitmapID )
{
	//Search the bitmap in the m_mapBitmapID
	int iBmpCtr = 0;
	map<UINT, vector<int> >::iterator it = m_mapBitmapID.find(uBitmapID);
	if( it != m_mapBitmapID.end( ) )
	{
		return ( std::distance( m_mapBitmapID.begin( ), it ) );
	}
	return -1;
}

//DESCRIPTION:
//    To add a given image and its mask to the list
//PARAM
//    uBmpID1[in]			-	ID of the bitmap 1
//    uBmpID2[in]			-	ID of the bitmap 2
//    uBmpID3[in]			-	ID of the bitmap 3
//    uBmpID4[in]			-	ID of the bitmap 4
int  CCheckboxesTreeCtrl::AddImage4Chk( UINT uBmpID1,UINT uBmpID2, UINT uBmpID3,UINT uBmpID4 )
{

	int newBmpW,newBmpH;
	CBitmap imageBmp;
	CClientDC dc(this);

	//Else prepare the image with its prefix.Similarly the mask of the image with its prefix
	PrepareImageWithPrefixBmp( uBmpID1,imageBmp,newBmpW,newBmpH,uBmpID2,uBmpID3,uBmpID4 );
	//If there is no image list create it
	if( !m_pImgList )
	{ 
		m_pImgList = new CImageList;
		m_pImgList->Create( newBmpW,newBmpH,ILC_COLOR32 | ILC_MASK,5,1);
		SetImageList(m_pImgList,TVSIL_NORMAL);
	}

	HBITMAP hNmp = imageBmp.operator HBITMAP();
	int index = m_pImgList->Add( &imageBmp,m_cMaskColor );

	//Get the image for the respective index
	IMAGEINFO imgInfo;
	m_pImgList->GetImageInfo(index,&imgInfo);
	return index;
}

//DESCRIPTION:
//			To add a given image and its mask to the list
//PARAM
//		uBmpID1[in]			-	ID of the bitmap 1
//		uBmpID2[in]			-	ID of the bitmap 2
//    uBmpID3[in]			-	ID of the bitmap 3
int  CCheckboxesTreeCtrl::AddImage3Chk( UINT uBmpID1,UINT uBmpID2, UINT uBmpID3 )
{

	int newBmpW,newBmpH;
	CBitmap imageBmp;
	CClientDC dc(this);

	//Else prepare the image with its prefix.Similarly the mask of the image with its prefix
	PrepareImageWithPrefixBmp( uBmpID1,imageBmp,newBmpW,newBmpH,uBmpID2,uBmpID3,NULL );

	//If there is no image list create it
	if( !m_pImgList )
	{
		m_pImgList = new CImageList;
		m_pImgList->Create( newBmpW,newBmpH,ILC_COLOR32 | ILC_MASK,5,1);
		SetImageList(m_pImgList,TVSIL_NORMAL);
	}
	HBITMAP hNmp = imageBmp.operator HBITMAP();
	int index = -1;
	index = m_pImgList->Add( &imageBmp,m_cMaskColor );

	//Get the image for the respective index
	IMAGEINFO imgInfo;
	m_pImgList->GetImageInfo(index,&imgInfo);
	return index;
}

//DESCRIPTION:
//			To add a given image and its mask to the list
//PARAM
//		uBmpID1[in]			-	ID of the bitmap 1
//		uBmpID2[in]			-	ID of the bitmap 2
int  CCheckboxesTreeCtrl::AddImage2Chk( UINT uBmpID1,UINT uBmpID2)
{

	int newBmpW,newBmpH;
	CBitmap imageBmp;
	CClientDC dc(this);

	//Else prepare the image with its prefix.Similarly the mask of the image with its prefix
	PrepareImageWithPrefixBmp( uBmpID1,imageBmp,newBmpW,newBmpH,uBmpID2,NULL,NULL );

	//If there is no image list create it
	if( !m_pImgList )
	{
		m_pImgList = new CImageList;
		m_pImgList->Create( newBmpW,newBmpH,ILC_COLOR32 | ILC_MASK,5,1);
		SetImageList(m_pImgList,TVSIL_NORMAL);
	}

	HBITMAP hNmp = imageBmp.operator HBITMAP();
	int index = -1;
	index = m_pImgList->Add( &imageBmp,m_cMaskColor );
	
	//Get the image for the respective index
	IMAGEINFO imgInfo;
	m_pImgList->GetImageInfo(index,&imgInfo);
	return index;
}

//DESCRIPTION:
//			To add a given image and its mask to the list
//PARAM
//		uBmpID1[in]			-	ID of the bitmap 1
int  CCheckboxesTreeCtrl::AddImage1Chk( UINT uBmpID1)
{
	int newBmpW,newBmpH;
	CBitmap imageBmp;
	CClientDC dc(this);

	//Else prepare the image with its prefix.Similarly the mask of the image with its prefix
	PrepareImageWithPrefixBmp( uBmpID1,imageBmp,newBmpW,newBmpH,NULL,NULL,NULL );

	//If there is no image list create it
	if( !m_pImgList )
	{
		m_pImgList = new CImageList;
		m_pImgList->Create( newBmpW,newBmpH,ILC_COLOR32 | ILC_MASK,5,1);
		SetImageList(m_pImgList,TVSIL_NORMAL);
	}

	HBITMAP hNmp = imageBmp.operator HBITMAP();
	int index = -1;
	index = m_pImgList->Add( &imageBmp,m_cMaskColor );
	
	//Get the image for the respective index
	IMAGEINFO imgInfo;
	m_pImgList->GetImageInfo(index,&imgInfo);
	return index;
}

void CCheckboxesTreeCtrl::OnPaint()
{
	CTreeCtrl::OnPaint();
	int n = GetVisibleCount();
  HTREEITEM hStart =	GetRootItem(); 
  
	m_bContinueScan = true;
	m_bStartFound = false;
  LOOPINFO lInfo;
  lInfo.pTree =  this;
	lInfo.hItem = hStart;
	lInfo.pParent = GetParent();
	lInfo.m_param1   = NULL;
  lInfo.m_nNumberOfCheckboxes = m_nNumberOfCheckboxes; 
  //DrawItem( &lInfo );
	//IterateItems( DrawItem,NULL,NULL,NULL );	
}

void CCheckboxesTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
  HTREEITEM hItem, hItemSel;

  if( GetKeyState(VK_MENU) < 0 ) // if Alt is pressed
    return;

  // set focus on this window 
  CTreeCtrl::SetFocus();

  // select clicked item
  hItem = HitTest(point);
  if( hItem )
  {

    //DEF11738 cannot invoke context menu for TC after changing the tabs
    hItemSel = GetSelectedItem();
    if (hItem != hItemSel)
    {
      //DEF11202 - check if selection was successful (validation of req fields)
      if( GetFocus()!=this )
        return;
    }
  }

  // check if there's any item selected
  hItem = GetSelectedItem();
  if( !hItem )
    return;

  EnsureVisible(hItem);


  if (!m_bContextMenu)
  {
    return;
  }
}

//DESCRIPTION:
//			To Create the disabled image for an item
//PARAM
//		bmp[out]		-	Resuting disabled image
void	CCheckboxesTreeCtrl::GenerateDisableImage( CBitmap& bmp )
{
	SetBitMapColor( bmp,::GetSysColor(COLOR_GRAYTEXT),RGB(0,128,128) );
}

//DESCRIPTION:
//			To set the color in the bitmap
//PARAM
//		bmp[out]		-	Resuting disabled image
//		color[in]		-	Color to be set for each pixel of the bitmap
//		excludeColor	-	Color to be excluded from setting
void	CCheckboxesTreeCtrl::SetBitMapColor( CBitmap& bmp,COLORREF color,COLORREF excludeColor)
{
	CClientDC dc(this);

	//Get bitmap size and height
	BITMAP bm;
	bmp.GetBitmap(&bm);

	WORD biBits = 32 * bm.bmPlanes;  

	//Get the info about the bitmap
	BITMAPINFO bi;
    ZeroMemory(&bi,sizeof(bi));
    bi.bmiHeader.biSize			= sizeof( BITMAPINFOHEADER );
    bi.bmiHeader.biHeight		= bm.bmHeight;
    bi.bmiHeader.biWidth		= bm.bmWidth;
    bi.bmiHeader.biPlanes		= bm.bmPlanes;
    bi.bmiHeader.biBitCount		= biBits;

	//Get the size of the bitmap in number of bytes.Pass 5th param as NULL.So 
	//that it will give  you the size
	GetDIBits( dc,bmp,0,bm.bmHeight,NULL,&bi,DIB_RGB_COLORS);

	//Next create the array of bytes to get the color values
	int nbBytes  = bi.bmiHeader.biSizeImage;
	LPBYTE pBits = new BYTE[ nbBytes ];

	//Get the color values from the bitmap
	GetDIBits( dc,bmp,0,bm.bmHeight,pBits,&bi,DIB_RGB_COLORS);

	int byteCtr = 0;

	if( bi.bmiHeader.biBitCount == 32 )
	{
		//if biBitCount = 32 then each pixel is represented by 32 bits.That is 4 bytes.
		//Highbyte is not used.RGB is in reversed order i.e, 2,1,0 and not 0,1,2
		for( ; byteCtr < nbBytes/4; byteCtr++ )
		{
			BYTE red	=	pBits[ byteCtr*4+2];
			BYTE green  =	pBits[ byteCtr*4+1];
			BYTE blue	=	pBits[ byteCtr*4+0];

			COLORREF col = RGB( red,green,blue );

			if( col != excludeColor )
			{
				pBits[ byteCtr*4+2] = GetRValue( color );
				pBits[ byteCtr*4+1] = GetGValue( color );
				pBits[ byteCtr*4+0] = GetBValue( color );					
			}
		}
		//Set the newly set color array to the bitmap
		SetDIBits( dc,bmp,0,bm.bmHeight,pBits,&bi,DIB_RGB_COLORS);
		delete pBits;
		pBits = NULL;	

	}
}	

//DESCRIPTION:
//			To get the bitmap of the image from the image list
//PARAM
//		iIndex[in]	-	Index of the image
//		bmp[out]	-	Resulting bitmap 
bool	CCheckboxesTreeCtrl::GetBitampFromImageList( int iIndex,CBitmap& bmp )
{
	//Get the image list of the tree control
	CImageList* pImageList = GetImageList(TVSIL_NORMAL);
	if( !pImageList )
		return false;

	//Get the count of images in the imagelist
	int iImgCt = pImageList->GetImageCount();
	if( iIndex < 0 || iIndex >= iImgCt )
		return false;

	//Get the size of image at the specified index
	int imgW,imgH;
	ImageList_GetIconSize( pImageList->GetSafeHandle(),&imgW,&imgH);

	//Extract the icon of the item
	HICON hIcon = pImageList->ExtractIcon(iIndex);

	//Convert the icon into bitmap
	GetBitmapFromIcon( hIcon,imgW,imgH,bmp);

	return true;
}

//DESCRIPTION:
//			To convert the icon into bitmap
//PARAM
//		hIcon[out]		-	Handle to the icon to be converted
//		ImgWidth[in]	-	Width of the icon
//		ImgHeight[in]	-	Height of the icon
bool	 CCheckboxesTreeCtrl::GetBitmapFromIcon( HICON hIcon,int ImgWidth,int ImgHeight,CBitmap& cBitmap )
{
	if( !hIcon )
		return false;

	//Create DC
	CClientDC dc(this);

	//Create memory dc
	CDC cMemDC;
	if ( !cMemDC.CreateCompatibleDC(&dc) )
		return false;

	//Create the bitmap of required size
	if(!cBitmap.CreateCompatibleBitmap(&dc, ImgWidth, ImgHeight))
		return false;

	//Select the bitmap into the memory DC
	CBitmap* pOldBmp = cMemDC.SelectObject(&cBitmap );

	//Fill the background with the masking color
	cMemDC.FillSolidRect(0,0,ImgWidth,ImgHeight, RGB(0, 128, 128));

	//Now draw the icon using DrawIconEx
	DrawIconEx(cMemDC.m_hDC, 0, 0, hIcon, ImgWidth, ImgHeight, 0, 0, DI_NORMAL);
	
	//Now restore the old bitmap
	cMemDC.SelectObject(pOldBmp );

	return true;

}

//DESCRIPTION:
//			To set the image index based on the state of the item
//PARAM
//		hItem[in]	-	Item to which the image to be set
void	CCheckboxesTreeCtrl::SetImage( HTREEITEM hItem )
{
	//Check for item
	if( !hItem )
		return;
	
	//Get the custom data
	PCUSTOMITEMDATA pCustData = (PCUSTOMITEMDATA) CTreeCtrl::GetItemData(hItem);
	if( !pCustData )
		return;

	//if the item checkboxes are enabled
	if( pCustData->m_bEnable || pCustData->m_bChecked)
	{
		SetCheckboxesState(hItem);
	}
	else 
	{
		//if the item checkboxes are disabled
		SetItemImage( hItem, pCustData->m_iDisableIndex,pCustData->m_iDisableIndex);
	}
	Invalidate();
}


//DESCRIPTION:
//			To set the image(checkboxes) index based on the state of the item checkboxes
//PARAM
//		hItem[in]	-	Item to which the image to be set
void	CCheckboxesTreeCtrl::SetCheckboxesState( HTREEITEM hItem )
{
	//Get the custom data
	PCUSTOMITEMDATA pCustData = (PCUSTOMITEMDATA) CTreeCtrl::GetItemData(hItem);
	if( !pCustData )
		return;

	int imgIndex = -1;
	//Check the item has the checkbox 
	if( pCustData->m_bChecked )
	{
		if (1 == m_nNumberOfCheckboxes)
		{
			if( TRUE == pCustData->m_bCheckState1  )
			{
				imgIndex = pCustData->m_iChecked;
			}
			else if ( FALSE == pCustData->m_bCheckState1 )
			{
	  			imgIndex = pCustData->m_iUnChecked;
			}
		}
		else if (2 == m_nNumberOfCheckboxes)
		{
			if( TRUE == pCustData->m_bCheckState1 && FALSE == pCustData->m_bCheckState2 )
			{
				imgIndex = pCustData->m_iCheckedUnChecked;
			}
			else if ( FALSE == pCustData->m_bCheckState1 && FALSE == pCustData->m_bCheckState2 )
			{
	  			imgIndex = pCustData->m_iUnCheckedUnChecked;
			}
			else if ( FALSE == pCustData->m_bCheckState1 && TRUE == pCustData->m_bCheckState2 )
			{
	  			imgIndex = pCustData->m_iUnCheckedChecked;
			}
			else if ( TRUE == pCustData->m_bCheckState1 && TRUE == pCustData->m_bCheckState2 )
			{
	  			imgIndex = pCustData->m_iCheckedChecked;
			}
		}
		else if (3 == m_nNumberOfCheckboxes)
		{
		    if( TRUE == pCustData->m_bCheckState1 && FALSE == pCustData->m_bCheckState2 && FALSE == pCustData->m_bCheckState3)
			{
				imgIndex = pCustData->m_iCheckedUnCheckedUnChecked;
			}
			else if ( FALSE == pCustData->m_bCheckState1 && FALSE == pCustData->m_bCheckState2 && FALSE == pCustData->m_bCheckState3)
			{
	  			imgIndex = pCustData->m_iUnCheckedUnCheckedUnChecked;
			}
			else if ( FALSE == pCustData->m_bCheckState1 && TRUE == pCustData->m_bCheckState2 && FALSE == pCustData->m_bCheckState3)
			{
	  			imgIndex = pCustData->m_iUnCheckedCheckedUnChecked;
			}
			else if ( TRUE == pCustData->m_bCheckState1 && TRUE == pCustData->m_bCheckState2 && FALSE == pCustData->m_bCheckState3)
			{
	  			imgIndex = pCustData->m_iCheckedCheckedUnChecked;
			}
			else if( TRUE == pCustData->m_bCheckState1 && FALSE == pCustData->m_bCheckState2 && TRUE == pCustData->m_bCheckState3)
			{
				imgIndex = pCustData->m_iCheckedUnCheckedChecked;
			}
			else if ( FALSE == pCustData->m_bCheckState1 && FALSE == pCustData->m_bCheckState2 && TRUE == pCustData->m_bCheckState3)
			{
	  			imgIndex = pCustData->m_iUnCheckedUnCheckedChecked;
			}
			else if ( FALSE == pCustData->m_bCheckState1 && TRUE == pCustData->m_bCheckState2 && TRUE == pCustData->m_bCheckState3)
			{
	  			imgIndex = pCustData->m_iUnCheckedCheckedChecked;
			}
			else if ( TRUE == pCustData->m_bCheckState1 && TRUE == pCustData->m_bCheckState2 && TRUE == pCustData->m_bCheckState3)
			{
	  			imgIndex = pCustData->m_iCheckedCheckedChecked;
			}
		}
		else if (4 == m_nNumberOfCheckboxes)
		{
		    if( TRUE == pCustData->m_bCheckState1 && FALSE == pCustData->m_bCheckState2 && 
			  FALSE == pCustData->m_bCheckState3 && FALSE == pCustData->m_bCheckState4)
			{
				imgIndex = pCustData->m_iChkUnChkUnChkUnChk;
			}
			else if ( FALSE == pCustData->m_bCheckState1 && FALSE == pCustData->m_bCheckState2 && 
			  FALSE == pCustData->m_bCheckState3 && FALSE == pCustData->m_bCheckState4)
			{
	  			imgIndex = pCustData->m_iUnChkUnChkUnChkUnChk;
			}
			else if ( FALSE == pCustData->m_bCheckState1 && TRUE == pCustData->m_bCheckState2 && 
			  FALSE == pCustData->m_bCheckState3 && FALSE == pCustData->m_bCheckState4)
			{
	  			imgIndex = pCustData->m_iUnChkChkUnChkUnChk;
			}
			else if ( TRUE == pCustData->m_bCheckState1 && TRUE == pCustData->m_bCheckState2 && 
              FALSE == pCustData->m_bCheckState3 && FALSE == pCustData->m_bCheckState4)
			{
	  			imgIndex = pCustData->m_iChkChkUnChkUnChk;
			}
			else if( TRUE == pCustData->m_bCheckState1 && FALSE == pCustData->m_bCheckState2 && 
			  TRUE == pCustData->m_bCheckState3 && FALSE == pCustData->m_bCheckState4)
			{
				imgIndex = pCustData->m_iChkUnChkChkUnChk;
			}
			else if ( FALSE == pCustData->m_bCheckState1 && FALSE == pCustData->m_bCheckState2 && 
			  TRUE == pCustData->m_bCheckState3 && FALSE == pCustData->m_bCheckState4)
			{
	  			imgIndex = pCustData->m_iUnChkUnChkChkUnChk;
			}
			else if ( FALSE == pCustData->m_bCheckState1 && TRUE == pCustData->m_bCheckState2 && 
		      TRUE == pCustData->m_bCheckState3 && FALSE == pCustData->m_bCheckState4)
			{
	  			imgIndex = pCustData->m_iUnChkChkChkUnChk;
			}
			else if ( TRUE == pCustData->m_bCheckState1 && TRUE == pCustData->m_bCheckState2 && 
			  TRUE == pCustData->m_bCheckState3 && FALSE == pCustData->m_bCheckState4)
			{
	  			imgIndex = pCustData->m_iChkChkChkUnChk;
			}
			else if( TRUE == pCustData->m_bCheckState1 && FALSE == pCustData->m_bCheckState2 && 
			  FALSE == pCustData->m_bCheckState3 && TRUE == pCustData->m_bCheckState4)
			{
				imgIndex = pCustData->m_iChkUnChkUnChkChk;
			}
			else if ( FALSE == pCustData->m_bCheckState1 && FALSE == pCustData->m_bCheckState2 && 
			  FALSE == pCustData->m_bCheckState3 && TRUE == pCustData->m_bCheckState4)
			{
	  			imgIndex = pCustData->m_iUnChkUnChkUnChkChk;
			}
			else if ( FALSE == pCustData->m_bCheckState1 && TRUE == pCustData->m_bCheckState2 && 
			  FALSE == pCustData->m_bCheckState3 && TRUE == pCustData->m_bCheckState4)
			{
	  			imgIndex = pCustData->m_iUnChkChkUnChkChk;
			}
			else if ( TRUE == pCustData->m_bCheckState1 && TRUE == pCustData->m_bCheckState2 && 
			  FALSE == pCustData->m_bCheckState3 && TRUE == pCustData->m_bCheckState4)
			{
	  			imgIndex = pCustData->m_iChkChkUnChkChk;
			}
			if( TRUE == pCustData->m_bCheckState1 && FALSE == pCustData->m_bCheckState2 && 
			  TRUE == pCustData->m_bCheckState3 && TRUE == pCustData->m_bCheckState4)
			{
				imgIndex = pCustData->m_iChkUnChkChkChk;
			}
			else if ( FALSE == pCustData->m_bCheckState1 && FALSE == pCustData->m_bCheckState2 && 
			  TRUE == pCustData->m_bCheckState3 && TRUE == pCustData->m_bCheckState4)
			{
	  			imgIndex = pCustData->m_iUnChkUnChkChkChk;
			}
			else if ( FALSE == pCustData->m_bCheckState1 && TRUE == pCustData->m_bCheckState2 && 
			  TRUE == pCustData->m_bCheckState3 && TRUE == pCustData->m_bCheckState4)
			{
	  			imgIndex = pCustData->m_iUnChkChkChkChk;
			}
			else if ( TRUE == pCustData->m_bCheckState1 && TRUE == pCustData->m_bCheckState2 && 
			  TRUE == pCustData->m_bCheckState3 && TRUE == pCustData->m_bCheckState4)
			{
	  			imgIndex = pCustData->m_iChkChkChkChk;
			}
		}
	}

  SetItemImage( hItem, imgIndex,imgIndex);
}


int	CCheckboxesTreeCtrl::IsNormalCheckedExists( UINT uBmpID )
{
	return 0;
}

void CCheckboxesTreeCtrl::SetMoveDraggedItem( bool flag )
{
	m_bMoveItem = flag;	
}

void CCheckboxesTreeCtrl::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
}

//DESCRIPTION:
//			To set number of checboxes for single item
//PARAM
//		nNumberOfCheckboxes[in]	-	number of checkboxes (max. 4)
void CCheckboxesTreeCtrl::SetNumberOfCheckboxes(int nNumberOfCheckboxes)
{
  if (nNumberOfCheckboxes > 4)
  {
    m_nNumberOfCheckboxes = 4;
  }
  else
  {
    m_nNumberOfCheckboxes = nNumberOfCheckboxes;
  }
}

void CCheckboxesTreeCtrl::EnableTooltipEx(bool bEnable/* = true*/)
{
  m_bDescriptionTS = bEnable;
}

void CCheckboxesTreeCtrl::ShowToolTip(CString szToolTip)
{
  m_ToolTip.Hide();

  CRect rect;
  CRect rectHover;
  CRect temp;
  GetClientRect(&rect);
  int nWidth = rect.Width() + (GetSystemMetrics(SM_CXVSCROLL) / 4);
  tagPOINT lPoint;
  GetCursorPos(&lPoint);
  this->GetWindowRect(&temp);
  rect.bottom = (lPoint.y - temp.top + 16);
  rect.left = (lPoint.x - temp.left + 16);

  rectHover.left = 0;
  rectHover.right = GetSystemMetrics(SM_CXFULLSCREEN);
  rectHover.top = 0;
  rectHover.bottom = GetSystemMetrics(SM_CYFULLSCREEN);

  COLORREF crTitleTipTextClr;
  COLORREF crTitleTipBackClr;
  crTitleTipTextClr = GetSysColor(COLOR_INFOTEXT);
  crTitleTipBackClr = GetSysColor(COLOR_INFOBK);

  m_ToolTip.Show(rect, szToolTip, 0,
  rectHover, NULL, crTitleTipTextClr, crTitleTipBackClr, TRUE );
}

void CCheckboxesTreeCtrl::SetToolTip(HTREEITEM hItem, CString strToolTipText)
{
  if ("" != strToolTipText && " " != strToolTipText)
  {
    m_strTmp.m_hItem = hItem;
    m_strTmp.m_szDesc = strToolTipText;
    m_ArrayDesc.Add(m_strTmp);
  }
}

CString CCheckboxesTreeCtrl::GetToolTipText(HTREEITEM hItem)
{
    for(int i = 0; i < m_ArrayDesc.GetSize(); i++)
    {
      if(hItem == m_ArrayDesc.GetAt(i).m_hItem)
      {
        return m_ArrayDesc.GetAt(i).m_szDesc;
      }
    }
    return "";
}


LRESULT CCheckboxesTreeCtrl::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
  m_bMouseOver = FALSE;
  m_ToolTip.Hide();
  return 0;
}

LRESULT CCheckboxesTreeCtrl::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
  if(!m_bDescriptionTS)
  {
    return 0;
  }
  TRACKMOUSEEVENT tme;
  tme.cbSize = sizeof (tme);
  tme.dwFlags = TME_HOVER | TME_LEAVE;
  tme.hwndTrack = m_hWnd;
  tme.dwHoverTime = 0;
  _TrackMouseEvent (&tme);
  //
  // Get the nearest item in the list box to the mouse cursor
  //
  CPoint point;
  point.x = LOWORD (lParam);
  point.y  = HIWORD (lParam);

  HTREEITEM hItem = HitTest(point);
  if (NULL == hItem)
  {
    return 0;
  }

  if(m_hItemPrev != hItem )
  {
    m_hItemPrev = hItem;
    
    int nSize = m_ArrayDesc.GetSize();
	  int i = 0;
    for(i = 0 ; i < nSize ; i++)
    {
      if( m_ArrayDesc.GetAt(i).m_hItem == hItem)
        break;
    }
    if (i >= nSize)
    {
      return 0;
    }
    
    CString szTmp;
    szTmp.Format("%s", m_ArrayDesc.GetAt(i).m_szDesc);
    ShowToolTip(szTmp);
  }

  return 0;
}

void CCheckboxesTreeCtrl::ExpandItemBranch(HTREEITEM hTreeItem, BOOL bExpand/*=TRUE*/)
{
  HTREEITEM hParent, hRoot;
  hRoot = hParent = hTreeItem;
  hParent = this->GetNextItem (hParent,TVGN_PARENT);
  while(NULL != hParent)
  {
    CTreeCtrl::Expand(hParent,TVE_EXPAND);
    hParent = this->GetNextItem (hParent,TVGN_PARENT);
    if(hRoot == hParent)
      hParent = NULL;
  }
}

void CCheckboxesTreeCtrl::ExpandTreeBranch(HTREEITEM hTreeItem, BOOL bExpand/*=TRUE*/)
{
  if( hTreeItem  && ItemHasChildren(hTreeItem))
  {
    HTREEITEM hChild, hRoot, hNext;

    hRoot = hTreeItem;
    hChild = this->GetChildItem(hRoot);

    while(hChild)
    {
      // expand item
      this->Expand(hTreeItem, bExpand?TVE_EXPAND:TVE_COLLAPSE);
      hNext = this->GetChildItem(hChild);
      if(hNext)
      {
        ExpandTreeBranch(hChild,bExpand);
      }
      hChild = this->GetNextItem (hChild,TVGN_NEXT);
    }
  }
}

void CCheckboxesTreeCtrl::SendMessageToParent(int code,HTREEITEM target,HTREEITEM source)
{
  NM_TREEVIEW nmtvMessage;
  nmtvMessage.hdr.hwndFrom = m_hWnd;
  nmtvMessage.hdr.idFrom   = GetDlgCtrlID();
	nmtvMessage.hdr.code     = code;
	nmtvMessage.itemNew.hItem = target;

  CWnd* pWnd = GetOwner();
  if( pWnd && pWnd->m_hWnd )
    pWnd->SendMessage(WM_NOTIFY, nmtvMessage.hdr.idFrom, (LPARAM)&nmtvMessage);
}

void CCheckboxesTreeCtrl::SortTreeItem(HTREEITEM hItem)
{ 
  if(hItem != NULL)
  {
      if(hItem == TVI_ROOT || this->ItemHasChildren(hItem))
      {
         HTREEITEM child = this->GetChildItem(hItem);

         while(child != NULL)
         {
            SortTreeItem(child);
            child = this->GetNextItem(child, TVGN_NEXT);
         }

         TVSORTCB tvs;
         tvs.hParent = hItem;
         tvs.lpfnCompare = MyCompareProc;
         tvs.lParam = reinterpret_cast<LPARAM>(this);
         SortChildrenCB(&tvs); 
      }
  }
}

void CCheckboxesTreeCtrl::ClearSelection()
{
  int nSelItemCount = m_vecSelectedItems.size();
  for( int nIdx = 0; nIdx < nSelItemCount ; ++nIdx )
  {
    SetItemState(m_vecSelectedItems[nIdx],0,TVIS_SELECTED);
  }
  m_vecSelectedItems.clear();
}

void CCheckboxesTreeCtrl::AddSelected(HTREEITEM hItem)
{
  SEL_ITEM_LIST::iterator itr;
  for( itr = m_vecSelectedItems.begin(); itr != m_vecSelectedItems.end(); ++itr )
  {
    if( (*itr) == hItem )
    {
      return;
    }
  }  
  m_vecSelectedItems.push_back(hItem);
}

void CCheckboxesTreeCtrl::ClearAllSetCheckboxes()
{
  for (int i = m_arrStructItemsTree.GetSize()-1; i >= 0 ; i-- )
  {
    PCUSTOMITEMDATA pCustData = (PCUSTOMITEMDATA) CTreeCtrl::GetItemData(m_arrStructItemsTree.GetAt(i).hItem);
    pCustData->m_bCheckState1 = false;
    pCustData->m_bCheckState2 = false;
    pCustData->m_bCheckState3 = false;
    pCustData->m_bCheckState4 = false;
    SetCheckboxesState(m_arrStructItemsTree.GetAt(i).hItem);
  }
}

void CCheckboxesTreeCtrl::SelectFocusItem(HTREEITEM hClickedItem)
{
  if (NULL != hClickedItem)
  {
    CTreeCtrl::SetFocus();
    ClearSelection();
    AddSelected(hClickedItem);
    SetItemState(hClickedItem,TVIS_SELECTED,TVIS_SELECTED);
	  CTreeCtrl::SelectItem(hClickedItem);
  }
}