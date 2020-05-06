// CheckboxesTreeDlg.cpp : implementation file
// 

#include "stdafx.h"
#include <Windows.h>
#include "CheckboxesTree.h"
#include "CheckboxesTreeDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckboxesTreeDlg dialog

CCheckboxesTreeDlg::CCheckboxesTreeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCheckboxesTreeDlg::IDD, pParent)
{
  m_llRootID = 99999999999;
  m_szSearch = "";
  m_TreeArrayBuffor.RemoveAll();
  m_TreeArraySearch.RemoveAll();
  m_TreeArrayBackUp.RemoveAll();
}

void CCheckboxesTreeDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CCheckboxesTreeDlg)
  DDX_Control(pDX, IDC_TREE, m_tree);
  DDX_Control(pDX, IDC_EDIT_SEARCH, m_editSearch);
  DDX_Control(pDX, IDC_RADIO_NONE, m_radioNone);
  DDX_Control(pDX, IDC_RADIO_ONE, m_radioOne);
  DDX_Control(pDX, IDC_RADIO_TWO, m_radioTwo);
  DDX_Control(pDX, IDC_RADIO_THREE, m_radioThree);
  DDX_Control(pDX, IDC_RADIO_FOUR, m_radioFour);
  DDX_Control(pDX, IDC_CHECK_SRCH_NAME, m_checkSearchName);
  DDX_Control(pDX, IDC_CHECK_SRCH_TOOLTIP, m_checkSearchTooltip);
  DDX_Control(pDX, IDC_LIST_ONE, m_listOne);
  DDX_Control(pDX, IDC_LIST_TWO, m_listTwo);
  DDX_Control(pDX, IDC_LIST_THREE, m_listThree);
  DDX_Control(pDX, IDC_LIST_FOUR, m_listFour);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCheckboxesTreeDlg, CDialog)
  //{{AFX_MSG_MAP(CCheckboxesTreeDlg)
  ON_BN_CLICKED(IDC_RADIO_NONE, OnBnClickedRadioNone)
  ON_BN_CLICKED(IDC_RADIO_ONE, OnBnClickedRadioOne)
  ON_BN_CLICKED(IDC_RADIO_TWO, OnBnClickedRadioTwo)
  ON_BN_CLICKED(IDC_RADIO_THREE, OnBnClickedRadioThree)
  ON_BN_CLICKED(IDC_RADIO_FOUR, OnBnClickedRadioFour)
  ON_BN_CLICKED(IDC_CHECK_SRCH_NAME, OnCheckClickedSrchCriteria)
  ON_BN_CLICKED(IDC_CHECK_SRCH_TOOLTIP, OnCheckClickedSrchCriteria)
  ON_NOTIFY(TVN_ITEM_CHECK_ONE, IDC_TREE, OnCheckboxOneClicked)
  ON_NOTIFY(TVN_ITEM_CHECK_TWO, IDC_TREE, OnCheckboxTwoClicked)
  ON_NOTIFY(TVN_ITEM_CHECK_THREE, IDC_TREE, OnCheckboxThreeClicked)
  ON_NOTIFY(TVN_ITEM_CHECK_FOUR, IDC_TREE, OnCheckboxFourClicked)
  //TVN_ITEM_CHECK_ONE
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckboxesTreeDlg message handlers

BOOL CCheckboxesTreeDlg::OnInitDialog()
{
  CDialog::OnInitDialog();
  m_radioFour.SetCheck(BST_CHECKED);
  m_checkSearchName.SetCheck(BST_CHECKED);
 
  ReInitializeTree();
  FillTree();
  OnCheckClickedSrchCriteria();

  return TRUE;  // return TRUE  unless you set the focus to a control
}


void CCheckboxesTreeDlg::OnBnClickedCancel()
{
  CDialog::OnCancel();
}

void CCheckboxesTreeDlg::ReInitializeTree()
{
  int nNumOfCheck = 0;
  if (BST_CHECKED == m_radioNone.GetCheck())
    nNumOfCheck = 0;
  else if (BST_CHECKED == m_radioOne.GetCheck())
    nNumOfCheck = 1;
  else if (BST_CHECKED == m_radioTwo.GetCheck())
    nNumOfCheck = 2;
  else if (BST_CHECKED == m_radioThree.GetCheck())
    nNumOfCheck = 3;
  else if (BST_CHECKED == m_radioFour.GetCheck())
    nNumOfCheck = 4;
  /*
  if(m_tree)
  {
    CRect rect;
    m_tree.GetClientRect(rect);
    m_tree.DestroyWindow();
    m_tree.Create(TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_DISABLEDRAGDROP | WS_BORDER | WS_HSCROLL | WS_TABSTOP, rect, this, IDC_TREE);
  }
  */
  if( m_tree.m_pImgList )
  {
	  m_tree.m_pImgList->DeleteImageList();
    m_tree.m_pImgList = NULL;
	  delete m_tree.m_pImgList;
  }
  m_tree.m_mapBitmapID.clear();
  
  m_tree.EnableTooltipEx();
  m_tree.ExpandTreeRoot(false);
  m_tree.SetNumberOfCheckboxes(nNumOfCheck);
  m_tree.m_bContextMenu = TRUE;
  m_tree.DeleteAllItems();

  OnSearch();
}

