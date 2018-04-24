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
};

