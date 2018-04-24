// RiptideHandler4.cpp : Implementation of CRiptideHandler4
#pragma region Includes
#include "stdafx.h"
#include "RiptideHandler4.h"
#include "Helper.h"
#pragma endregion
// CRiptideHandler4 - synced
/*
*	This class will be used to indicate any document or folder on the local file system has been updated and synced with the server
*	2012, March 21
*/
/*
*	Provides the location of the icon overlay's bitmap.
*/

IFACEMETHODIMP CRiptideHandler4::GetOverlayInfo(
  LPWSTR pwszIconFile,
  int cchMax,int* pIndex,
  DWORD* pdwFlags)
{
	GetModuleFileNameW(_AtlBaseModule.GetModuleInstance(), pwszIconFile, cchMax);

	*pIndex = 3;
	*pdwFlags = ISIOI_ICONFILE | ISIOI_ICONINDEX;
	return S_OK;
}
/*
*	Specifies the priority of an icon overlay.
*/
IFACEMETHODIMP CRiptideHandler4::GetPriority(int* pPriority)
{
	// highest priority
	*pPriority=1;
	return S_OK;
}

/*
*  Specifies whether an icon overlay should be added to a Shell object's icon.
*/
IFACEMETHODIMP CRiptideHandler4::IsMemberOf(LPCWSTR pwszPath,DWORD dwAttrib)
{
	try{
		if (CHelper::IsGoDriveItem(pwszPath) && CHelper::IsLocal(pwszPath))
			return S_OK;
	}
	catch(...){}
	return S_FALSE;
}

