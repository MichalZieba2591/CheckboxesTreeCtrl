#ifdef INCLUDE_VERID
static char verid[]="@(#)$RCSfile: TreeItem.cpp,v $$Revision: 1.13 $$Date: 2010/11/25 08:50:07Z $";
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

// treeItem.cpp: implementation of the CTreeItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "treeItem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTreeItem::CTreeItem()
{
  Init();
}

void CTreeItem::Init()
{
  m_Item        = NULL;
  m_ID          = -1;
  m_Sub         = -1;
  m_lParentId   = -1;
  m_llCode      = -1;
  m_szName      = _T("");
  m_szTooltip = _T("");
  m_bArchive    = 0;
  m_strActive   = _T("");
  m_bHasSteps   = false;
  m_nCurrVer    = -1;
  m_nSiteID     = -1;
  m_Type        = UNKNOWN;
  m_cstrExInfo  = _T("");
  m_llExInfo    = -1;
  m_TCType      = _T("");
  m_bIsLocked   = false;
	m_strVisible	 = _T("");;
	m_strResponsible = _T("");
	m_strInProcess = _T("");;
	m_bWasExpanded = false;
  m_strAutoSave  = _T("");
  m_lProductId   = -1;
}

CTreeItem::CTreeItem(LONGLONG id, CString name, LONGLONG llProduct/* = -1*/)
{
  Init();

  m_ID          = id;
  m_szTooltip = name;
  m_lProductId  = llProduct;
}

CTreeItem::CTreeItem(const CTreeItem& treeItem)
{
	m_nCurrVer    = treeItem.m_nCurrVer;
	m_lParentId   = treeItem.m_lParentId;
	m_ID          = treeItem.m_ID;
  m_nSiteID     = treeItem.m_nSiteID;
	m_Item        = treeItem.m_Item;
  m_llCode      = treeItem.m_llCode;
  m_szName      = treeItem.m_szName;
	m_szTooltip = treeItem.m_szTooltip;
	m_Sub         = treeItem.m_Sub;
  m_bHasSteps   = treeItem.m_bHasSteps;
  m_bArchive    = treeItem.m_bArchive;
  m_strActive   = treeItem.m_strActive;
  m_Type        = treeItem.m_Type;
  m_cstrExInfo  = treeItem.m_cstrExInfo;
  m_llExInfo    = treeItem.m_llExInfo;
  m_TCType      = treeItem.m_TCType;
  m_bIsLocked   = treeItem.m_bIsLocked;
	m_strVisible		= treeItem.m_strVisible;
	m_strResponsible = treeItem.m_strResponsible;
	m_strInProcess	= treeItem.m_strInProcess;
	m_bWasExpanded = treeItem.m_bWasExpanded;
  m_strAutoSave  = treeItem.m_strAutoSave;
  m_lProductId   = treeItem.m_lProductId;
}

CTreeItem::~CTreeItem()
{
}

CTreeItem& CTreeItem::operator=(const CTreeItem& treeItem)
{
	m_nCurrVer    = treeItem.m_nCurrVer;
	m_lParentId   = treeItem.m_lParentId;
	m_ID          = treeItem.m_ID;
  m_nSiteID     = treeItem.m_nSiteID;
	m_Item        = treeItem.m_Item;
  m_llCode      = treeItem.m_llCode;
  m_szName      = treeItem.m_szName;
	m_szTooltip   = treeItem.m_szTooltip;
	m_Sub         = treeItem.m_Sub;
  m_bHasSteps   = treeItem.m_bHasSteps;
  m_bArchive    = treeItem.m_bArchive;
  m_strActive   = treeItem.m_strActive;
  m_Type        = treeItem.m_Type;
  m_cstrExInfo  = treeItem.m_cstrExInfo;
  m_llExInfo    = treeItem.m_llExInfo;
  m_TCType      = treeItem.m_TCType;
  m_bIsLocked   = treeItem.m_bIsLocked;
	m_strVisible		= treeItem.m_strVisible;
	m_strResponsible = treeItem.m_strResponsible;
	m_strInProcess	= treeItem.m_strInProcess;
	m_bWasExpanded = treeItem.m_bWasExpanded;
  m_strAutoSave  = treeItem.m_strAutoSave;
  m_lProductId   = treeItem.m_lProductId;

  return *this;
}
//const CString CTreeItem::sItemType[] = { "UNKNOWN", "TEST", "CASE", "ROOT", "DEL", "ARCHIVE"};


