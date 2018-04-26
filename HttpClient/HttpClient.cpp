// HttpClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <wininet.h>  

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"


using namespace rapidjson;
bool IsLocal(TCHAR* path)
{
	bool result = false;

	CString url;
	url.Format(_T("is_local?path=%s"), path);
	CString host = _T("localhost");
	int port = 8010;


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
int main()
{
	TCHAR* path = _T("C:\\Users\\qing.chen^\\Downloads}\\New dump files");

	TCHAR pathEscaped[1024] = { NULL };
	DWORD dwSize = sizeof(pathEscaped) / sizeof(TCHAR);
	HRESULT hr = UrlEscape(path, pathEscaped, &dwSize, URL_ESCAPE_SEGMENT_ONLY);
	pathEscaped[dwSize] = NULL;

	bool result = IsLocal(pathEscaped);
}

