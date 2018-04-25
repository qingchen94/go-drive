#include "stdafx.h"
#include "Helper.h"

CAtlMap<CString, bool> CHelper::g_localFiles;

CHelper::CHelper()
{
}


CHelper::~CHelper()
{
}

bool CHelper::IsGoDriveItem(const CString& sPath)
{
	TCHAR temp;
	bool bRet = false;
	UNIVERSAL_NAME_INFO * puni = NULL;
	CString sRet = sPath;
	DWORD bufsize = 0;
	//Call WNetGetUniversalName using UNIVERSAL_NAME_INFO_LEVEL option
	if (WNetGetUniversalName(sPath,
		UNIVERSAL_NAME_INFO_LEVEL,
		(LPVOID)&temp,
		&bufsize) == ERROR_MORE_DATA)
	{
		// now we have the size required to hold the UNC path
		TCHAR * buf = new TCHAR[bufsize + 1];
		puni = (UNIVERSAL_NAME_INFO *)buf;
		if (WNetGetUniversalName(sPath,
			UNIVERSAL_NAME_INFO_LEVEL,
			(LPVOID)puni,
			&bufsize) == NO_ERROR)
		{
			sRet = puni->lpUniversalName;
			sRet.MakeLower();
			if (-1 != sRet.Find(_T("\\davwwwroot\\go!drive")))
				bRet = true;
		}
		delete[] buf;
	}
	return bRet;
}

bool CHelper::IsLocal(const CString& path)
{
	bool bRet = false;
	if (g_localFiles.Lookup(path, bRet))
		return true;

	return false;
}

void CHelper::AddFileToLocal(const CString& file)
{
	g_localFiles[file] = true;
}

void CHelper::RemoveFileFromLocal(const CString& file)
{
	g_localFiles.RemoveKey(file);
}