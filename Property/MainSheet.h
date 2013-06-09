#if !defined(AFX_MAINSHEET_H__EB0E5A52_9A6E_4D40_AC1D_ED13D65C4228__INCLUDED_)
#define AFX_MAINSHEET_H__EB0E5A52_9A6E_4D40_AC1D_ED13D65C4228__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MainSheet.h : header file
//

//=============================================================================================
// CMainSheet

class CMainSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CMainSheet)

// Construction
public:
	CMainSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CMainSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainSheet)
	public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainSheet();

protected:
	//{{AFX_MSG(CMainSheet)
	afx_msg void OnPaint();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
//owner add
public:
	void SetLogoFont(
		IN	CString Name,
		IN	int nHeight		= 24,
		IN	int nWeight		= FW_BOLD,
		IN	BYTE bItalic	= true,
		IN	BYTE bUnderline = false
		);

	CFont		m_FontLogo;
	CString		m_LogoText;
	void		SetLogoText(CString Text);
	void		InitDlgResource();

public:
	BOOL	OnApply();
	void	OnCancel();
};

//=============================================================================================

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINSHEET_H__EB0E5A52_9A6E_4D40_AC1D_ED13D65C4228__INCLUDED_)
