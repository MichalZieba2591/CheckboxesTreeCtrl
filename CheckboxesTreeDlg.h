#if !defined(AFX_CHECKBOXESTREEDLG_H__E4024467_B22B_11D6_ACA4_0080C6EB5BBF__INCLUDED_)
#define AFX_CHECKBOXESTREEDLG_H__E4024467_B22B_11D6_ACA4_0080C6EB5BBF__INCLUDED_

#pragma once
#include "CheckboxesTreeCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CCheckboxesTreeDlg dialog

class CCheckboxesTreeDlg : public CDialog
{
// Construction
public:
	CCheckboxesTreeDlg(CWnd* pParent = NULL);	// standard constructor
  CString m_szSearch;
  LONGLONG m_llRootID;

// Dialog Data
	//{{AFX_DATA(CCheckboxesTreeDlg)
	enum { IDD = IDD_CHECKBOXESTREE_DIALOG };

	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckboxesTreeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  CCheckboxesTreeCtrl m_tree;
  CButton m_radioNone;
  CButton m_radioOne;
  CButton m_radioTwo;
  CButton m_radioThree;
  CButton m_radioFour;
  CEdit m_editSearch;
  CButton m_checkSearchName;
  CButton m_checkSearchTooltip;

	//}}AFX_VIRTUAL

  void ReInitializeTree();
  void FillTree();
  void OnSearch();

// Implementation
protected:
	HICON m_hIcon;

  CArray<CTreeItem,CTreeItem> m_TreeArrayBuffor;
  CArray<CTreeItem,CTreeItem> m_TreeArraySearch;
  CArray<CTreeItem,CTreeItem> m_TreeArrayBackUp;

  void CompleteTreeArraySearch();
  void FillTreeToTreeFromStructSearch();
  void FillTreeToTreeFromStructBackUp();

	// Generated message map functions
	//{{AFX_MSG(CCheckboxesTreeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedCancel();
  afx_msg void OnBnClickedRadioNone();
  afx_msg void OnBnClickedRadioOne();
  afx_msg void OnBnClickedRadioTwo();
  afx_msg void OnBnClickedRadioThree();
  afx_msg void OnBnClickedRadioFour();
  afx_msg void OnCheckClickedSrchCriteria();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKBOXESTREEDLG_H__E4024467_B22B_11D6_ACA4_0080C6EB5BBF__INCLUDED_)
