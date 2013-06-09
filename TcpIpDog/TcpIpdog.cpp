/*=============================================================================================

	文件：		TcpIpDog.cpp

	说明:
	---------------------------------------------------
		Winsock 钩子，用来截获 Winsock 调用从而拦截
	TCP/IP封包，并做相应处理。
	---------------------------------------------------

	工程：		Xfilter 个人防火墙
	作者：		朱雁辉，朱雁冰
	创建日期：	2001/08/21
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
// include header file and global variables

#include "stdafx.h"
#include "TcpIpDog.h"
#include "CheckAcl.h"

#pragma data_seg(".inidata")
	int				gAclChangeCount = INIT_ACL_CHANGE_COUNT;
	int				m_iDllCount		= 0;
#pragma data_seg()

#pragma bss_seg(".uinidata")
	QUERY_SESSION	m_QuerySession[MAX_QUERY_SESSION];
#pragma bss_seg()

CCheckAcl			m_CheckAcl;

CRITICAL_SECTION	gCriticalSection;
WSPPROC_TABLE		NextProcTable   ;
TCHAR				m_sProcessName[MAX_PATH];

//=============================================================================================
//DllMain Procedure

BOOL WINAPI DllMain(
	HINSTANCE	hModule, 
    DWORD		ul_reason_for_call, 
    LPVOID		lpReserved
)
{
	if(ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
 		GetModuleFileName(NULL, m_sProcessName, MAX_PATH);
		InitializeCriticalSection(&gCriticalSection);

		EnterCriticalSection(&gCriticalSection);
		{
			m_iDllCount ++;

			if(m_iDllCount == 1)
				m_CheckAcl.SetWindowsVersion();
		}
		LeaveCriticalSection(&gCriticalSection);

		ODS2(m_sProcessName,_T(" Loading ..."));
	}
	else if(ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		EnterCriticalSection(&gCriticalSection);
		{
			m_iDllCount -- ;
		}
		LeaveCriticalSection(&gCriticalSection);

		ODS2(m_sProcessName,_T(" Exit ..."));
	}

	return TRUE;
}

//=============================================================================================
//Exported Functions

int WSPAPI WSPStartup(
	WORD				wVersionRequested,
	LPWSPDATA			lpWSPData,
	LPWSAPROTOCOL_INFOW	lpProtocolInfo,
	WSPUPCALLTABLE		upcallTable,
	LPWSPPROC_TABLE		lpProcTable
)
{
	ODS(_T("WSPStartup..."));

	ODS(m_CheckAcl.m_bIsWin9x ? _T("IsWin9x") : _T("IsWinNT or Win2000 ..."));
    
	if(!m_CheckAcl.m_bIsWin9x && m_CheckAcl.CheckStartup() == XF_QUERY && !QueryAccess())
		return SOCKET_ERROR;

	TCHAR				sLibraryPath[512];
    LPWSPSTARTUP        WSPStartupFunc      = NULL;
	HMODULE				hLibraryHandle		= NULL;
    INT                 ErrorCode           = 0; 

	if (!GetHookProvider(lpProtocolInfo, sLibraryPath)
		|| (hLibraryHandle = LoadLibrary(sLibraryPath)) == NULL
		|| (WSPStartupFunc = (LPWSPSTARTUP)GetProcAddress(hLibraryHandle, "WSPStartup")) == NULL
		)
		return WSAEPROVIDERFAILEDINIT;

	if ((ErrorCode = WSPStartupFunc(wVersionRequested, lpWSPData, lpProtocolInfo, upcallTable, lpProcTable)) != ERROR_SUCCESS)
		return ErrorCode;
	
    if( !lpProcTable->lpWSPAccept              ||
		!lpProcTable->lpWSPAddressToString     ||        
		!lpProcTable->lpWSPAsyncSelect         ||   
		!lpProcTable->lpWSPBind                ||   
		!lpProcTable->lpWSPCancelBlockingCall  || 
        !lpProcTable->lpWSPCleanup             ||     
		!lpProcTable->lpWSPCloseSocket         ||    
		!lpProcTable->lpWSPConnect             ||
		!lpProcTable->lpWSPDuplicateSocket     ||
		!lpProcTable->lpWSPEnumNetworkEvents   ||
		!lpProcTable->lpWSPEventSelect         ||
		!lpProcTable->lpWSPGetOverlappedResult ||
		!lpProcTable->lpWSPGetPeerName         ||
		!lpProcTable->lpWSPGetSockName         ||
		!lpProcTable->lpWSPGetSockOpt          ||   
		!lpProcTable->lpWSPGetQOSByName        ||
		!lpProcTable->lpWSPIoctl               ||
		!lpProcTable->lpWSPJoinLeaf            ||  
		!lpProcTable->lpWSPListen              || 
        !lpProcTable->lpWSPRecv                ||     
		!lpProcTable->lpWSPRecvDisconnect      ||     
		!lpProcTable->lpWSPRecvFrom            ||     
		!lpProcTable->lpWSPSelect              ||   
		!lpProcTable->lpWSPSend                ||      
		!lpProcTable->lpWSPSendDisconnect      ||      
		!lpProcTable->lpWSPSendTo              ||      
		!lpProcTable->lpWSPSetSockOpt          ||       
		!lpProcTable->lpWSPShutdown            ||      
		!lpProcTable->lpWSPSocket              ||       
		!lpProcTable->lpWSPStringToAddress )
		return WSAEINVALIDPROCTABLE;

	EnterCriticalSection(&gCriticalSection);

	NextProcTable = *lpProcTable;

	lpProcTable->lpWSPSocket				= WSPSocket;
	lpProcTable->lpWSPCloseSocket			= WSPCloseSocket;
	lpProcTable->lpWSPConnect				= WSPConnect;
	lpProcTable->lpWSPAccept				= WSPAccept;
	lpProcTable->lpWSPSend					= WSPSend;
	lpProcTable->lpWSPSendTo				= WSPSendTo;
	lpProcTable->lpWSPRecv					= WSPRecv;
	lpProcTable->lpWSPRecvFrom				= WSPRecvFrom;

	LeaveCriticalSection(&gCriticalSection);

	return 0;
}

int WINAPI XfIoControl(
	int					iControlType, 
	XFILTER_IO_CONTROL	*ioControl
)
{
	if(iControlType == IO_CONTROL_SET_WORK_MODE)
		return CCheckAcl::SetWorkMode(ioControl->iWorkMode);
	
	else if(iControlType == IO_CONTROL_GET_WORK_MODE)
		return CCheckAcl::m_iWorkMode;

	else if(iControlType == IO_CONTROL_SET_ACL)
		return CCheckAcl::SetAcl(ioControl->AclFile);

	else if(iControlType == IO_CONTROL_GET_ACL_CHANGE_COUNT)
		return gAclChangeCount;

	else if(iControlType == IO_CONTROL_SET_GUI_INSTANCE)
	{
		EnterCriticalSection(&gCriticalSection);
		{
			CCheckAcl::m_GuiHwnd		= ioControl->hwnd;
			_tcscpy(CCheckAcl::m_sGuiPathName, m_sProcessName);
		}
		LeaveCriticalSection(&gCriticalSection);
	}

	else if(iControlType == IO_CONTROL_GET_SESSION)
	{
		EnterCriticalSection(&gCriticalSection);
		{
			*ioControl->session = CCheckAcl::m_SessionBuf[ioControl->isession];
			CCheckAcl::m_SessionBuf[ioControl->isession].s = 0;
		}
		LeaveCriticalSection(&gCriticalSection);
	}

	else if(iControlType == IO_CONTROL_GET_QUERY_SESSION)
		_tcscpy(ioControl->sPathName, m_QuerySession[ioControl->isession].sPathName);

	else if(iControlType == IO_CONTROL_SET_QUERY_SESSION)
	{
		EnterCriticalSection(&gCriticalSection);
		{
			m_QuerySession[ioControl->isession].status = 0;
		}
		LeaveCriticalSection(&gCriticalSection);
	}

	return XERR_SUCCESS;
}

//=============================================================================================
//Socket Private functions

BOOL QueryAccess()
{
	ODS(_T("Query Access ..."));

	int		i;
	
	for(i = 0; i < MAX_QUERY_SESSION; i++)
	{
		if(m_QuerySession[i].status == 1 
			&& _tcscpy(m_QuerySession[i].sPathName, m_sProcessName) == 0)
		{
			while(m_QuerySession[i].status == 1)
			{
				static int sec = 0;
				sec ++;
				if(sec > 6000)
					return FALSE;
				Sleep(100);
			}
			return TRUE;
		}
	}

	for(i = 0; i < MAX_QUERY_SESSION; i++)
	{
		if(m_QuerySession[i].status == 0)
		{
			EnterCriticalSection(&gCriticalSection);
			{
				m_QuerySession[i].status = 1;
				_tcscpy(m_QuerySession[i].sPathName, m_sProcessName);
			}
			LeaveCriticalSection(&gCriticalSection);

			if(!::PostMessage(CCheckAcl::m_GuiHwnd, WM_QUERY_ACL_NOTIFY, i, NULL))
				return FALSE;

			ODS(_T("Query Access PostMessage ..."));

			while(m_QuerySession[i].status == 1)
			{
				static int sec = 0;
				sec ++;
				if(sec > 600)
					return FALSE;
				Sleep(1000);
			}

			return TRUE;
		}
	}

	if(i >= MAX_QUERY_SESSION)
		return FALSE;

	return TRUE;
}

BOOL GetHookProvider(
	IN	WSAPROTOCOL_INFOW	*pProtocolInfo, 
	OUT	TCHAR				*sPathName
)
{
	TCHAR sItem[21];
	GetRightEntryIdItem(pProtocolInfo, sItem);

	HKEY	hSubkey;
	DWORD	ulDateLenth	= MAX_PATH;
	TCHAR	sTemp[MAX_PATH];


	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_INSTALL_KEY, 0, KEY_ALL_ACCESS, &hSubkey) != ERROR_SUCCESS)
		return FALSE;

	if (RegQueryValueEx(hSubkey, sItem, 0, NULL, (BYTE*)sTemp, &ulDateLenth)
		|| ExpandEnvironmentStrings(sTemp, sPathName, ulDateLenth) == 0)
		return FALSE;

	if(sPathName[0] == '\0' && sTemp[0] != '\0')
		_tcscpy(sPathName, sTemp);

	RegCloseKey(hSubkey);

	return TRUE;
}

void GetRightEntryIdItem(
	IN	WSAPROTOCOL_INFOW	*pProtocolInfo, 
	OUT	TCHAR				*sItem
)
{
	if(pProtocolInfo->ProtocolChain.ChainLen <= 1)
		_stprintf(sItem, _T("%u"), pProtocolInfo->dwCatalogEntryId);
	else
		_stprintf(sItem, _T("%u"), 
		pProtocolInfo->ProtocolChain.ChainEntries[pProtocolInfo->ProtocolChain.ChainLen - 1]);
}

void XfShutdown(SOCKET s)
{
	int		iError;
	if(NextProcTable.lpWSPShutdown(s, SD_BOTH, &iError) != 0)
		WSASetLastError(iError);
}

//=============================================================================================
//Winsock 2 service provider hook functions

SOCKET WSPAPI WSPSocket(
	int			af,                               
	int			type,                             
	int			protocol,                         
	LPWSAPROTOCOL_INFOW lpProtocolInfo,   
	GROUP		g,                              
	DWORD		dwFlags,                        
	LPINT		lpErrno
)
{
	ODS(_T("XFILTER.DLL: WSPSocket ..."));

	SOCKET	s = NextProcTable.lpWSPSocket(af, type, protocol, lpProtocolInfo, g, dwFlags, lpErrno);

	if(s == INVALID_SOCKET)
		return s;

	if (af == FROM_PROTOCOL_INFO)
		af = lpProtocolInfo->iAddressFamily;

	if (type == FROM_PROTOCOL_INFO)
		type = lpProtocolInfo->iSocketType;

	if (protocol == FROM_PROTOCOL_INFO)
		protocol = lpProtocolInfo->iProtocol;

	m_CheckAcl.CheckSocket(s, af, type, protocol);

	return s;
}

int WSPAPI WSPCloseSocket(
	SOCKET		s,
	LPINT		lpErrno
)
{
	ODS(_T("XFILTER.DLL: WSPCloseSocket ..."));

	m_CheckAcl.CheckCloseSocket(s);

	return NextProcTable.lpWSPCloseSocket(s, lpErrno);
}

int WSPAPI WSPConnect(
	SOCKET			s,
	const struct	sockaddr FAR * name,
	int				namelen,
	LPWSABUF		lpCallerData,
	LPWSABUF		lpCalleeData,
	LPQOS			lpSQOS,
	LPQOS			lpGQOS,
	LPINT			lpErrno
)
{
	ODS(_T("XFILTER.DLL: WSPConnect ..."));

	if(m_CheckAcl.CheckConnect(s, name, namelen) != XF_PASS)
	{
		ODS2(_T("Deny the application "), m_sProcessName);
		*lpErrno = WSAECONNREFUSED;
		return SOCKET_ERROR;
	}
 
	return NextProcTable.lpWSPConnect(s, name, namelen, lpCallerData, lpCalleeData, lpSQOS, lpGQOS, lpErrno);
}

SOCKET WSPAPI WSPAccept(
	SOCKET			s,
	struct sockaddr FAR *addr,
	LPINT			addrlen,
	LPCONDITIONPROC	lpfnCondition,
	DWORD			dwCallbackData,
	LPINT			lpErrno
)
{
	ODS(_T("XFILTER.DLL: WSPAccept ..."));

	SOCKET	news	= NextProcTable.lpWSPAccept(s, addr, addrlen, lpfnCondition, dwCallbackData, lpErrno);

	if (news != INVALID_SOCKET && m_CheckAcl.CheckAccept(s, news) != XF_PASS)
	{
		int iError;
		if(NextProcTable.lpWSPCloseSocket(news, &iError) != 0)
			WSASetLastError(iError);
	}

	return news;
}

int WSPAPI WSPSend(
	SOCKET			s,
	LPWSABUF		lpBuffers,
	DWORD			dwBufferCount,
	LPDWORD			lpNumberOfBytesSent,
	DWORD			dwFlags,
	LPWSAOVERLAPPED	lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
	LPWSATHREADID	lpThreadId,
	LPINT			lpErrno
)
{
	ODS(_T("XFILTER.DLL: WSPSend ..."));

	if (m_CheckAcl.CheckSend(s, lpBuffers[0].buf, lpBuffers[0].len, lpNumberOfBytesSent) != XF_PASS)
	{
		XfShutdown(s);
		*lpErrno = WSAECONNABORTED;
		return SOCKET_ERROR;
	}

	return NextProcTable.lpWSPSend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped
				, lpCompletionRoutine, lpThreadId, lpErrno);

}

int WSPAPI WSPSendTo(
	SOCKET			s,
	LPWSABUF		lpBuffers,
	DWORD			dwBufferCount,
	LPDWORD			lpNumberOfBytesSent,
	DWORD			dwFlags,
	const struct sockaddr FAR * lpTo,
	int				iTolen,
	LPWSAOVERLAPPED	lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
	LPWSATHREADID	lpThreadId,
	LPINT			lpErrno
)
{
	ODS(_T("XFILTER.DLL: WSPSendTo ..."));

	if (m_CheckAcl.CheckSendTo(s, lpTo, lpBuffers[0].buf, lpBuffers[0].len, lpNumberOfBytesSent) != XF_PASS)
	{
		XfShutdown(s);
		*lpErrno = WSAECONNABORTED;
		return SOCKET_ERROR;
	}

	return NextProcTable.lpWSPSendTo(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpTo
			, iTolen, lpOverlapped, lpCompletionRoutine, lpThreadId, lpErrno);

}

int WSPAPI WSPRecv(
	SOCKET			s,
	LPWSABUF		lpBuffers,
	DWORD			dwBufferCount,
	LPDWORD			lpNumberOfBytesRecvd,
	LPDWORD			lpFlags,
	LPWSAOVERLAPPED	lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
	LPWSATHREADID	lpThreadId,
	LPINT			lpErrno
)
{
	ODS(_T("XFILTER.DLL: WSPRecv ..."));

	int		iRet;

	if(iRet = NextProcTable.lpWSPRecv(s, lpBuffers, 1, lpNumberOfBytesRecvd, lpFlags, lpOverlapped
					, lpCompletionRoutine, lpThreadId, lpErrno) == SOCKET_ERROR)
		return iRet;

	if (m_CheckAcl.CheckRecv(s, lpBuffers[0].buf, lpBuffers[0].len, lpNumberOfBytesRecvd) != XF_PASS)
	{
		XfShutdown(s);
		*lpErrno = WSAECONNABORTED;
		return SOCKET_ERROR;
	}

	return iRet;
}

int WSPAPI WSPRecvFrom (
	SOCKET			s,
	LPWSABUF		lpBuffers,
	DWORD			dwBufferCount,
	LPDWORD			lpNumberOfBytesRecvd,
	LPDWORD			lpFlags,
	struct sockaddr FAR * lpFrom,
	LPINT			lpFromlen,
	LPWSAOVERLAPPED lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
	LPWSATHREADID	lpThreadId,
	LPINT			lpErrno
)
{
	ODS(_T("XFILTER.DLL: WSPRecvFrom ..."));

	int		iRet;

	if(iRet = NextProcTable.lpWSPRecvFrom(s, lpBuffers, 1, lpNumberOfBytesRecvd, lpFlags, lpFrom
			, lpFromlen, lpOverlapped, lpCompletionRoutine, lpThreadId, lpErrno) == SOCKET_ERROR)
		return iRet;

	if (m_CheckAcl.CheckRecvFrom(s, lpFrom, lpBuffers[0].buf, lpBuffers[0].len, lpNumberOfBytesRecvd) != XF_PASS)
	{
		XfShutdown(s);
		*lpErrno = WSAECONNABORTED;
		return SOCKET_ERROR;
	}

	return iRet;
}
