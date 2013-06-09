//=============================================================================================
/*
	文件：		HyperLink.cpp

	说明:
	---------------------------------------------------
		从 CStatic 继承而来的超级链接类，将静态文本框
	以超级链接形式显示。
	---------------------------------------------------

	工程：		Xfilter 个人防火墙
	作者：		朱雁辉，朱雁冰
	创建日期：	2001/08/06
	网址：		http://www.xfilt.com
	电子邮件：	xstudio@xfilt.com
	版权所有 (c) 2001-2002 X 工作室

	警告:
	---------------------------------------------------
		本电脑程序受著作权法的保护。未经授权，不能使用
	和修改本软件全部或部分源代码。凡擅自复制、盗用或散
	布此程序或部分程序或者有其它任何越权行为，将遭到民
	事赔偿及刑事的处罚，并将依法以最高刑罚进行追诉。
	
		凡通过合法途径购买本软件源代码的用户被默认授权
	可以在自己的程序中使用本软件的部分代码，但作者不对
	代码产生的任何后果负责。
	
		使用了本软件代码的程序只能以可执行文件形式发布，
	未经特别许可，不能将含有本软件源代码的源程序以任何
	形式发布。
	---------------------------------------------------	
*/
//=============================================================================================

#include "stdafx.h"
#include "HyperLink.h"

#include <AFXPRIV.H>
#include "..\guires.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TOOLTIP_ID 1

LPSTR XW2T(LPWSTR lp)	
{										
	USES_CONVERSION;					
	return W2A(lp);						
}
										
CHyperLink::CHyperLink()
{
    m_hLinkCursor		= NULL;                
    m_crLinkColour      = RGB(  0,  0, 238);  
    m_crVisitedColour	= RGB( 85,  26, 139);  
    m_crHoverColour		= ::GetSysColor(COLOR_HIGHLIGHT);
    m_bOverControl      = FALSE;                
    m_bVisited          = FALSE;                
    m_bUnderline        = TRUE;                
    m_bAdjustToFit      = TRUE;                
    m_strURL.Empty();
}

CHyperLink::~CHyperLink()
{
    m_Font.DeleteObject();
}

BEGIN_MESSAGE_MAP(CHyperLink, CStatic)
    //{{AFX_MSG_MAP(CHyperLink)
    ON_CONTROL_REFLECT(STN_CLICKED, OnClicked)
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_SETCURSOR()
    ON_WM_MOUSEMOVE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CHyperLink::PreTranslateMessage(MSG* pMsg) 
{
    m_ToolTip.RelayEvent(pMsg);
    return CStatic::PreTranslateMessage(pMsg);
}

void CHyperLink::OnClicked()
{
    int result = (int)GotoURL(m_strURL, SW_SHOW);
    m_bVisited = (result > HINSTANCE_ERROR);
    if (!m_bVisited) 
		AfxMessageBox(GUI_MESSAGE_HYPER_LINK_ERROR, MB_ICONEXCLAMATION | MB_OK);
    else 
        SetVisited();                       
}

HBRUSH CHyperLink::CtlColor(CDC* pDC, UINT nCtlColor) 
{
    ASSERT(nCtlColor == CTLCOLOR_STATIC);

    if (m_bOverControl)
        pDC->SetTextColor(m_crHoverColour);
    else if (m_bVisited)
        pDC->SetTextColor(m_crVisitedColour);
    else
        pDC->SetTextColor(m_crLinkColour);

    pDC->SetBkMode(TRANSPARENT);
    return (HBRUSH)GetStockObject(NULL_BRUSH);
}

void CHyperLink::OnMouseMove(UINT nFlags, CPoint point) 
{
    CStatic::OnMouseMove(nFlags, point);

    if (m_bOverControl)        
    {
        CRect rect;
        GetClientRect(rect);

        if (!rect.PtInRect(point))
        {
            m_bOverControl = FALSE;
            ReleaseCapture();
            RedrawWindow();
            return;
        }
    }
    else                      
    {
        m_bOverControl = TRUE;
        RedrawWindow();
        SetCapture();
    }
}

BOOL CHyperLink::OnSetCursor(CWnd*, UINT, UINT) 
{
    if (m_hLinkCursor)
    {
        ::SetCursor(m_hLinkCursor);
        return TRUE;
    }
    return FALSE;
}