void CCheckboxesTreeDlg::FillTree()
{
  UpdateData();
  m_TreeArrayBackUp.RemoveAll();

  //Item 1
  CTreeItem item;
  item.m_ID = 1;
  item.m_llCode = 1;
  item.m_szName = "Item 1";
  item.m_szTooltip = "Tooltip 1";
  item.m_strActive = "Y";
  item.m_lParentId = m_llRootID;
  HTREEITEM hParentItem1 = m_tree.InsertItem(item.m_szName, item);
  item.m_Item = hParentItem1;
  m_TreeArrayBackUp.Add(item);
  m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
  //m_tree.SetItemData(hParentItem1, item.m_ID);

    item.m_ID = 11;
    item.m_llCode = 11;
    item.m_szName = "Item 11";
    item.m_szTooltip = "Tooltip 11";
    item.m_strActive = "Y";
    item.m_lParentId = 1;
    HTREEITEM hParentItem11 = m_tree.InsertItem(item.m_szName, item, hParentItem1);
    item.m_Item = hParentItem11;
    m_TreeArrayBackUp.Add(item);
    m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
    //m_tree.SetItemData(hParentItem11, item.m_ID);

      item.m_ID = 111;
      item.m_llCode = 111;
      item.m_szName = "Item 111";
      item.m_szTooltip = "Tooltip 111";
      item.m_strActive = "Y";
      item.m_lParentId = 11;
      item.m_Item = m_tree.InsertItem(item.m_szName, item, hParentItem11);
      m_TreeArrayBackUp.Add(item);
      m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
      //m_tree.SetItemData(m_tree.InsertItem("Item 111", item, hParentItem11), item.m_ID);

    item.m_ID = 12;
    item.m_llCode = 12;
    item.m_szName = "Item 12";
    item.m_szTooltip = "Tooltip 12";
    item.m_strActive = "Y";
    item.m_lParentId = 1;
    item.m_Item = m_tree.InsertItem(item.m_szName, item, hParentItem1);
    m_TreeArrayBackUp.Add(item);
    m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
    //m_tree.SetItemData(m_tree.InsertItem("Item 12", item, hParentItem1), item.m_ID);

    item.m_ID = 13;
    item.m_llCode = 13;
    item.m_szName = "Item 13";
    item.m_szTooltip = "Tooltip 13";
    item.m_strActive = "Y";
    item.m_lParentId = 1;
    item.m_Item = m_tree.InsertItem(item.m_szName, item, hParentItem1);
    m_TreeArrayBackUp.Add(item);   
    m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
    //m_tree.SetItemData(m_tree.InsertItem("Item 13", item, hParentItem1), item.m_ID);

    item.m_ID = 14;
    item.m_llCode = 14;
    item.m_szName = "Item 14";
    item.m_szTooltip = "Tooltip 14";
    item.m_strActive = "Y";
    item.m_lParentId = 1;
    item.m_Item = m_tree.InsertItem(item.m_szName, item, hParentItem1);
    m_TreeArrayBackUp.Add(item);
    m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
    //m_tree.SetItemData(m_tree.InsertItem("Item 14", item, hParentItem1), item.m_ID);

  //Item 2
  item.m_ID = 2;
  item.m_llCode = 2;
  item.m_szName = "Item 2";
  item.m_szTooltip = "Tooltip 2";
  item.m_strActive = "Y";
  item.m_lParentId = m_llRootID;
  HTREEITEM hParentItem2 = m_tree.InsertItem(item.m_szName, item);
  item.m_Item = hParentItem2;
  m_TreeArrayBackUp.Add(item);
  m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
  //m_tree.SetItemData(hParentItem2, item.m_ID);

    item.m_ID = 21;
    item.m_llCode = 21;
    item.m_szName = "Item 21";
    item.m_szTooltip = "Tooltip 21";
    item.m_strActive = "Y";
    item.m_lParentId = 2;
    HTREEITEM hParentItem21 = m_tree.InsertItem(item.m_szName, item, hParentItem2);
    item.m_Item = hParentItem21;
    m_TreeArrayBackUp.Add(item);
    m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
    //m_tree.SetItemData(hParentItem21, item.m_ID);

      item.m_ID = 211;
      item.m_llCode = 211;
      item.m_szName = "Item 211";
      item.m_szTooltip = "Tooltip 211";
      item.m_strActive = "Y";
      item.m_lParentId = 21;
      item.m_Item = m_tree.InsertItem(item.m_szName, item, hParentItem21);
      m_TreeArrayBackUp.Add(item);
      m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
      //m_tree.SetItemData(m_tree.InsertItem("Item 211", item, hParentItem21), item.m_ID);

    item.m_ID = 22;
    item.m_llCode = 22;
    item.m_szName = "Item 22";
    item.m_szTooltip = "Tooltip 22";
    item.m_strActive = "Y";
    item.m_lParentId = 2;
    item.m_Item = m_tree.InsertItem(item.m_szName, item, hParentItem2);
    m_TreeArrayBackUp.Add(item);
    m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
    //m_tree.SetItemData(m_tree.InsertItem("Item 22", item, hParentItem2), item.m_ID);

    item.m_ID = 23;
    item.m_llCode = 23;
    item.m_szName = "Item 23";
    item.m_szTooltip = "Tooltip 23";
    item.m_strActive = "Y";
    item.m_lParentId = 2;
    item.m_Item = m_tree.InsertItem(item.m_szName, item, hParentItem2);
    m_TreeArrayBackUp.Add(item);
    m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
    //m_tree.SetItemData(m_tree.InsertItem("Item 23", item, hParentItem2), item.m_ID);

    item.m_ID = 24;
    item.m_llCode = 24;
    item.m_szName = "Item 24";
    item.m_szTooltip = "Tooltip 24";
    item.m_strActive = "Y";
    item.m_lParentId = 2;
    item.m_Item = m_tree.InsertItem(item.m_szName, item, hParentItem2);
    m_TreeArrayBackUp.Add(item);
    m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
    //m_tree.SetItemData(m_tree.InsertItem("Item 24", item, hParentItem2), item.m_ID);

  //Item 3
  item.m_ID = 3;
  item.m_llCode = 3;
  item.m_szName = "Item 3";
  item.m_szTooltip = "Tooltip 3";
  item.m_strActive = "Y";
  item.m_lParentId = m_llRootID;
  HTREEITEM hParentItem3 = m_tree.InsertItem(item.m_szName, item);
  item.m_Item = hParentItem3;
  m_TreeArrayBackUp.Add(item);
  m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
  //m_tree.SetItemData(hParentItem3, item.m_ID);

    item.m_ID = 31;
    item.m_llCode = 31;
    item.m_szName = "Item 31";
    item.m_szTooltip = "Tooltip 31";
    item.m_strActive = "Y";
    item.m_lParentId = 3;
    HTREEITEM hParentItem31 = m_tree.InsertItem(item.m_szName, item, hParentItem3);
    item.m_Item = hParentItem31;
    m_TreeArrayBackUp.Add(item);
    m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
    //m_tree.SetItemData(hParentItem31, item.m_ID);

      item.m_ID = 311;
      item.m_llCode = 311;
      item.m_szName = "Item 311";
      item.m_szTooltip = "Tooltip 311";
      item.m_strActive = "Y";
      item.m_lParentId = 31;
      item.m_Item = m_tree.InsertItem(item.m_szName, item, hParentItem31);
      m_TreeArrayBackUp.Add(item);
      m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
      //m_tree.SetItemData(m_tree.InsertItem("Item 311", item, hParentItem31), item.m_ID);

    item.m_ID = 32;
    item.m_llCode = 32;
    item.m_szName = "Item 32";
    item.m_szTooltip = "Tooltip 32";
    item.m_strActive = "Y";
    item.m_lParentId = 3;
    item.m_Item = m_tree.InsertItem(item.m_szName, item, hParentItem3);
    m_TreeArrayBackUp.Add(item);
    m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
    //m_tree.SetItemData(m_tree.InsertItem("Item 32", item, hParentItem3), item.m_ID);

    item.m_ID = 33;
    item.m_llCode = 33;
    item.m_szName = "Item 33";
    item.m_szTooltip = "Tooltip 33";
    item.m_strActive = "Y";
    item.m_lParentId = 3;
    item.m_Item = m_tree.InsertItem(item.m_szName, item, hParentItem3);
    m_TreeArrayBackUp.Add(item);
    m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
    //m_tree.SetItemData(m_tree.InsertItem("Item 33", item, hParentItem3), item.m_ID);

    item.m_ID = 34;
    item.m_llCode = 34;
    item.m_szName = "Item 34";
    item.m_szTooltip = "Tooltip 34";
    item.m_strActive = "Y";
    item.m_lParentId = 3;
    item.m_Item = m_tree.InsertItem(item.m_szName, item, hParentItem3);
    m_TreeArrayBackUp.Add(item);
    m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
    //m_tree.SetItemData(m_tree.InsertItem("Item 34", item, hParentItem3), item.m_ID);

  //Item 4
  item.m_ID = 4;
  item.m_llCode = 4;
  item.m_szName = "Item 4";
  item.m_szTooltip = "Tooltip 4";
  item.m_strActive = "Y";
  item.m_lParentId = m_llRootID;
  HTREEITEM hParentItem4 = m_tree.InsertItem(item.m_szName, item);
  item.m_Item = hParentItem4;
  m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
  m_TreeArrayBackUp.Add(item);
  //m_tree.SetItemData(hParentItem4, item.m_ID);

    item.m_ID = 41;
    item.m_llCode = 41;
    item.m_szName = "Item 41";
    item.m_szTooltip = "Tooltip 41";
    item.m_strActive = "Y";
    item.m_lParentId = 4;
    HTREEITEM hParentItem41 = m_tree.InsertItem(item.m_szName, item, hParentItem4);
    item.m_Item = hParentItem41;
    m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
    m_TreeArrayBackUp.Add(item);
    //m_tree.SetItemData(hParentItem41, item.m_ID);

      item.m_ID = 411;
      item.m_llCode = 411;
      item.m_szName = "Item 411";
      item.m_szTooltip = "Tooltip 411";
      item.m_strActive = "Y";
      item.m_lParentId = 41;
      item.m_Item = m_tree.InsertItem(item.m_szName, item, hParentItem41);
      m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
      m_TreeArrayBackUp.Add(item);
      //m_tree.SetItemData(m_tree.InsertItem("Item 411", item, hParentItem41), item.m_ID);

    item.m_ID = 42;
    item.m_llCode = 42;
    item.m_szName = "Item 42";
    item.m_szTooltip = "Tooltip 42";
    item.m_strActive = "Y";
    item.m_lParentId = 4;
    item.m_Item = m_tree.InsertItem(item.m_szName, item, hParentItem4);
    m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
    m_TreeArrayBackUp.Add(item);
    //m_tree.SetItemData(m_tree.InsertItem("Item 42", item, hParentItem4), item.m_ID);

    item.m_ID = 43;
    item.m_llCode = 43;
    item.m_szName = "Item 43";
    item.m_szTooltip = "Tooltip 43";
    item.m_strActive = "Y";
    item.m_lParentId = 4;
    item.m_Item = m_tree.InsertItem(item.m_szName, item, hParentItem4);
    m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
    m_TreeArrayBackUp.Add(item);
    //m_tree.SetItemData(m_tree.InsertItem("Item 43", item, hParentItem4), item.m_ID);

    item.m_ID = 44;
    item.m_llCode = 44;
    item.m_szName = "Item 44";
    item.m_szTooltip = "Tooltip 44";
    item.m_strActive = "Y";
    item.m_lParentId = 4;
    item.m_Item = m_tree.InsertItem(item.m_szName, item, hParentItem4);
    m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
    m_TreeArrayBackUp.Add(item);
    //m_tree.SetItemData(m_tree.InsertItem("Item 44", item, hParentItem4), item.m_ID);

  //Item 5
  item.m_ID = 5;
  item.m_llCode = 5;
  item.m_szName = "Item 5";
  item.m_szTooltip = "Tooltip 5";
  item.m_strActive = "Y";
  item.m_lParentId = m_llRootID;
  HTREEITEM hParentItem5 = m_tree.InsertItem(item.m_szName, item);
  m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
  item.m_Item = hParentItem5;
  m_TreeArrayBackUp.Add(item);
  //m_tree.SetItemData(hParentItem5, item.m_ID);

    item.m_ID = 51;
    item.m_llCode = 51;
    item.m_szName = "Item 51";
    item.m_szTooltip = "Tooltip 51";
    item.m_strActive = "Y";
    item.m_lParentId = 5;
    HTREEITEM hParentItem51 = m_tree.InsertItem(item.m_szName, item, hParentItem5);
    m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
    item.m_Item = hParentItem51;
    m_TreeArrayBackUp.Add(item);
    //m_tree.SetItemData(hParentItem51, item.m_ID);

      item.m_ID = 511;
      item.m_llCode = 511;
      item.m_szName = "Item 511";
      item.m_szTooltip = "Tooltip 511";
      item.m_strActive = "Y";
      item.m_lParentId = 51;
      item.m_Item = m_tree.InsertItem(item.m_szName, item, hParentItem51);
      m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
      m_TreeArrayBackUp.Add(item);
      //m_tree.SetItemData(m_tree.InsertItem("Item 511", item, hParentItem51), item.m_ID);

    item.m_ID = 52;
    item.m_llCode = 52;
    item.m_szName = "Item 52";
    item.m_szTooltip = "Tooltip 52";
    item.m_strActive = "Y";
    item.m_lParentId = 5;
    item.m_Item = m_tree.InsertItem(item.m_szName, item, hParentItem5);
    m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
    m_TreeArrayBackUp.Add(item);
    //m_tree.SetItemData(m_tree.InsertItem("Item 52", item, hParentItem5), item.m_ID);

    item.m_ID = 53;
    item.m_llCode = 53;
    item.m_szName = "Item 53";
    item.m_szTooltip = "Tooltip 53";
    item.m_strActive = "Y";
    item.m_lParentId = 5;
    item.m_Item = m_tree.InsertItem(item.m_szName, item, hParentItem5);
    m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
    m_TreeArrayBackUp.Add(item);
    //m_tree.SetItemData(m_tree.InsertItem("Item 53", item, hParentItem5), item.m_ID);

    item.m_ID = 54;
    item.m_llCode = 54;
    item.m_szName = "Item 54";
    item.m_szTooltip = "Tooltip 54";
    item.m_strActive = "Y";
    item.m_lParentId = 5;
    item.m_Item = m_tree.InsertItem(item.m_szName, item, hParentItem5);
    m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
    m_TreeArrayBackUp.Add(item);
    //m_tree.SetItemData(m_tree.InsertItem("Item 54", item, hParentItem5), item.m_ID);

  //Item 9
  item.m_ID = 9;
  item.m_llCode = 9;
  item.m_szName = "Item 9";
  item.m_szTooltip = "Tooltip 9";
  item.m_strActive = "N";
  item.m_lParentId = m_llRootID;
  HTREEITEM hParentItem9 = m_tree.InsertItem(item.m_szName, item);
  m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
  item.m_Item = hParentItem9;
  m_TreeArrayBackUp.Add(item);
  //m_tree.SetItemData(hParentItem9, item.m_ID);

    item.m_ID = 91;
    item.m_llCode = 91;
    item.m_szName = "Item 91";
    item.m_szTooltip = "Tooltip 91";
    item.m_strActive = "N";
    item.m_lParentId = 9;
    item.m_Item = m_tree.InsertItem(item.m_szName, item, hParentItem9);
    m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
    m_TreeArrayBackUp.Add(item);
    //m_tree.SetItemData(m_tree.InsertItem("Item 91", item, hParentItem9), item.m_ID);

    item.m_ID = 92;
    item.m_llCode = 92;
    item.m_szName = "Item 92";
    item.m_szTooltip = "Tooltip 92";
    item.m_strActive = "Y";
    item.m_lParentId = 9;
    item.m_Item = m_tree.InsertItem(item.m_szName, item, hParentItem9);
    m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
    m_TreeArrayBackUp.Add(item);
    //m_tree.SetItemData(m_tree.InsertItem("Item 92", item, hParentItem9), item.m_ID);

    item.m_ID = 93;
    item.m_llCode = 93;
    item.m_szName = "Item 93";
    item.m_szTooltip = "Tooltip 93";
    item.m_strActive = "N";
    item.m_lParentId = 9;
    item.m_Item = m_tree.InsertItem(item.m_szName, item, hParentItem9);
    m_tree.SetToolTip(item.m_Item, item.m_szTooltip);
    m_TreeArrayBackUp.Add(item);
    //m_tree.SetItemData(m_tree.InsertItem("Item 93", item, hParentItem9), item.m_ID);


  //Automatic fill structure from control tree
  /*
  HTREEITEM hItem = m_tree.GetRootItem();
  while (NULL != hItem)
  {
    CTreeItem tiItem;
    tiItem.m_Item        = hItem;
    tiItem.m_ID          = m_tree.GetItemData(hItem);
    tiItem.m_szName      = m_tree.GetItemText(hItem);
    tiItem.m_szTooltip   = m_tree.GetToolTipText(hItem);
    HTREEITEM hItemParent = m_tree.GetParentItem(hItem);
    if (NULL == hItemParent)
    {
      tiItem.m_lParentId = m_llRootID;
    }
    else
    {
      tiItem.m_lParentId   = m_tree.GetItemData(m_tree.GetParentItem(hItem));
    }
    m_TreeArrayBackUp.Add(tiItem);
    hItem = m_tree.GetNextItemSmart(hItem);
  }
  */
}

