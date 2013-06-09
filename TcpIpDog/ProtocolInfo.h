//=============================================================================================
/*
	ProtocolInfo.h

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

class CProtocolInfo
{
private:
	static int	GetFromSend		(SESSION *session, TCHAR *pBuf, int nBufLenth);
	static int	GetFromRecv		(SESSION *session, TCHAR *pBuf, int nBufLenth);
	static int	GetFtp			(SESSION *session, TCHAR *pBuf, int nBufLenth);
	static int	GetHttp			(SESSION *session, TCHAR *pBuf, int nBufLenth);
	static int	GetSmtp			(SESSION *session, TCHAR *pBuf, int nBufLenth);
	static int	GetPop3BySend	(SESSION *session, TCHAR *pBuf, int nBufLenth);
	static int	GetPop3			(SESSION *session, TCHAR *pBuf, int nBufLenth);

public:
	static int	GetProtocolInfo	(SESSION *session, TCHAR *pBuf, int nBufLenth, BOOL IsSend);
};