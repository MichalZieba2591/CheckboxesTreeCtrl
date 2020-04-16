#if !defined(AFX_CHECKBOXESTREECTRL_H__234C8D10_F2CD_42B3_9972_384F6609F5D8__INCLUDED_)
#define AFX_CHECKBOXESTREECTRL_H__234C8D10_F2CD_42B3_9972_384F6609F5D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CCheckboxesTreeCtrl.h : header file
//
#include "TitleTip.h"
#include "TreeItem.h"
#include <vector>
#include <set>
#include <map>
using namespace std;

#define TVN_SELECTION_CHANGED		300
#define TVN_STATEICON_CLICKED		301
#define TVN_ITEM_CHECK_TOGGLE		302

#define TVN_ITEM_CHECK_ONE		303
#define TVN_ITEM_CHECK_TWO		304
#define TVN_ITEM_CHECK_THREE	305
#define TVN_ITEM_CHECK_FOUR		306


class CCheckboxesTreeCtrl;						 

typedef std::vector<HTREEITEM> SEL_ITEM_LIST;
typedef SEL_ITEM_LIST::iterator SEL_ITEM_LIST_IT;
//*************************************************************************
//THIS STRUCTURE IS SENT WHEN THE ITEMS OF THE TREE ARE SCANNED.
//SCAN IS NOTHING BUT THE TRAVERSING ALL THE ITEMS OF THE TREE
typedef struct __LOOPINFO
{
	CCheckboxesTreeCtrl* pTree;		//Pointer to the tree control
	HTREEITEM hItem;			//Current traversed item
	CWnd*	  pParent;			//Parent to the tree control usually the dialog
	void*	  m_param1;			//User specific information
	void*     m_param2;			//User specific information
	void*	  m_pResult;		//Return value if any
  int   m_nNumberOfCheckboxes;

}LOOPINFO,*PLOOPINFO;
//*************************************************************************

//TYPEDEF OF CALLBACK FUNCTION
typedef int  (*ScanCallBackFunc) (LOOPINFO* pLoopInfo );