void CCheckboxesTreeDlg::OnBnClickedRadioNone()
{
  //Uncheck rest of radio buttons
  m_radioOne.SetCheck(BST_UNCHECKED);
  m_radioTwo.SetCheck(BST_UNCHECKED);
  m_radioThree.SetCheck(BST_UNCHECKED);
  m_radioFour.SetCheck(BST_UNCHECKED);
  //Refresh tree values
  ReInitializeTree();
  //Show/Hide lists with selected checkboxes
  GetDlgItem(IDC_STATIC_SEL_ONE)->ShowWindow(SW_HIDE);
  m_listOne.ShowWindow(SW_HIDE);
  GetDlgItem(IDC_STATIC_SEL_TWO)->ShowWindow(SW_HIDE);
  m_listTwo.ShowWindow(SW_HIDE);
  GetDlgItem(IDC_STATIC_SEL_THREE)->ShowWindow(SW_HIDE);
  m_listThree.ShowWindow(SW_HIDE);
  GetDlgItem(IDC_STATIC_SEL_FOUR)->ShowWindow(SW_HIDE);
  m_listFour.ShowWindow(SW_HIDE);
}

void CCheckboxesTreeDlg::OnBnClickedRadioOne()
{
  //Uncheck rest of radio buttons
  m_radioNone.SetCheck(BST_UNCHECKED);
  m_radioTwo.SetCheck(BST_UNCHECKED);
  m_radioThree.SetCheck(BST_UNCHECKED);
  m_radioFour.SetCheck(BST_UNCHECKED);
  ReInitializeTree();
  //Show/Hide lists with selected checkboxes
  GetDlgItem(IDC_STATIC_SEL_ONE)->ShowWindow(SW_SHOW);
  m_listOne.ShowWindow(SW_SHOW);
  GetDlgItem(IDC_STATIC_SEL_TWO)->ShowWindow(SW_HIDE);
  m_listTwo.ShowWindow(SW_HIDE);
  GetDlgItem(IDC_STATIC_SEL_THREE)->ShowWindow(SW_HIDE);
  m_listThree.ShowWindow(SW_HIDE);
  GetDlgItem(IDC_STATIC_SEL_FOUR)->ShowWindow(SW_HIDE);
  m_listFour.ShowWindow(SW_HIDE);
}

