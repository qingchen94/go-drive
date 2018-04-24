// RiptideHandler3.cpp : Implementation of CRiptideHandler3
#pragma region Includes
#include "stdafx.h"
#include "RiptideHandler3.h"
#include "RTStatusManager.h"
#pragma endregion
// CRiptideHandler3 - no sync
/*
*	This class will be used to indicate any document or folder on the local file system cannot be synced with the server for some reason
*	2012, March 21
*/
/*
*	Provides the location of the icon overlay's bitmap.
*/
extern class CRTStatusManager _rtManager;

IFACEMETHODIMP CRiptideHandler3::GetOverlayInfo(
  LPWSTR pwszIconFile,
  int cchMax,int* pIndex,
  DWORD* pdwFlags)
{
	GetModuleFileNameW(_AtlBaseModule.GetModuleInstance(), pwszIconFile, cchMax);

	*pIndex = 2;
	*pdwFlags = ISIOI_ICONFILE | ISIOI_ICONINDEX;
	return S_OK;
}
/*
*	Specifies the priority of an icon overlay.
*/
IFACEMETHODIMP CRiptideHandler3::GetPriority(int* pPriority)
{
	// highest priority
	*pPriority=2;
	return S_OK;
}

/*
*  Specifies whether an icon overlay should be added to a Shell object's icon.
*/
IFACEMETHODIMP CRiptideHandler3::IsMemberOf(LPCWSTR pwszPath,DWORD dwAttrib)
{
	//try
	//{
	//	int nIdx = _rtManager.GetICONIndex(pwszPath, dwAttrib);
	//	if (nIdx == 2) return S_OK;
	//}
	//catch(...){}
	return S_FALSE;
}
