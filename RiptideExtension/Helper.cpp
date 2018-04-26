#include "stdafx.h"
#include "Helper.h"
#include <Shlwapi.h>
#include <WinInet.h>

#include "rapidjson/document.h"
using namespace rapidjson;

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
	//bool bRet = false;
	//if (g_localFiles.Lookup(path, bRet))
	//	return true;
	//else
	//	return false;

	bool result = false;
	CString host = _T("localhost");
	int port = 8010;

	// Escape the path
	TCHAR pathEscaped[MAX_PATH] = { NULL };
	DWORD dwSize = sizeof(pathEscaped) / sizeof(TCHAR);
	HRESULT hr = UrlEscape(path, pathEscaped, &dwSize, URL_ESCAPE_SEGMENT_ONLY);
	pathEscaped[dwSize] = NULL;

	CString url;
	if (SUCCEEDED(hr))
		url.Format(_T("is_local?path=%s"), pathEscaped);
	else
		url.Format(_T("is_local?path=%s"), path);

	HINTERNET hIntSession =
		InternetOpen(_T("Go!Drive"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

	if (hIntSession == NULL)
		return result;

	HINTERNET hHttpSession =
		InternetConnect(hIntSession, host, port, 0, 0, INTERNET_SERVICE_HTTP, 0, NULL);

	if (hHttpSession == NULL)
		return result;

	HINTERNET hHttpRequest = HttpOpenRequest(
		hHttpSession,
		_T("PUT"),
		url,
		0, 0, 0, INTERNET_FLAG_RELOAD, 0);

	if (hHttpSession == NULL)
		return result;

	TCHAR* szHeaders = _T("Content-Type: text/*");
	CHAR szReq[1024] = "";
	if (!HttpSendRequest(hHttpRequest, szHeaders, _tcslen(szHeaders), szReq, strlen(szReq))) {
		DWORD dwErr = GetLastError();
		/// handle error
	}

	CHAR szBuffer[1025];
	std::string json = "";
	DWORD dwRead = 0;
	while (InternetReadFile(hHttpRequest, szBuffer, sizeof(szBuffer) - 1, &dwRead) && dwRead) {
		szBuffer[dwRead] = NULL;
		json += szBuffer;
		dwRead = 0;
	}

	//json = "{ \"result\": true }";
	Document d;
	d.Parse(json.c_str());

	if (d.IsObject())
	{
		if (d.HasMember("result"))
		{
			if (d["result"].IsBool())
			{
				result = d["result"].GetBool();
			}
		}
	}

	InternetCloseHandle(hHttpRequest);
	InternetCloseHandle(hHttpSession);
	InternetCloseHandle(hIntSession);

	return result;
}

void CHelper::AddFileToLocal(const CString& file)
{
	g_localFiles[file] = true;
}

void CHelper::RemoveFileFromLocal(const CString& file)
{
	g_localFiles.RemoveKey(file);
}