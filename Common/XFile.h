//=============================================================================================
/*
	XFile.h
	File operate function of ACL file

	Project	: XFILTER 1.0 Personal Firewall
	Author	: Tony Zhu
	Create Date	: 2001/08/03
	Email	: xstudio@xfilt.com
	URL		: http://www.xfilt.com

	Copyright (c) 2001-2002 XStudio Technology.
	All Rights Reserved.

	WARNNING: 
*/
//=============================================================================================

#include "XFileRes.h"
#include "Debug.h"

//=============================================================================================
// Internet Info

typedef struct _XUSER_INFO
{
	TCHAR		sEmail[51];
	TCHAR		sUserName[21];
	TCHAR		sPassword[21];
	TCHAR		sQQ[13];
	TCHAR		sICQ[13];
	TCHAR		sName[21];
	BYTE		bIdType;
	TCHAR		sId[21];
	int			iStatus;
	BYTE		bGender;
	BYTE		bMetier;
	TCHAR		sDuty[21];
	BYTE		bDegree;
	BYTE		bSalary;
	CTime		tBirthday;
	WORD 		wCountry;
	WORD 		wCity;
	TCHAR		sAddress[51];
	TCHAR		sZip[11];
	TCHAR		sInc[51];
} XUSER_INFO, *PXUSER_INFO;

typedef struct _XNET_COMMAND_HEADER
{
	long		lVersion;
	long		lCount;
	CTime		tCheckTime;
} XNET_COMMAND_HEADER, *PXNET_COMMAND_HEADER;

//=============================================================================================
// ACL file

typedef struct _XACL_HEADER
{
	TCHAR		sSignature[16];
	DWORD		ulHeaderLenth;
	BYTE		bMajor;
	BYTE		bMinor;
	UINT		uiSerial;
	DWORD		ulVersion;
	BYTE		bSet;
	UINT		uiLogSize;
	BYTE		bUpdateInterval;
	TCHAR		sUserName[16];
	TCHAR		sACode[16];
	TCHAR		sWebURL[MAX_PATH];
	TCHAR		sCommandURL[MAX_PATH];
	TCHAR		sUserRegisterURL[MAX_PATH];
	TCHAR		sEmail[MAX_PATH];
	DWORD		ulAclOffset;
	DWORD		ulAclCount;
	DWORD		ulIntranetIPOffset;
	DWORD		ulIntranetIPCount;
	DWORD		ulDistrustIPOffset;
	DWORD		ulDistrustIPCount;
	DWORD		ulTrustIPOffset;
	DWORD		ulTrustIPCount;
	DWORD		ulCustomIPOffset;
	DWORD		ulCustomIPCount;
	DWORD		ulTimeOffset;
	DWORD		ulTimeCount;
} XACL_HEADER, *PXACL_HEADER;

typedef struct _XACL
{
	DWORD		ulAclID;
	TCHAR		sApplication[MAX_PATH];
	BYTE		bRemoteNetType;
	BYTE		bAccessTimeType;
	BYTE		bAction;
	BYTE		bDirection;
	BYTE		bServiceType;
	UINT		uiServicePort;
	TCHAR		sMemo[51];
} XACL, *PXACL;

typedef struct _XACL_IP
{
	DWORD		ulStartIP;
	DWORD		ulEndIP;
} XACL_IP, *PXACL_IP;

typedef struct _XACL_TIME
{
	BYTE		bWeekDay;
	CTime		tStartTime;
	CTime		tEndTime;
} XACL_TIME, *PXACL_TIME;

typedef struct _XACL_FILE
{
	XACL_HEADER		mAclHeader;
	XACL_IP			mAclIntranetIP;
	XACL_TIME		mAclTime		[ACL_HEADER_TIME_COUNT];
	XACL_IP			mpAclDistrustIP	[MAX_IP_ARIA];
	XACL_IP			mpAclTrustIP	[MAX_IP_ARIA];
	XACL_IP			mpAclCustomIP	[MAX_IP_ARIA];
	XACL			mpAcl			[MAX_ACL];
} XACL_FILE, *PXACL_FILE;

