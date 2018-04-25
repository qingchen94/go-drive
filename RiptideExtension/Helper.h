#pragma once
#include <atlstr.h>
#include <atlcoll.h>
#include "resource.h"

class CHelper
{
public:
	CHelper();
	virtual ~CHelper();

	static bool IsGoDriveItem(const CString& sPath);
	static bool IsLocal(const CString& path);
	static void AddFileToLocal(const CString& file);
	static void RemoveFileFromLocal(const CString& file);

private:
	static CAtlMap<CString, bool> g_localFiles;
};