void CCheckboxesTreeDlg::OnBnClickedRadioTwo()
{
  //Uncheck rest of radio buttons
  m_radioNone.SetCheck(BST_UNCHECKED);
  m_radioOne.SetCheck(BST_UNCHECKED);
  m_radioThree.SetCheck(BST_UNCHECKED);
  m_radioFour.SetCheck(BST_UNCHECKED);
  //Refresh tree values
  ReInitializeTree();
  //Show/Hide lists with selected checkboxes
  GetDlgItem(IDC_STATIC_SEL_ONE)->ShowWindow(SW_SHOW);
  m_listOne.ShowWindow(SW_SHOW);
  GetDlgItem(IDC_STATIC_SEL_TWO)->ShowWindow(SW_SHOW);
  m_listTwo.ShowWindow(SW_SHOW);
  GetDlgItem(IDC_STATIC_SEL_THREE)->ShowWindow(SW_HIDE);
  m_listThree.ShowWindow(SW_HIDE);
  GetDlgItem(IDC_STATIC_SEL_FOUR)->ShowWindow(SW_HIDE);
  m_listFour.ShowWindow(SW_HIDE);
}

void CCheckboxesTreeDlg::OnBnClickedRadioThree()
{
  //Uncheck rest of radio buttons
  m_radioNone.SetCheck(BST_UNCHECKED);
  m_radioOne.SetCheck(BST_UNCHECKED);
  m_radioTwo.SetCheck(BST_UNCHECKED);
  m_radioFour.SetCheck(BST_UNCHECKED);
  //Refresh tree values
  ReInitializeTree();
  //Show/Hide lists with selected checkboxes
  GetDlgItem(IDC_STATIC_SEL_ONE)->ShowWindow(SW_SHOW);
  m_listOne.ShowWindow(SW_SHOW);
  GetDlgItem(IDC_STATIC_SEL_TWO)->ShowWindow(SW_SHOW);
  m_listTwo.ShowWindow(SW_SHOW);
  GetDlgItem(IDC_STATIC_SEL_THREE)->ShowWindow(SW_SHOW);
  m_listThree.ShowWindow(SW_SHOW);
  GetDlgItem(IDC_STATIC_SEL_FOUR)->ShowWindow(SW_HIDE);
  m_listFour.ShowWindow(SW_HIDE);
}


