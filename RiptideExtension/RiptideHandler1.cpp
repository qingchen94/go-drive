// RiptideHandler1.cpp : Implementation of CRiptideHandler1

#pragma region Includes
#include "stdafx.h"
#include "RiptideHandler1.h"
#include "RTStatusManager.h"
#pragma endregion
// CRiptideHandler1 - Sync Conflict
/*
*	This class will be used to indicate any document or folder on the local file system has a conflict to sync with the server
*	2012, March 21
*/

/*
*	Provides the location of the icon overlay's bitmap.
*/
extern class CRTStatusManager _rtManager;

IFACEMETHODIMP CRiptideHandler1::GetOverlayInfo(
  LPWSTR pwszIconFile,
  int cchMax,int* pIndex,
  DWORD* pdwFlags)
{
	GetModuleFileNameW(_AtlBaseModule.GetModuleInstance(), pwszIconFile, cchMax);

	*pIndex = 0;
	*pdwFlags = ISIOI_ICONFILE | ISIOI_ICONINDEX;
	return S_OK;
}
/*
*	Specifies the priority of an icon overlay.
*/
IFACEMETHODIMP CRiptideHandler1::GetPriority(int* pPriority)
{
	// highest priority
	*pPriority=0;
	return S_OK;
}

/*
*  Specifies whether an icon overlay should be added to a Shell object's icon.
*/
IFACEMETHODIMP CRiptideHandler1::IsMemberOf(LPCWSTR pwszPath,DWORD dwAttrib)
{
	try{
		if (dwAttrib != 0xffffffff && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
			return S_FALSE;

		int nIdx = _rtManager.GetICONIndex(pwszPath, dwAttrib);
		if (nIdx == 0) 
			return S_OK;		
	}
	catch(...){}
	return S_FALSE;
}