//CUSTOM DATA WHICH IS ASSOCIATED WITH EACH TREE ITEM.THIS CONTAIN LOT OF INFORMATION
//OF THE ITEMS SUCH AS ITS NORMAL IMAGE,EXPANDED IMAGE,MASK FOR THEM,CHECKBOX IS REQUIRED ETC
typedef struct __CUSTOMITEMDATA
{
	__CUSTOMITEMDATA()
	{
		m_cMaskColor			= RGB(0,0,0);//RGB(0,128,128);
    m_ll_ID            = -1;
		m_llCode          = -1;
    m_bEditable				= false;
		m_bDropTarget			= true;
		m_bChecked				= true;
		m_bCheckState1			= false;
    m_bCheckState2			= false;
    m_bCheckState3			= false;
    m_bCheckState4			= false;

		m_iUnChecked							= -1;
		m_iChecked						    = -1;

		    //Bitmap for 2 checkbox
			m_iUnCheckedUnChecked			= -1;
			m_iCheckedUnChecked			    = -1;
			m_iUnCheckedChecked			    = -1;
			m_iCheckedChecked	            = -1;
			//Bitmap for 3 checkbox
			m_iUnCheckedUnCheckedUnChecked  = -1;			
			m_iCheckedUnCheckedUnChecked    = -1;	
			m_iUnCheckedCheckedUnChecked	= -1;	
			m_iCheckedCheckedUnChecked      = -1;
			m_iUnCheckedUnCheckedChecked	= -1; 		
			m_iCheckedUnCheckedChecked		= -1;
			m_iUnCheckedCheckedChecked		= -1;
			m_iCheckedCheckedChecked        = -1;
      //Bitmap for 4 checkbox
      m_iUnChkUnChkUnChkUnChk   =-1;			
		  m_iChkUnChkUnChkUnChk		  =-1;
		  m_iUnChkChkUnChkUnChk		  =-1;
		  m_iChkChkUnChkUnChk       =-1;
		  m_iUnChkUnChkChkUnChk			=-1;
		  m_iChkUnChkChkUnChk		    =-1;
		  m_iUnChkChkChkUnChk		    =-1;
		  m_iChkChkChkUnChk         =-1;
      m_iUnChkUnChkUnChkChk			=-1;
		  m_iChkUnChkUnChkChk	    	=-1;
		  m_iUnChkChkUnChkChk	    	=-1;
		  m_iChkChkUnChkChk         =-1;
		  m_iUnChkUnChkChkChk			  =-1;
		  m_iChkUnChkChkChk     		=-1;
		  m_iUnChkChkChkChk	      	=-1;
		  m_iChkChkChkChk           =-1;

		m_cItemColor									= RGB(255,255,255);
		m_bIsBold										= false;
		m_uMenuID									= -1;
		m_bEnable										= true;
		m_iDisableIndex							= -1;
		m_bExpStateBefDisable				= false;
	}

	__CUSTOMITEMDATA(__CUSTOMITEMDATA& src )
	{
    m_ll_ID                     = src.m_ll_ID;
		m_llCode                    = src.m_llCode;		
    m_bEditable									  = src.m_bEditable;
		m_bDropTarget								= src.m_bDropTarget;
		m_bChecked									= src.m_bChecked;
		m_bCheckState1							= src.m_bCheckState1;
    m_bCheckState2							= src.m_bCheckState2;
    m_bCheckState3							= src.m_bCheckState3;
    m_bCheckState4							= src.m_bCheckState4;
		m_iUnChecked							  = src.m_iUnChecked;
		m_iChecked		         		  = src.m_iChecked;

		m_iUnCheckedUnChecked				= src.m_iUnCheckedUnChecked;
		m_iCheckedUnChecked			    = src.m_iCheckedUnChecked;
		m_iUnCheckedChecked			    = src.m_iUnCheckedChecked;
		m_iCheckedChecked	          = src.m_iCheckedChecked;

		m_iUnCheckedUnCheckedUnChecked  = src.m_iUnCheckedUnCheckedUnChecked;			
		m_iCheckedUnCheckedUnChecked    = src.m_iCheckedUnCheckedUnChecked;	
		m_iUnCheckedCheckedUnChecked	= src.m_iUnCheckedCheckedUnChecked;	
		m_iCheckedCheckedUnChecked      = src.m_iCheckedCheckedUnChecked;
		m_iUnCheckedUnCheckedChecked	= src.m_iUnCheckedUnCheckedChecked; 		
		m_iCheckedUnCheckedChecked		= src.m_iCheckedUnCheckedChecked;
		m_iUnCheckedCheckedChecked		= src.m_iUnCheckedCheckedChecked;
		m_iCheckedCheckedChecked        = src.m_iCheckedCheckedChecked;

    m_iUnChkUnChkUnChkUnChk   =src.m_iUnChkUnChkUnChkUnChk;			
		m_iChkUnChkUnChkUnChk		  =src.m_iChkUnChkUnChkUnChk;
    m_iUnChkChkUnChkUnChk		  =src.m_iUnChkChkUnChkUnChk;
		m_iChkChkUnChkUnChk       =src.m_iChkChkUnChkUnChk;
	  m_iUnChkUnChkChkUnChk			=src.m_iUnChkUnChkChkUnChk;
	  m_iChkUnChkChkUnChk		    =src.m_iChkUnChkChkUnChk;
	  m_iUnChkChkChkUnChk		    =src.m_iUnChkChkChkUnChk;
	  m_iChkChkChkUnChk         =src.m_iChkChkChkUnChk;
    m_iUnChkUnChkUnChkChk			=src.m_iUnChkUnChkUnChkChk;
	  m_iChkUnChkUnChkChk	    	=src.m_iChkUnChkUnChkChk;
		m_iUnChkChkUnChkChk	    	=src.m_iUnChkChkUnChkChk;
	  m_iChkChkUnChkChk         =src.m_iChkChkUnChkChk;
	  m_iUnChkUnChkChkChk			  =src.m_iUnChkUnChkChkChk;
	  m_iChkUnChkChkChk     		=src.m_iChkUnChkChkChk;
	  m_iUnChkChkChkChk	      	=src.m_iUnChkChkChkChk;
	  m_iChkChkChkChk           =src.m_iChkChkChkChk;

		m_cItemColor								= src.m_cItemColor;
		m_bIsBold										= src.m_bIsBold;
		m_uMenuID									  = src.m_uMenuID;
		m_bEnable										= src.m_bEnable;
		m_iDisableIndex							= src.m_iDisableIndex;
		m_bExpStateBefDisable				= src.m_bExpStateBefDisable;
	}

	void operator = ( __CUSTOMITEMDATA& rhs )
	{
    m_ll_ID                     = rhs.m_ll_ID;
		m_llCode                    = rhs.m_llCode;	
		m_bEditable						= rhs.m_bEditable;
		m_bDropTarget					= rhs.m_bDropTarget;
		m_bChecked						= rhs.m_bChecked;
		m_bCheckState1					= rhs.m_bCheckState1;
    m_bCheckState2					= rhs.m_bCheckState2;
    m_bCheckState3					= rhs.m_bCheckState3;
    m_bCheckState4					= rhs.m_bCheckState4;

		m_iUnChecked							= rhs.m_iUnChecked;
		m_iChecked						    = rhs.m_iChecked;

		m_iUnCheckedUnChecked			= rhs.m_iUnCheckedUnChecked;
		m_iCheckedUnChecked			    = rhs.m_iCheckedUnChecked;
		m_iUnCheckedChecked			    = rhs.m_iUnCheckedChecked;
		m_iCheckedChecked               = rhs.m_iCheckedChecked;
		
		m_iUnCheckedUnCheckedUnChecked  = rhs.m_iUnCheckedUnCheckedUnChecked;			
		m_iCheckedUnCheckedUnChecked    = rhs.m_iCheckedUnCheckedUnChecked;	
		m_iUnCheckedCheckedUnChecked	= rhs.m_iUnCheckedCheckedUnChecked;	
		m_iCheckedCheckedUnChecked      = rhs.m_iCheckedCheckedUnChecked;
		m_iUnCheckedUnCheckedChecked	= rhs.m_iUnCheckedUnCheckedChecked; 		
		m_iCheckedUnCheckedChecked		= rhs.m_iCheckedUnCheckedChecked;
		m_iUnCheckedCheckedChecked		= rhs.m_iUnCheckedCheckedChecked;
		m_iCheckedCheckedChecked        = rhs.m_iCheckedCheckedChecked;

    m_iUnChkUnChkUnChkUnChk   =rhs.m_iUnChkUnChkUnChkUnChk;			
		m_iChkUnChkUnChkUnChk		  =rhs.m_iChkUnChkUnChkUnChk;
    m_iUnChkChkUnChkUnChk		  =rhs.m_iUnChkChkUnChkUnChk;
		m_iChkChkUnChkUnChk       =rhs.m_iChkChkUnChkUnChk;
	  m_iUnChkUnChkChkUnChk			=rhs.m_iUnChkUnChkChkUnChk;
	  m_iChkUnChkChkUnChk		    =rhs.m_iChkUnChkChkUnChk;
	  m_iUnChkChkChkUnChk		    =rhs.m_iUnChkChkChkUnChk;
	  m_iChkChkChkUnChk         =rhs.m_iChkChkChkUnChk;
    m_iUnChkUnChkUnChkChk			=rhs.m_iUnChkUnChkUnChkChk;
	  m_iChkUnChkUnChkChk	    	=rhs.m_iChkUnChkUnChkChk;
		m_iUnChkChkUnChkChk	    	=rhs.m_iUnChkChkUnChkChk;
	  m_iChkChkUnChkChk         =rhs.m_iChkChkUnChkChk;
	  m_iUnChkUnChkChkChk			  =rhs.m_iUnChkUnChkChkChk;
	  m_iChkUnChkChkChk     		=rhs.m_iChkUnChkChkChk;
	  m_iUnChkChkChkChk	      	=rhs.m_iUnChkChkChkChk;
	  m_iChkChkChkChk           =rhs.m_iChkChkChkChk;

		m_cItemColor								= rhs.m_cItemColor;
		m_bIsBold										= rhs.m_bIsBold;
		m_uMenuID									  = rhs.m_uMenuID;
		m_bEnable										= rhs.m_bEnable;
		m_iDisableIndex							= rhs.m_iDisableIndex;
		m_bExpStateBefDisable				= rhs.m_bExpStateBefDisable;
	}



	//EXPOSED DATA: TO BE SET BY THE USER.
	//####################################

		//Resource of the images for normal and expanded states.Remember these are the bitmap
		//ID's and not the indice.
		COLORREF		m_cMaskColor;							//If the mask image is not given this can be considered as the mask color
    LONGLONG    m_ll_ID;                  //ID items
		LONGLONG    m_llCode;                 //Code from AL_CHILDREN column, which is tree list sorted

		bool					m_bEditable;						//Flag to indicate whether the item is editable or not
		bool					m_bDropTarget;				//Is the item is valid target for dropping operation
		bool					m_bChecked;						//Whether the item has to have CHECKBOXES or NOT
		bool					m_bCheckState1;					//If checkbox is needed, state of it [CHECKED/UNCHECKED]
    bool					m_bCheckState2;
    bool					m_bCheckState3;
    bool					m_bCheckState4;
		COLORREF		m_cItemColor;					//Color of the text of the item
		bool					m_bIsBold;							//Item to be displayed in bold font
		UINT				m_uMenuID;						//Context menu id given to the item
		bool					m_bEnable;							//Enable or disable tree item
		bool					m_bExpStateBefDisable;	//True if the item is in expanded state before disabling, false otherwise


	//Indice of the images representing various state from the image list
	//-------------------------------------------------------------------
			//Normal State with NO CHECKBOX
			int m_iUnChecked;
			int	m_iChecked;

			//Normal State for 2 CHECKBOX
			int	  m_iUnCheckedUnChecked;			
			int	  m_iCheckedUnChecked;		
			int	  m_iUnCheckedChecked;		
			int	  m_iCheckedChecked;	

			//Normal State for 3 CHECKBOX
			int	  m_iUnCheckedUnCheckedUnChecked;			
			int	  m_iCheckedUnCheckedUnChecked;		
			int	  m_iUnCheckedCheckedUnChecked;		
			int	  m_iCheckedCheckedUnChecked;
			int	  m_iUnCheckedUnCheckedChecked;			
			int	  m_iCheckedUnCheckedChecked;		
			int	  m_iUnCheckedCheckedChecked;		
			int	  m_iCheckedCheckedChecked;

      //Normal State for 4 CHECKBOX
			int	  m_iUnChkUnChkUnChkUnChk;			
			int	  m_iChkUnChkUnChkUnChk;		
			int	  m_iUnChkChkUnChkUnChk;		
			int	  m_iChkChkUnChkUnChk;
			int	  m_iUnChkUnChkChkUnChk;			
			int	  m_iChkUnChkChkUnChk;		
			int	  m_iUnChkChkChkUnChk;		
			int	  m_iChkChkChkUnChk;
      int	  m_iUnChkUnChkUnChkChk;			
			int	  m_iChkUnChkUnChkChk;		
			int	  m_iUnChkChkUnChkChk;		
			int	  m_iChkChkUnChkChk;
			int	  m_iUnChkUnChkChkChk;			
			int	  m_iChkUnChkChkChk;		
			int	  m_iUnChkChkChkChk;		
			int	  m_iChkChkChkChk;

			//Disabled image index
			int	  m_iDisableIndex;	//Index of the image for the disable state


}CUSTOMITEMDATA,*PCUSTOMITEMDATA;

