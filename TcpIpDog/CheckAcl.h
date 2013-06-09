//=============================================================================================
/*
	CheckAcl.h

	Project	: XFILTER 1.0 Personal Firewall
	Author	: Tony Zhu
	Create Date	: 2001/08/21
	Email	: xstudio@xfilt.com
	URL		: http://www.xfilt.com

	Copyright (c) 2001-2002 XStudio Technology.
	All Rights Reserved.

	WARNNING: 
*/
//=============================================================================================

class CCheckAcl
{
public:
	CCheckAcl();
	virtual	~CCheckAcl();

private:
	static	BOOL IsLocalIP	(DWORD *ip);
	int		PreGetAccessInfo(SOCKET s);
	int		GetAccessInfo	(SESSION *session);
	int		GetAccessFromWorkMode();
	int		GetAccessFromAcl(SESSION *mSession);

	DWORD	FindAcl			(CString sApplication, DWORD iStart);
	int		FindTime		(CTime time);
	int		FindIP			(DWORD IP);
	int		FinallySession	();

	BOOL	InitializeSession(SESSION* session);
	int		CreateSession	(SOCKET s, int nProtocol);
	int		DeleteSession	(SOCKET s);
	int		FindSession		(SOCKET s);
	int		SetSession		(SESSION *session, BYTE bDirection, UINT uiPort, DWORD ulRemoteIP);
	int		SetSessionEx	(SESSION *session, BYTE bDirection, const TCHAR *pMemo, int ByteCount, BOOL isSend);

public:
	BOOL	SetWindowsVersion();
	int		CheckStartup	();
	void	CheckSocket		(SOCKET s, int af, int type, int protocol);
	void	CheckCloseSocket(SOCKET s);
	int		CheckConnect	(SOCKET s, const struct sockaddr FAR *name, int namelen);
	int		CheckAccept		(SOCKET s, SOCKET news);
	int		CheckSend		(SOCKET s, TCHAR *buf, int len, LPDWORD lpNumberOfBytesSent);
	int		CheckSendTo		(SOCKET s, const SOCKADDR *pTo, TCHAR *buf, int len, LPDWORD lpNumberOfBytesSent);
	int		CheckRecv		(SOCKET s, TCHAR *buf, int len, LPDWORD lpNumberOfBytesRecvd);
	int		CheckRecvFrom	(SOCKET s, SOCKADDR *pFrom, TCHAR *buf, int len, LPDWORD lpNumberOfBytesRecvd);

public:
	static int	SetWorkMode			(int iWorkMode);
	static int	SetAcl				(XACL_FILE AclFile);
	static BOOL	SetAclToChangedMode	(BOOL IsChange);
	static int	SendSessionToApp	(SESSION *session);

public:
	static int			m_iWorkMode;
	static BOOL			m_bAclIsChange;
	static XACL_FILE	m_AclFile;
	static HWND			m_GuiHwnd;
	static SESSION		m_SessionBuf[MAX_SESSION_BUFFER];
	static TCHAR		m_sGuiPathName[MAX_PATH];
	static BOOL			m_bIsWin9x;
	static TCHAR		m_sSystemPath[MAX_PATH];
	static TCHAR		m_sWin9xSys1[MAX_PATH];
	static TCHAR		m_sWin2kSys1[MAX_PATH];

public:
	SESSION			*m_Session;
	int				m_SessionCount;
};