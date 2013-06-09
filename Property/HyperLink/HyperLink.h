//=============================================================================================
/*
	HyperLink.h

	Project	: XFILTER 1.0
	Author	: Tony Zhu
	Create Date	: 2001/08/06
	Email	: xstudio@xfilt.com
	URL		: http://www.xfilt.com

	Copyright (c) 2001-2002 XStudio Technology.
	All Rights Reserved.

	WARNNING: 
*/
//=============================================================================================

#if !defined(HYPERLINK_H_INCLUDED)
#define HYPERLINK_H_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif 

class CHyperLink : public CStatic
{
public:
    CHyperLink();
    virtual ~CHyperLink();

public:
    void SetURL(CString strURL);

    void SetVisited(BOOL bVisited = TRUE);

    HINSTANCE GotoURL(LPCTSTR url, int showcmd);

    //{{AFX_VIRTUAL(CHyperLink)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

protected:
    LONG GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata);
    void PositionWindow();
    void SetDefaultCursor();

protected:
    COLORREF	m_crLinkColour;
	COLORREF	m_crVisitedColour;    
    COLORREF	m_crHoverColour;                      
    BOOL		m_bOverControl;                        
    BOOL		m_bVisited;                            
    BOOL		m_bUnderline;                         
    BOOL		m_bAdjustToFit;                        
    CString		m_strURL;                             
    CFont		m_Font;                                
    HCURSOR		m_hLinkCursor;                        
    CToolTipCtrl m_ToolTip;                        

protected:
    //{{AFX_MSG(CHyperLink)
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    //}}AFX_MSG
    afx_msg void OnClicked();
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(HYPERLINK_H_INCLUDED)


