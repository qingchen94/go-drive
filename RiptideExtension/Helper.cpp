#include "stdafx.h"
#include "Helper.h"


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
			if (-1 != sRet.Find(_T("localhost@8880")))
				bRet = true;
		}
		delete[] buf;
	}
	return bRet;
}

bool CHelper::IsLocal(const CString& path)
{
	return true;
}