//*************************************************************************


//INSERT STRUCTURE USED TO INSERT THE ITEM IN THE TREE.SIMILAR TO TVINSERTSTRUCT
//USED IN THE CTreeCtrl WITH ADDITIONAL INFORMATION OF CUSTOM DATA
typedef struct __CUSTOMINSERTSTRUCT
{
	__CUSTOMINSERTSTRUCT()
	{
		m_pCustomData = NULL;
	}
	~__CUSTOMINSERTSTRUCT()
	{
		delete m_pCustomData;
	}

	TVINSERTSTRUCT		m_tvIS;			//Insert structure of the CTreeCtrl
	PCUSTOMITEMDATA		m_pCustomData;	//Custom data to be used with the item
	
}CUSTOMINSERTSTRUCT,*PCUSTOMINSERTSTRUCT;

//*************************************************************************

//CUSTOM STRUCTURE WHICH GIVES INFORMATION ABOUT THE NOTIFICATIION MESSAGE.
//SAME AS OF NMHDR OF CTreeCtrl WITH ADDITIONAL INFORMATION
typedef struct _CUSTNMHDR
{
	NMHDR		m_hdr;		//Info about the notification	
	HTREEITEM	m_hItem;	//Tree item which caused the notification
	void*		m_data;		//User specific data,if any
}CUSTNMHDR,*LPCUSTNMHDR;

