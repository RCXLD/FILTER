#if !defined(AFX_NETTIMESHEET_H__120D81B0_DB29_44AB_AD79_310F5B60C90D__INCLUDED_)
#define AFX_NETTIMESHEET_H__120D81B0_DB29_44AB_AD79_310F5B60C90D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NetTimeSheet.h : header file
//

//=============================================================================================
// CNetTimeSheet

class CNetTimeSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CNetTimeSheet)

// Construction
public:
	CNetTimeSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CNetTimeSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNetTimeSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNetTimeSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNetTimeSheet)
	afx_msg void OnOK();
	afx_msg BOOL OnCancel();
	afx_msg void OnApply();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//owner add 
public:
	void		InitDlgResource	();
	int			EnableButton	(BOOL bEnable);
	int			Apply			();
};

//=============================================================================================

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NETTIMESHEET_H__120D81B0_DB29_44AB_AD79_310F5B60C90D__INCLUDED_)