void CCheckboxesTreeDlg::OnBnClickedRadioFour()
{
  //Uncheck rest of radio buttons
  m_radioNone.SetCheck(BST_UNCHECKED);
  m_radioOne.SetCheck(BST_UNCHECKED);
  m_radioTwo.SetCheck(BST_UNCHECKED);
  m_radioThree.SetCheck(BST_UNCHECKED);
  //Refresh tree values
  ReInitializeTree();
  //Show/Hide lists with selected checkboxes
  GetDlgItem(IDC_STATIC_SEL_ONE)->ShowWindow(SW_SHOW);
  m_listOne.ShowWindow(SW_SHOW);
  GetDlgItem(IDC_STATIC_SEL_TWO)->ShowWindow(SW_SHOW);
  m_listTwo.ShowWindow(SW_SHOW);
  GetDlgItem(IDC_STATIC_SEL_THREE)->ShowWindow(SW_SHOW);
  m_listThree.ShowWindow(SW_SHOW);
  GetDlgItem(IDC_STATIC_SEL_FOUR)->ShowWindow(SW_SHOW);
  m_listFour.ShowWindow(SW_SHOW);
}


BOOL CCheckboxesTreeDlg::PreTranslateMessage(MSG* pMsg)
{
  CString szSearch;
  m_editSearch.GetWindowText(szSearch);
  if(m_szSearch != szSearch)
  {
    m_szSearch = szSearch;
    OnSearch();
    return TRUE;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

void CCheckboxesTreeDlg::OnSearch()
{
  CTreeItem tiItem;
  CString szSearchText;
  m_TreeArraySearch.RemoveAll();
  m_tree.DeleteAllItems();

  m_editSearch.GetWindowText(szSearchText);
  szSearchText.MakeLower();

  BOOL bSrchName = m_checkSearchName.GetCheck();
  BOOL bSrchTooltip = m_checkSearchTooltip.GetCheck();

  if ("" != szSearchText)
  {
    for(int counter = 0;counter < m_TreeArrayBackUp.GetSize();counter++)
    {
      CString szName, szTooltip;

      tiItem.m_Item        = m_TreeArrayBackUp[counter].m_Item;
      tiItem.m_ID          = m_TreeArrayBackUp[counter].m_ID;
      tiItem.m_llCode      = m_TreeArrayBackUp[counter].m_llCode;
      tiItem.m_lParentId   = m_TreeArrayBackUp[counter].m_lParentId;
      tiItem.m_szName      = szName = m_TreeArrayBackUp[counter].m_szName;
      tiItem.m_szTooltip   = szTooltip = m_TreeArrayBackUp[counter].m_szTooltip;
      tiItem.m_strActive   = m_TreeArrayBackUp[counter].m_strActive;

      tiItem.m_bIsLocked   = FALSE;     //Item belonging to branch but not exactly search item
      tiItem.m_bWasExpanded   = FALSE;  //Only Search Item

      szName.MakeLower();
      szTooltip.MakeLower();

      if (TRUE == bSrchName && szName.Find(szSearchText) > -1)
      {
        tiItem.m_bIsLocked   = tiItem.m_bWasExpanded   = TRUE;
      }
      else if (TRUE == bSrchTooltip && szTooltip.Find(szSearchText) > -1)
      {
        tiItem.m_bIsLocked   = tiItem.m_bWasExpanded   = TRUE;
      }
      m_TreeArraySearch.Add(tiItem);
    }
    m_tree.SetFocus();
    CompleteTreeArraySearch();
    FillTreeToTreeFromStructSearch();
    m_tree.SortTreeItem(TVI_ROOT);
    m_tree.ExpandTreeRoot();
  }
  else if ("" == szSearchText)
  {
    m_tree.SetFocus();
    FillTreeToTreeFromStructBackUp();
    m_tree.SortTreeItem(TVI_ROOT);
  }
  ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
  m_editSearch.SetFocus();
}

void CCheckboxesTreeDlg::FillTreeToTreeFromStructBackUp()
{
  CString szTmp;
  m_TreeArrayBuffor.RemoveAll();
  m_tree.DeleteAllItems();

  int counter_item = 0;
  while( counter_item != m_TreeArrayBackUp.GetSize() )
  {
    int current_item = -1;
    LONGLONG tmp_root = m_llRootID;

    for(int counter = 0;counter < m_TreeArrayBuffor.GetSize();counter++)
    {
      if(m_TreeArrayBackUp[counter_item].m_lParentId == m_TreeArrayBuffor[counter].m_ID)
      {
        tmp_root = m_TreeArrayBackUp[counter_item].m_lParentId;
        current_item = counter;
        break;
      }
    }

    if(m_TreeArrayBackUp[counter_item].m_lParentId == tmp_root)
    {
      CString description = m_TreeArrayBackUp[counter_item].m_szName;
      LONGLONG llCode = m_TreeArrayBackUp[counter_item].m_llCode;
      CString descriptionEx = m_TreeArrayBackUp[counter_item].m_szTooltip;
      CTreeItem item;
      item.m_ID = m_TreeArrayBackUp[counter_item].m_ID;
      item.m_llCode = llCode;
      item.m_szName = description;
      item.m_szTooltip = descriptionEx;
      item.m_strActive = m_TreeArrayBackUp[counter_item].m_strActive;
      /*CString szCode = ToString<LONGLONG>(llCode);
      for (int i=szCode.GetLength(); 4>i;i++)
      {
        szCode += "x";
      }
      CString szTmp = "";
      if ( 0 < llCode)
      {
        szTmp.Format(_T("%s %s"), szCode, description);
      }
      else
      {*/
        szTmp.Format(_T("%s"), description);
      //}

      if ( current_item > -1 )
      {
        // insert sub-item (child)
        item.m_Item = m_tree.InsertItem(szTmp, item, m_TreeArrayBuffor[current_item].m_Item);
        m_tree.SetToolTip(item.m_Item, descriptionEx);
      }
      else
      {
        // insert root item (parent)
        item.m_Item = m_tree.InsertItem(szTmp, item);
        m_tree.SetToolTip(item.m_Item, descriptionEx);
      }
      m_TreeArrayBuffor.Add(item);
    }
    counter_item++;
  }
}

void CCheckboxesTreeDlg::FillTreeToTreeFromStructSearch()
{
  CString szTmp;
  m_TreeArrayBuffor.RemoveAll();

  int counter_item = 0;
  while( counter_item != m_TreeArraySearch.GetSize() )
  {
    int current_item = -1;
    LONGLONG tmp_root = m_llRootID;
    if ( FALSE == m_TreeArraySearch[counter_item].m_bIsLocked)
      continue;

    for(int counter = 0;counter < m_TreeArrayBuffor.GetSize();counter++)
    {
      if(m_TreeArraySearch[counter_item].m_lParentId == m_TreeArrayBuffor[counter].m_ID)
      {
        tmp_root = m_TreeArraySearch[counter_item].m_lParentId;
        current_item = counter;
        break;
      }
    }

    if(m_TreeArraySearch[counter_item].m_lParentId == tmp_root)
    {
      CString description = m_TreeArraySearch[counter_item].m_szName;
      LONGLONG llCode = m_TreeArraySearch[counter_item].m_llCode;
      CString descriptionEx = m_TreeArraySearch[counter_item].m_szTooltip;
      CTreeItem item;
      item.m_ID = m_TreeArraySearch[counter_item].m_ID;
      item.m_strActive = m_TreeArraySearch[counter_item].m_strActive;
      item.m_llCode = llCode;
      item.m_szName = description;
      item.m_szTooltip = descriptionEx;
      /*
      CString szCode = ToString<LONGLONG>(llCode);
      for (int i=szCode.GetLength(); 4>i;i++)
      {
        szCode += "x";
      }
      CString szTmp = "";
      if ( 0 < llCode)
      {
        szTmp.Format(_T("%s %s"), szCode, description);
      }
      else
      {*/
        szTmp.Format(_T("%s"), description);
      //}

      if( current_item > -1 )
      {
        // insert sub-item (child)
        item.m_Item = m_tree.InsertItem(szTmp, item, m_TreeArrayBuffor[current_item].m_Item);
        m_tree.SetToolTip(item.m_Item, descriptionEx);
      }
      else
      {
        // insert root item (parent)
        item.m_Item = m_tree.InsertItem(szTmp, item);
        m_tree.SetToolTip(item.m_Item, descriptionEx);
      }

      //Bolded appropriate item
      if (TRUE == m_TreeArraySearch[counter_item].m_bWasExpanded)
      {
        m_tree.SetBoldFont(item.m_Item);
      }

      m_TreeArrayBuffor.Add(item);
    }
    counter_item++;
  }
}

void CCheckboxesTreeDlg::CompleteTreeArraySearch()
{
  for(int counter = m_TreeArraySearch.GetSize()-1;counter >= 0;counter--)
  {
    if ( TRUE == m_TreeArraySearch[counter].m_bIsLocked && m_llRootID != m_TreeArraySearch[counter].m_lParentId)
    {
      for(int cnt = 0;cnt < m_TreeArraySearch.GetSize();cnt++)
      {
        if (m_TreeArraySearch[counter].m_lParentId == m_TreeArraySearch[cnt].m_ID)
        {
          m_TreeArraySearch[cnt].m_bIsLocked = TRUE;
        }
      }
    }
    else if (FALSE == m_TreeArraySearch[counter].m_bIsLocked)
    {
      m_TreeArraySearch.RemoveAt(counter);
    }
  }
}

void CCheckboxesTreeDlg::OnCheckClickedSrchCriteria()
{
  if (BST_UNCHECKED == m_checkSearchName.GetCheck() && BST_UNCHECKED == m_checkSearchTooltip.GetCheck())
  {
    m_editSearch.EnableWindow(FALSE);
    m_editSearch.SetWindowText("");
    m_tree.SetFocus();
    FillTreeToTreeFromStructBackUp();
    m_tree.SortTreeItem(TVI_ROOT);
  }
  else
  {
    m_editSearch.EnableWindow(TRUE);
  }
}

void CCheckboxesTreeDlg::OnCheckboxOneClicked(NMHDR* pNotifyStruct, LRESULT* result)
{
  NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNotifyStruct;
	//Get the old selected item and new item
	HTREEITEM hOldSelItem = pNMTreeView->itemOld.hItem;
	HTREEITEM hNewSelItem = pNMTreeView->itemNew.hItem;
  //Get item text
	CString szItemText = m_tree.GetItemText(hNewSelItem);

	if (BST_CHECKED == m_tree.GetCheckOne(hNewSelItem))
	{
    m_listOne.AddString(szItemText);
	}
  else
  {
    m_listOne.DeleteString(GetItemIndexFromList(m_listOne, szItemText));
  }
}

void CCheckboxesTreeDlg::OnCheckboxTwoClicked(NMHDR* pNotifyStruct, LRESULT* result)
{
  NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNotifyStruct;
	//Get the old selected item and new item
	HTREEITEM hOldSelItem = pNMTreeView->itemOld.hItem;
	HTREEITEM hNewSelItem = pNMTreeView->itemNew.hItem;
  //Get item text
	CString szItemText = m_tree.GetItemText(hNewSelItem);

	if (BST_CHECKED == m_tree.GetCheckTwo(hNewSelItem))
	{
    m_listTwo.AddString(szItemText);
	}
  else
  {
    m_listTwo.DeleteString(GetItemIndexFromList(m_listTwo, szItemText));
  }
}

void CCheckboxesTreeDlg::OnCheckboxThreeClicked(NMHDR* pNotifyStruct, LRESULT* result)
{
  NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNotifyStruct;
	//Get the old selected item and new item
	HTREEITEM hOldSelItem = pNMTreeView->itemOld.hItem;
	HTREEITEM hNewSelItem = pNMTreeView->itemNew.hItem;
  //Get item text
	CString szItemText = m_tree.GetItemText(hNewSelItem);

	if (BST_CHECKED == m_tree.GetCheckThree(hNewSelItem))
	{
    m_listThree.AddString(szItemText);
	}
  else
  {
    m_listThree.DeleteString(GetItemIndexFromList(m_listThree, szItemText));
  }
}
void CCheckboxesTreeDlg::OnCheckboxFourClicked(NMHDR* pNotifyStruct, LRESULT* result)
{
  NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNotifyStruct;
	//Get the old selected item and new item
	HTREEITEM hOldSelItem = pNMTreeView->itemOld.hItem;
	HTREEITEM hNewSelItem = pNMTreeView->itemNew.hItem;
  //Get item text
	CString szItemText = m_tree.GetItemText(hNewSelItem);

	if (BST_CHECKED == m_tree.GetCheckFour(hNewSelItem))
	{
    m_listFour.AddString(szItemText);
	}
  else
  {
    m_listFour.DeleteString(GetItemIndexFromList(m_listFour, szItemText));
  }
}

int CCheckboxesTreeDlg::GetItemIndexFromList(const CListBox &list, const CString &strItemText)
{
  CString szTmp;
  for (int i=0; i< list.GetCount(); i++)
  {
    list.GetText(i, szTmp);
    if (0 == strItemText.Compare(szTmp))
    {
      return i;
    }
  }

  return -1;
}