void CHyperLink::PreSubclassWindow() 
{
    DWORD dwStyle = GetStyle();
    ::SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);
    
    if (m_strURL.IsEmpty())
        GetWindowText(m_strURL);

    CString strWndText;
    GetWindowText(strWndText);
    if (strWndText.IsEmpty()) {
        ASSERT(!m_strURL.IsEmpty());    
        SetWindowText(m_strURL);
    }

    LOGFONT lf;
    GetFont()->GetLogFont(&lf);
    lf.lfUnderline = m_bUnderline;
    m_Font.CreateFontIndirect(&lf);
    SetFont(&m_Font);

    PositionWindow();        
    SetDefaultCursor();     

    CRect rect; 
    GetClientRect(rect);
    m_ToolTip.Create(this);
    m_ToolTip.AddTool(this, m_strURL, rect, TOOLTIP_ID);

    CStatic::PreSubclassWindow();
}

void CHyperLink::SetURL(CString strURL)
{
    m_strURL = strURL;

    if (::IsWindow(GetSafeHwnd())) {
        PositionWindow();
        m_ToolTip.UpdateTipText(strURL, this, TOOLTIP_ID);
    }
}

void CHyperLink::SetVisited(BOOL bVisited) 
{ 
    m_bVisited = bVisited; 

    if (::IsWindow(GetSafeHwnd()))
        Invalidate(); 
}

void CHyperLink::PositionWindow()
{
    if (!::IsWindow(GetSafeHwnd()) || !m_bAdjustToFit) 
        return;

    CRect rect;
    GetWindowRect(rect);

    CWnd* pParent = GetParent();
    if (pParent)
        pParent->ScreenToClient(rect);

    CString strWndText;
    GetWindowText(strWndText);

    CDC* pDC = GetDC();
    CFont* pOldFont = pDC->SelectObject(&m_Font);
    CSize Extent = pDC->GetTextExtent(strWndText);
    pDC->SelectObject(pOldFont);
    ReleaseDC(pDC);

    DWORD dwStyle = GetStyle();

    if (dwStyle & SS_CENTERIMAGE)
        rect.DeflateRect(0, (rect.Height() - Extent.cy)/2);
    else
        rect.bottom = rect.top + Extent.cy;

    if (dwStyle & SS_CENTER)  
        rect.DeflateRect((rect.Width() - Extent.cx)/2, 0);
    else if (dwStyle & SS_RIGHT) 
        rect.left  = rect.right - Extent.cx;
    else  
        rect.right = rect.left + Extent.cx;

    SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER);
}

void CHyperLink::SetDefaultCursor()
{
    if (m_hLinkCursor == NULL)                
    {
        CString strWndDir;
        GetWindowsDirectory(strWndDir.GetBuffer(MAX_PATH), MAX_PATH);
        strWndDir.ReleaseBuffer();

        strWndDir += _T("\\winhlp32.exe");
        HMODULE hModule = LoadLibrary(strWndDir);
        if (hModule) {
            HCURSOR hHandCursor = ::LoadCursor(hModule, MAKEINTRESOURCE(106));
            if (hHandCursor)
                m_hLinkCursor = CopyCursor(hHandCursor);
        }
        FreeLibrary(hModule);
    }
}

LONG CHyperLink::GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
    HKEY hkey;
    LONG retval = RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hkey);

    if (retval == ERROR_SUCCESS) {
        long datasize = MAX_PATH;
        TCHAR data[MAX_PATH];
        RegQueryValue(hkey, NULL, data, &datasize);
        lstrcpy(retdata,data);
        RegCloseKey(hkey);
    }

    return retval;
}

HINSTANCE CHyperLink::GotoURL(LPCTSTR url, int showcmd)
{
    TCHAR key[MAX_PATH + MAX_PATH];
	
    HINSTANCE result = ShellExecute(NULL
		, (url[_tcslen(url) -1] == '/') ? _T("explore") : _T("open")
		, url, NULL,NULL, showcmd);

    if ((UINT)result <= HINSTANCE_ERROR) {

        if (GetRegKey(HKEY_CLASSES_ROOT, _T(".htm"), key) == ERROR_SUCCESS) {
            lstrcat(key, _T("\\shell\\open\\command"));

            if (GetRegKey(HKEY_CLASSES_ROOT,key,key) == ERROR_SUCCESS) {
                TCHAR *pos;
                pos = _tcsstr(key, _T("\"%1\""));
                if (pos == NULL) {                    
                    pos = _tcsstr(key, _T("%1"));     
                    if (pos == NULL)                 
                        pos = key+lstrlen(key)-1;
                    else
                        *pos = '\0';                  
                }
                else
                    *pos = '\0';                      

                lstrcat(pos, _T(" "));
                lstrcat(pos, url);
                result = (HINSTANCE) WinExec(key, showcmd);
            }
        }
    }

    return result;
}
