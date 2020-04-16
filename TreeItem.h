#ifdef INCLUDE_VERID
static char verid[]="@(#)$RCSfile: TreeItem.h,v $$Revision: 1.18 $$Date: 2010/11/25 08:50:09Z $";
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
// TreeItem.h: interface for the CTreeItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TREEITEM_H__0D3190F4_5EF9_11D5_9DD8_009027C53643__INCLUDED_)
#define AFX_TREEITEM_H__0D3190F4_5EF9_11D5_9DD8_009027C53643__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTreeItem  
{
public:
  enum ItemType { UNKNOWN = 0, TEST, CASE, ROOT, DEL, ARCHIVE, COMMON_FILTER, PRIVATE_FILTER };
  static const CString sItemType[];

public:
	HTREEITEM m_Item;
  LONGLONG  m_ID;
	LONGLONG  m_Sub;
  LONGLONG  m_lParentId;
  LONGLONG  m_lProductId;
  LONGLONG  m_llCode;
  CString   m_szName;
	CString   m_szTooltip;
  BOOL      m_bArchive;
  CString   m_strActive;
  bool      m_bHasSteps;
  bool      m_bIsLocked;
  int       m_nCurrVer;
  int       m_nSiteID;
  ItemType  m_Type;
  CString   m_cstrExInfo;
  LONGLONG  m_llExInfo;
  CString   m_TCType;
	CString		m_strVisible;
	CString   m_strResponsible;
	CString		m_strInProcess;
	bool			m_bWasExpanded;
  CString   m_strAutoSave;


  //ctor/dtor
	CTreeItem();
  CTreeItem(LONGLONG id, CString name, LONGLONG llProduct = -1);
  CTreeItem(const CTreeItem& treeItem);
	~CTreeItem();

  CTreeItem& operator=(const CTreeItem& treeitem);

  void Init();
};

class IsIDForTreeItem
{
public:
  
  IsIDForTreeItem (LONGLONG x = -1) : val(x) {}

  bool operator() (CTreeItem& x) const { return ((-1 != val) && (x.m_ID == val)); }
  
private:
  LONGLONG val;
};

class IsItemForTreeItem
{
public:
  
  IsItemForTreeItem (HTREEITEM x = NULL) : val(x) {}

  bool operator() (CTreeItem& x) const { return (val && (x.m_Item == val)); }
  
private:
  HTREEITEM val;
};

class IsParentItem
{
public:
  
  IsParentItem (LONGLONG x = -1) : val(x) {}

  bool operator() (CTreeItem& x) const { return ((-1 != val) && (x.m_lParentId == val)); }
  
private:
  LONGLONG val;
};

class IsParentItemForTreeItem
{
public:
  
  IsParentItemForTreeItem (LONGLONG x = -1) : val(x) {}

  bool operator() (CTreeItem& x) const { return ((-1 != val) && (x.m_lParentId == val) && (NULL != x.m_Item)); }
  
private:
  LONGLONG val;
};

class IsSameParentAndActivity
{
public:
  IsSameParentAndActivity(LONGLONG x = -1, CString a = "Y") : val(x), activity(a) {}
  bool operator() (CTreeItem& x) const { return ((-1 != val) && (x.m_lParentId == val) && (x.m_strActive == activity)); }
  
private:
  LONGLONG val;
  CString  activity;
};

class IsTopRevision
{
public:
  bool operator()(CTreeItem& item1, CTreeItem& item2)
  {
    return (item1.m_nCurrVer < item2.m_nCurrVer);
  }
};

#endif // !defined(AFX_TREEITEM_H__0D3190F4_5EF9_11D5_9DD8_009027C53643__INCLUDED_)
