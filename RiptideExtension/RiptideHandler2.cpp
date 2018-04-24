// RiptideHandler2.cpp : Implementation of CRiptideHandler2
#pragma region Includes
#include "stdafx.h"
#include "RiptideHandler2.h"
#include "Helper.h"
#pragma endregion
// CRiptideHandler2 - Need to be synced (modified)
/*
*	This class will be used to indicate any document or folder on the local file system has been modified, need to be synced with the server
*	2012, March 21
*/

/*
*	Provides the location of the icon overlay's bitmap.
*/

IFACEMETHODIMP CRiptideHandler2::GetOverlayInfo(
  LPWSTR pwszIconFile,
  int cchMax,int* pIndex,
  DWORD* pdwFlags)
{
	GetModuleFileNameW(_AtlBaseModule.GetModuleInstance(), pwszIconFile, cchMax);

	*pIndex = 1;
	*pdwFlags = ISIOI_ICONFILE | ISIOI_ICONINDEX;
	return S_OK;
}
/*
*	Specifies the priority of an icon overlay.
*/
IFACEMETHODIMP CRiptideHandler2::GetPriority(int* pPriority)
{
	// highest priority
	*pPriority=0;
	return S_OK;
}

/*
*  Specifies whether an icon overlay should be added to a Shell object's icon.
*/
IFACEMETHODIMP CRiptideHandler2::IsMemberOf(LPCWSTR pwszPath,DWORD dwAttrib)
{
	try
	{
		if (PathIsDirectoryW(pwszPath))
			return S_FALSE;

		if (CHelper::IsGoDriveItem(pwszPath) && !CHelper::IsLocal(pwszPath))
			return S_OK;
	}
	catch(...){}
	return S_FALSE;
}