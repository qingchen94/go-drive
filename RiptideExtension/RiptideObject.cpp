#include "StdAfx.h"
#include "RiptideObject.h"

#define EPOCH_DIFF 116444736000000000
#define RATE_DIFF 10000

CRiptideObject::CRiptideObject(void)
{
	m_syncStatus = enumSyncDefault;
	m_fileSize = 0;
	m_readOnly = false;
	m_fileVersion = 0;
	m_name = m_path = m_ID = m_MD5 = m_hash = L"";
	m_fileModifiedTicks = m_fileCreatedTicks = 0LL;	
	m_bShared = false;
	m_bPublished = false;
}

CRiptideObject::~CRiptideObject(void)
{
}

FILETIME CRiptideObject::UnixTimeToFileTime(LONGLONG llTime)
{
	LONGLONG wUTC = (llTime * RATE_DIFF) + EPOCH_DIFF;
	FILETIME ft;
	
	ft.dwLowDateTime = (DWORD)wUTC;
	ft.dwHighDateTime = (DWORD)(wUTC >> 32);
	return ft;
}