static int CALLBACK MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);


//*************************************************************************

/////////////////////////////////////////////////////////////////////////////
// CCheckboxesTreeCtrl window

class CCheckboxesTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CCheckboxesTreeCtrl();
  ~CCheckboxesTreeCtrl();
// Attributes
public:

	//Dragging Related Info
	//---------------------
    int m_nNumberOfCheckboxes;            //Number of checkboxes display for single item

    bool						m_bMoveItem;				  //When dropped on the target the source item will be moved
    bool            m_bIsBandingON;				//Is Banding selection is ON/OFF
	bool	          m_bOldItemSelected;   //Flag to indicate the selected items matches with the previous selected items
    bool					  m_bStartFound;			  //Flag used for scan operation
	bool				    m_bAllowExpand;
    bool	          m_bContinueScan;
    BOOL m_bContextMenu;

		HTREEITEM		m_hDragSourceItem;	      //Item which is getting dragged
		HTREEITEM		m_hDragTargetItem;		    //Target item on which the dragged to be dropped
		HTREEITEM		m_hOrigTgt;
    HTREEITEM	  m_hRClickItem;

		CPoint    m_startPt,m_endPt;											//Start point and end point of the banding rectangle
		CArray<HTREEITEM,HTREEITEM>	m_aItemsInRect;	//Items falls inside the banding rectangle
    
    struct structItems
    {  
      HTREEITEM m_hItem;
      CString m_szDesc;    
    };
    CArray<structItems,structItems> m_ArrayDesc;
    structItems m_strTmp;

		HCURSOR   m_defaultCursor,m_noCursor;						//Cursors to indicate the banding status

		std::vector<HTREEITEM> m_vecSelectedItems; 		//vector of selected items 
		std::vector<HTREEITEM> m_vecDraggedItems;		  //vector of items being dragged for drop operation
		std::set<HTREEITEM> m_vecCheckedItems;				  //vector of items with check box is ON

		CImageList*			m_pImgList;				//Image list associated with the tree
		CImageList*			m_pDragImageList;	//Image list to be used for dragging

		std::map<UINT, std::vector<int> > m_mapBitmapID;

		COLORREF	m_cMaskColor;
	

