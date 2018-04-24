// RiptideExtension.cpp : Implementation of DLL Exports.


#include "stdafx.h"
#include "resource.h"
#include "RiptideExtension_i.h"
#include "dllmain.h"

static HINSTANCE g_ResInstance = NULL;
HINSTANCE GetResounceInstance(LPCTSTR strDll);
HINSTANCE GetResInstance();
// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
    return _AtlModule.DllCanUnloadNow();
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}

// DllInstall - Adds/Removes entries to the system registry per user
//              per machine.	
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine)
{
    HRESULT hr = E_FAIL;
    static const wchar_t szUserSwitch[] = _T("user");

    if (pszCmdLine != NULL)
    {
    	if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0)
    	{
    		AtlSetPerUserRegistration(true);
    	}
    }

    if (bInstall)
    {	
    	hr = DllRegisterServer();
    	if (FAILED(hr))
    	{	
    		DllUnregisterServer();
    	}
		else{
			g_ResInstance = GetResounceInstance(_T("FlowCMExtensionRes.dll"));
			if (g_ResInstance != NULL){				
				_AtlBaseModule.m_hInst = g_ResInstance;
			}
		}
    }
    else
    {
    	hr = DllUnregisterServer();
    }

    return hr;
}

HINSTANCE GetResInstance()
{
	if (g_ResInstance == NULL){
		g_ResInstance = _AtlBaseModule.m_hInst;
	}
	return g_ResInstance;
}

HINSTANCE GetResounceInstance(LPCTSTR strDll)
{
	HINSTANCE hInst = NULL;

	static TCHAR szFilePath[_MAX_PATH*4] = _T("\0");

	CRegKey key;
    LONG nError = key.Open(HKEY_LOCAL_MACHINE, SHARE_SYNC_HIVE, KEY_READ);
    if(nError != ERROR_SUCCESS)
	{
		return hInst;
	}

	TCHAR szPath[MAX_PATH*4];
	DWORD cbSize;
	cbSize = sizeof( szPath ) / sizeof( TCHAR );
	nError = key.QueryStringValue( _T("InstallPath"), szPath, &cbSize );
	key.Close();
	if (nError != ERROR_SUCCESS)
	{
		return hInst;
	}

	if(_tcslen(szPath) >0)
	{
		LCID lcid = GetThreadLocale();
		
		WORD  lid = LANGIDFROMLCID(lcid);
		TCHAR buf[16];
		swprintf_s(buf, _T("\\%d\\"), lid);
		wcscpy_s(szFilePath, szPath);
		PathAppendW(szFilePath, buf);
		PathAppendW(szFilePath, strDll);
		
		g_ResInstance = ::LoadLibrary(szFilePath);

		//find main language lcid
		if(0==g_ResInstance && lid != 1033)
		{
			lid = MAKELANGID(PRIMARYLANGID(lid), SUBLANG_DEFAULT); 
			swprintf_s(buf, _T("\\%d\\"), lid);
			wcscpy_s(szFilePath, szPath);
			PathAppendW(szFilePath, buf);
			PathAppendW(szFilePath, strDll);
		
			g_ResInstance = ::LoadLibrary(szFilePath);

			if(0==g_ResInstance)
			{
				wcscpy_s(szFilePath, szPath);
				PathAppendW(szFilePath, _T("\\1033\\"));
				PathAppendW(szFilePath, strDll);
		
				g_ResInstance = ::LoadLibrary(szFilePath);
			}
		}
		return g_ResInstance;
	}
	return NULL;
}
