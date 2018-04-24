// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include "resource.h"

//ATL
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <atlfile.h>

using namespace ATL;

// Shell
#include <comdef.h>
#include <shlobj.h>

#include <comutil.h>
#include <map>
#include <list>

#define SHARE_SYNC_HIVE L"Software\\iManage\\Share Sync"

enum EnumSyncStatus
{
	enumSyncDefault = 0,
	enumSyncNew,
	enumSyncConflict,
	enumSyncModified,
	enumSyncStopped,
	enumSyncUpdated
};

enum TrusteesRights
{
	NO_ACCESS = 0,
	READ = 1,
	READ_WRITE = 2
};