// Operations
public:

    struct structItemsTree
    {
      HTREEITEM hItem;
      LONGLONG llData;
      BOOL bEmptyChild;
      PCUSTOMINSERTSTRUCT* lpInsertStruct;
      PCUSTOMITEMDATA* pCustomData;
    };
    CArray<structItemsTree, structItemsTree> m_arrStructItemsTree; 
    PCUSTOMITEMDATA      m_pArrItemsTree1;
    PCUSTOMINSERTSTRUCT  m_pArrItemsTree2;

    void SetToolTip(HTREEITEM hItem, CString strToolTipText);
    CString GetToolTipText(HTREEITEM hItem);
    void EnableTooltipEx(bool bEnable = true);
    void ExpandTreeBranch(HTREEITEM hTreeItem, BOOL bExpand=TRUE);
    void ExpandItemBranch(HTREEITEM hTreeItem, BOOL bExpand=TRUE);
    void DeleteEmptyItem(HTREEITEM hItem);
    void SendMessageToParent(int code,HTREEITEM target = NULL,HTREEITEM source =NULL );
    HTREEITEM GetNextItemSmart(HTREEITEM hItem);
    HTREEITEM GetItemWithText(CString szText);
    HTREEITEM GetItemByData(LONGLONG llAAID);
    LONGLONG GetItemDataID(HTREEITEM hItem);


    virtual CWnd* SetFocus();

    void SetBackgroundColor( COLORREF bkColor );          //To set the background color for the tree control
    void SetNumberOfCheckboxes(int nNumberOfCheckboxes);  //Set number of checkboxes (max 4.)
		
	//Helper function used to Show the item information
	void ShowItemInfo(HTREEITEM hItem );

	//To set the bold font for the label
	void SetBoldFont( HTREEITEM hItem );	//[in]	-	Item to which the font to be set as bold

	//To set the color for the item text
	void SetItemColor( HTREEITEM hItem,	COLORREF col );

	//To enable/disable the item
	void EnableItem( HTREEITEM hItem, bool	bEnable	);

	//To expand the entire tree
	void ExpandTree(HTREEITEM hItem,bool Expand = true);	//[in]	- Item to be expanded
    void ExpandTreeRoot(bool bExpand = true); // Expand all tree
    
    HTREEITEM InsertItem(UINT nMask, LPCTSTR lpszItem, int nImage, int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam, HTREEITEM hParent, HTREEITEM hInsertAfter);
    HTREEITEM InsertItem(LPCTSTR lpszItem, CTreeItem tiItem, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST, BOOL bEmptyChild = FALSE);
    HTREEITEM InsertItem(LPCTSTR lpszItem, int nImage, int nSelectedImage, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);
    HTREEITEM InsertItem(PCUSTOMINSERTSTRUCT lpInsertStruct );

    BOOL DeleteAllItems();
    void AddEmptyChild(HTREEITEM hItem);


    HTREEITEM AddFolder( CString& lpszItem, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);

		//Copy item to another parent
		void CopyItem(	HTREEITEM hSourceItem, HTREEITEM hTargetItem, bool bMoveSibling = false	);

		//To a delete an item recurisvely
		void DeleteItem( HTREEITEM hTreeItem );	//[in]	-	Item to be deleted
    void DeleteChildItem( HTREEITEM hItem );

		//To invert the rectangle being drawn
		void	InvertRectangle( CDC* pDC, CPoint startPt, CPoint endPt );	

		//To find the items inside the given rectangle
		bool	ItemsInRect(CRect rect);	//[in]	-	Bounding rectangle

		//To get the list of selected items
		void	CollectSelectedItems();

		//To get the first selected item
		HTREEITEM GetFirstSelectedItem() const ;

		//To get the next selected item
		HTREEITEM GetNextSelectedItem(HTREEITEM hItem) const ;

		//To generate the imagelist for the dragging						 
		CImageList* GetImageListForDrag(std::vector<HTREEITEM> &cArrDragItems);								 
		
		//To prepare the bitmap along with the check box
		int	PrepareImageWithPrefixBmp(  UINT iBmpID1,	CBitmap& bmp,	int& newBmpW,	int& newBmpH,	UINT iBmpID2 = -1,  UINT iBmpID3 = -1,  UINT iBmpID4 = -1 );		

		int	AddImageToList( PCUSTOMINSERTSTRUCT lpInsertStruct );

		//To add the images for normal and expanded state [ WITHOUT CHECKBOX ]
		int	AddNoramlStateImages( PCUSTOMINSERTSTRUCT lpInsertStruct );
		
		//To add the images for the check,uncheck images for normal state [WITH CHECKBOX]
		int	AddCheckStateImages(UINT uCheckBmpID,UINT uUnCheckBmpID, PCUSTOMITEMDATA pCustData	);

		//To find the index of the bitmap image
		int		GetImageIndex( UINT nBmpID );		//[in]	-	ID of the bitmap

		//To scale a giveb bitmap
		void	ScaleBitmap( CBitmap& bmp, int Width,	int	Height,	int newWidth,	int newHeight,	CBitmap& resBmp );	

		//To combine to given bitmaps
		bool AddBitamps4Chk( CBitmap*  pBmp1, CBitmap*  pBmp2, CBitmap*  pBmp3, CBitmap*  pBmp4, CBitmap* resBmp, int W = -1, int H = -1);	
    bool AddBitamps3Chk( CBitmap*  pBmp1, CBitmap*  pBmp2, CBitmap*  pBmp3, CBitmap* resBmp, int W = -1, int H = -1);			
    bool AddBitamps2Chk( CBitmap*  pBmp1, CBitmap*  pBmp2, CBitmap* resBmp, int W = -1, int H = -1);	
    bool AddBitamps1Chk( CBitmap*  pBmp1, CBitmap* resBmp,	 int W = -1,	int H = -1);
    bool AddBitampsNoneChk(int W = -1,	int H = -1);

		int	GetBitmapIndexFromImgList( UINT uBitmapID );	  //To get the index of the bitmap from the image list

		//To add a given image and its mask to the list
		int  AddImage4Chk(  UINT uBmpID1,	UINT uBmpID2,	UINT uBmpID3, UINT uBmpID4 );	
    int  AddImage3Chk(  UINT uBmpID1,	UINT uBmpID2,	UINT uBmpID3 );
    int  AddImage2Chk(  UINT uBmpID1,	UINT uBmpID2 );
    int  AddImage1Chk(  UINT uBmpID1	);

		//To set the Move Item flag of drag 
		void SetMoveDraggedItem( bool flag );	//[in]	-	True to move the item,false to copy the item

		//To set the color in the bitmap
		void SetBitMapColor( CBitmap& bmp,COLORREF color,	COLORREF excludeColor);

		//To generate the disabled image for the item
		void	GenerateDisableImage( CBitmap& bmp );	//[out]	-	Resuting disabled image

		//To get the bitmap of the image from the image list
		bool	GetBitampFromImageList( int iIndex,	CBitmap& bmp );			

		//To convert the icon into bitmap
		bool	 GetBitmapFromIcon( HICON hIcon, int w,	int h ,CBitmap& bmp);									
		
		//To set the image index based on the state of the item
		void	SetImage( HTREEITEM hItem );	//[in]	-	Item to which the image to be set
    void	SetCheckboxesState( HTREEITEM hItem );

    void  SortTreeItem(HTREEITEM hItem);

		//To check whether normal checked index is avialbale or not
		int		IsNormalCheckedExists( UINT uBmpID );

    bool GetCheckOne(HTREEITEM hClickedItem);
    bool GetCheckTwo(HTREEITEM hClickedItem);
    bool GetCheckThree(HTREEITEM hClickedItem);
    bool GetCheckFour(HTREEITEM hClickedItem);

    void SetCheckOne(HTREEITEM hClickedItem, bool bChecked);
    void SetCheckTwo(HTREEITEM hClickedItem, bool bChecked);
    void SetCheckThree(HTREEITEM hClickedItem, bool bChecked);
    void SetCheckFour(HTREEITEM hClickedItem, bool bChecked);

    void ClearAllSetCheckboxes();
    void SelectFocusItem(HTREEITEM hClickedItem);

    int	DrawItem( LOOPINFO* pLoopInfo );
    int FindItemsInRect( LOOPINFO* pLoopInfo );
    int SelectItems( LOOPINFO* pLoopInfo );
    int ExpandItem( LOOPINFO* pLoopInfo );