//=============================================================================================
// the packet struct of TCPIPDOG Capture 

typedef struct _SESSION
{
	SOCKET		s;
	DWORD		ulRemoteIP;
	CTime		tStartTime;
	BYTE		bDirection;
	BYTE		bProtocol;
	UINT		uiPort;
	BYTE		bAction;
	UINT		uiLocalPort;
	DWORD		ulLocalIP;
	DWORD		ulSendData;
	DWORD		ulRecvData;
	TCHAR		sPathName[MAX_PATH];
	TCHAR		sMemo[MAX_PATH];
	CTime		tEndTime;
} SESSION, *PSESSION;

#define SESSION_LENTH		sizeof(SESSION)

//=============================================================================================
// the query session 

typedef struct _QUERY_SESSION
{
	BYTE		status;				//0: no use, 1: using
	TCHAR		sPathName[MAX_PATH];
} QUERY_SESSION, *PQUERY_SESSION;

//=============================================================================================
// XFITLER Io Control struct

typedef BOOL (WINAPI * ADD_ACL_QUERY)(TCHAR* sPathName, BOOL EnableComboApplication = FALSE);

typedef struct _XFILTER_IO_CONTROL
{
	int				iWorkMode;
	XACL_FILE		AclFile;
	ADD_ACL_QUERY	pAddAclQuery;
	HWND			hwnd;
	SESSION			*session;
	int				isession;
	TCHAR			sPathName[MAX_PATH];
} XFILTER_IO_CONTROL, *PXFILTER_IO_CONTROL;

typedef int  (WINAPI * XF_IO_CONTROL)(int iControlType, XFILTER_IO_CONTROL* ioControl);

//=============================================================================================
// class of Acl file

class CAclFile
{
private:
	CRITICAL_SECTION	gCriticalSectionFile;

private:
	void	InitDefaultValue();
	int		CreateAcl	(const TCHAR *sPathName);
	int		WriteAcl	(const TCHAR *sPathName);
	int		OpenAcl		();
	int		AddAclAcl	(XACL *pAddAcl, int AddAclCount);
	int		AddDistrustIP(XACL_IP *pDistrustIP, int AddCount);
	int		AddTrustIP	(XACL_IP *pTrustIP, int AddCount);
	int		AddCustomIP	(XACL_IP *pCustomIP, int AddCount);
	int		DelAclIP	(XACL_IP *pAclIP, DWORD* pCount, DWORD iIndex);
	int		DelAcl		(XACL *pAcl, DWORD* pCount, DWORD iIndex);

public:
	CAclFile();
	virtual ~CAclFile	();
	void	CloseAcl	();
	int		ReadAcl		(BOOL IsDLL = FALSE, HINSTANCE instance = NULL);
	int		SaveAcl		();
	int		AddAcl		(void *pAddAcl, int AddAclCount, int AclType = 0);
	int		DelAcl		(int iIndex, int AclType);
	int		FreeMemory	();

public:
	static	int		GetBit		(BYTE bit, int index, int count = 1);
	static	int		SetBit		(BYTE* bit, int index, BOOL isTrue);
	static	CString	DIPToSIP	(DWORD* pIP);
	static  CString	GetAppPath	(BOOL IsDLL = FALSE, HINSTANCE instance = NULL,  BOOL IsFullPathName = FALSE);
	static	CString GetPath		(TCHAR *sFilename);
	static	CString GetName		(TCHAR *sFilename);

public:
	CFile			mAclFile;
	XACL_HEADER		mAclHeader;
	XACL_IP			mAclIntranetIP;
	XACL_TIME		mAllTime;
	XACL_TIME		mAclTime[ACL_HEADER_TIME_COUNT];
	XACL_IP			*mpAclDistrustIP;
	XACL_IP			*mpAclTrustIP;
	XACL_IP			*mpAclCustomIP;
	XACL			*mpAcl;
	CString			mAppPath;
};