private:
  bool m_bDescriptionTS;

  void ClearSelection();
  void AddSelected(HTREEITEM hItem);
	// Generated message map functions
protected:
  BOOL m_bMouseOver;
  HTREEITEM m_hItemPrev;
  CCmnToolTip m_ToolTip;

  struct structData
  {
    HTREEITEM hItem;
    LONGLONG llData;
    BOOL bEmptyChild;
    structData()
    : bEmptyChild(FALSE)
    , llData(-1)
    , hItem(NULL)
    {}
  };
  CArray<structData, structData> m_arrStructData; 

  void ShowToolTip(CString szToolTip);

  //{{AFX_MSG(CCheckboxesTreeCtrl)
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnClickLButton(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg LRESULT SetChkBoxOne( WPARAM wp, LPARAM lp);
  afx_msg LRESULT SetChkBoxTwo( WPARAM wp, LPARAM lp);
  afx_msg LRESULT SetChkBoxThree( WPARAM wp, LPARAM lp);
  afx_msg LRESULT SetChkBoxFour( WPARAM wp, LPARAM lp);
  afx_msg void OnPaint();			
  afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);

  LRESULT OnMouseHover (WPARAM wParam, LPARAM lParam);
  LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKBOXESTREECTRL_H__234C8D10_F2CD_42B3_9972_384F6609F5D8__INCLUDED_)
