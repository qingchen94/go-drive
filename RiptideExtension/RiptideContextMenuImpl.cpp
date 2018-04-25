//RiptideContextMenuImpl.cpp : Implementation of CRiptideContextMenuImpl

/*********************************** Module Header ***********************************\
* Module Name:  RiptideContextMenuImpl.cpp
* Project:      RiptideExtension
* Copyright (c) Autonomy
* 
* 
**************************************************************************************/

#pragma region Includes
#include "stdafx.h"
#include "RiptideContextMenuImpl.h"
#include <strsafe.h>
#include "dllmain.h"
#include "TransparentBitmap.h"
#include "Helper.h"
#pragma endregion

#ifdef _UNICODE
	#define CF_TEXT_FORMAT      CF_UNICODETEXT
    #define ALLOC_GLOBAL_STRING(hGlobal, strText) HGLOBAL hGlobal = ::GlobalAlloc(GMEM_SHARE, ((wcslen(strText) + 1) * 2));
#else
	  #define CF_TEXT_FORMAT      CF_TEXT
	  #define ALLOC_GLOBAL_STRING(hGlobal, strText) HGLOBAL hGlobal = ::GlobalAlloc(GMEM_SHARE, strlen(strText) + 1);
#endif
/*************************************************************************************\
* CRiptideContextMenuImpl IShellExtInit methods.
*
*
*   FUNCTION: CRiptideContextMenuImpl::Initialize(LPCITEMIDLIST, LPDATAOBJECT, HKEY)
*
*
*   PURPOSE: Initializes the context menu extension.
**************************************************************************************/
IFACEMETHODIMP CRiptideContextMenuImpl::Initialize(
    LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID)
{
    HRESULT hr = E_INVALIDARG;
    
	m_selectedFiles.RemoveAll();
	ZeroMemory(m_szFileName, sizeof(m_szFileName));

    if (NULL == pDataObj)
    {
        return hr;
    }

    FORMATETC fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stm;

    // pDataObj contains the objects being acted upon. In this example, 
    // we get an HDROP handle for enumerating the selected files.
    if (SUCCEEDED(pDataObj->GetData(&fe, &stm)))
    {
        // Get an HDROP handle.
        HDROP hDrop = static_cast<HDROP>(GlobalLock(stm.hGlobal));
        if (hDrop != NULL)
        {
            // Determine how many files are involved in this operation.
            UINT nFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);

			for (UINT i = 0; i < nFiles; i++)
			{
                UINT nSize = DragQueryFile(hDrop, i, NULL, MAX_PATH + 1);
				if (nSize != 0)
				{
					TCHAR* buffer = new TCHAR[nSize + 1];
					
					if (0 != DragQueryFile(hDrop, i, buffer, nSize+1))
					{
						CString fileName(buffer);
						m_selectedFiles.Add(fileName);
						ATLTRACE2("File = %s", (LPCTSTR) fileName);
					}
					delete[] buffer;
				}
			}

			if (nFiles > 0)
			{
				CString fileName = m_selectedFiles[0];
				_tcscpy_s(m_szFileName, fileName);
			}


            GlobalUnlock(stm.hGlobal);
        }

        ReleaseStgMedium(&stm);

		return S_OK;
    }

    return hr;
}

/*************************************************************************************\
* CRiptideContextMenuImpl IContextMenu methods.
* 
*
*   FUNCTION: CRiptideContextMenuImpl::OnAction(HWND)
*
*   PURPOSE: OnAction handles the "xxx" verb of the shell extension.
**************************************************************************************/
void CRiptideContextMenuImpl::OnAction(HWND hWnd, UINT idCmd)
{	
	CString Ops = CString(_T("context_menu ")) +  menuItemsData[idCmd] +
		_T(" \"") + m_szFileName + _T("\"");

	HINSTANCE hInst = NULL;
	CString strFilePath;
	TCHAR NPath[MAX_PATH];

	HMODULE handle = GetModuleHandle(_T("GoDriveShellExtension.dll"));
	if (handle == nullptr)
	{
		AtlTrace("failed to get the module handle of GoDriveShellExtension.dll !");
		return;
	}
	// When passing NULL to GetModuleHandle, it returns handle of exe itself
	if (0 == GetModuleFileName(handle, NPath, MAX_PATH))
	{
		AtlTrace("failed to get the module handle of GoDriveShellExtension.dll !");
		return;
	}

	CString strDllDir = NPath;
	strDllDir = strDllDir.Left(strDllDir.ReverseFind(_T('\\')));
	CString strExe = strDllDir + _T("\\") + _T("go_drive.exe");

	ATLTRACE2(_T("Ops = %s\n"), Ops);
	ATLTRACE2(_T("go_drive.exe = %s\n"), strDllDir);

	ShellExecute(hWnd, _T("open"), strExe, Ops, NULL, SW_SHOW);

	if (menuItemsData[idCmd] == "-d")
		CHelper::AddFileToLocal(m_szFileName);
	else if (menuItemsData[idCmd] == "-r")
		CHelper::RemoveFileFromLocal(m_szFileName);

	SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATH, m_szFileName, NULL);

	return;
}

static UINT g_idCmdFirst;
static int g_idCmdCount;

/*************************************************************************************\
*   FUNCTION: CRiptideContextMenuImpl::QueryContextMenu(HMENU, UINT, UINT, UINT, UINT)
*
*   PURPOSE: The Shell calls IContextMenu::QueryContextMenu to allow the 
*            context menu handler to add its menu items to the menu. It 
*            passes in the HMENU handle in the hmenu parameter. The 
*            indexMenu parameter is set to the index to be used for the 
*            first menu item that is to be added.
**************************************************************************************/
IFACEMETHODIMP CRiptideContextMenuImpl::QueryContextMenu(
    HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
// If uFlags include CMF_DEFAULTONLY then we should not do anything
    if (CMF_DEFAULTONLY & uFlags)
    {
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(0));
    }

	g_idCmdFirst = idCmdFirst;
	UINT uID = idCmdFirst;
	g_idCmdCount =  menuItemsVerb.GetSize();
	if (hBitmapRemove == NULL)
	{
		HICON hIcon = (HICON) LoadImage(GetResInstance(), MAKEINTRESOURCE(IDI_ICON5), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
		hBitmapRemove = CTransparentBitmap().CreateBitmap32(hIcon);
		DeleteObject(hIcon);
	}

	if (hBitmapDownload == NULL)
	{
		HICON hIcon = (HICON)LoadImage(GetResInstance(), MAKEINTRESOURCE(IDI_ICON6), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
		hBitmapDownload = CTransparentBitmap().CreateBitmap32(hIcon);
		DeleteObject(hIcon);
	}

	if (hBitmapPreview == NULL)
	{
		HICON hIcon = (HICON)LoadImage(GetResInstance(), MAKEINTRESOURCE(IDI_ICON7), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
		hBitmapPreview = CTransparentBitmap().CreateBitmap32(hIcon);
		DeleteObject(hIcon);
	}

	if (m_selectedFiles.GetCount() == 1 && CHelper::IsGoDriveItem(m_selectedFiles[0]))
	{
		InsertMenu(hMenu, indexMenu, MF_SEPARATOR | MF_BYPOSITION, -1, _T(""));
		indexMenu++;

		if (CHelper::IsLocal(m_selectedFiles[0]))
		{
			//Remove
			InsertMenu(hMenu, indexMenu, MF_STRING | MF_BYPOSITION, (UINT)uID + 2, OLE2T(menuItems[2]));
			SetMenuItemBitmaps(hMenu, indexMenu, MF_BYPOSITION, static_cast<HBITMAP>(hBitmapRemove), static_cast<HBITMAP>(hBitmapRemove));
			indexMenu++;
		}
		else
		{
			//Download
			InsertMenu(hMenu, indexMenu, MF_STRING | MF_BYPOSITION, (UINT)uID + 1, OLE2T(menuItems[1]));
			SetMenuItemBitmaps(hMenu, indexMenu, MF_BYPOSITION, static_cast<HBITMAP>(hBitmapDownload), static_cast<HBITMAP>(hBitmapDownload));
			indexMenu++;
		}

		//Preview
		InsertMenu(hMenu, indexMenu, MF_STRING | MF_BYPOSITION, (UINT)uID, OLE2T(menuItems[0]));
		SetMenuItemBitmaps(hMenu, indexMenu, MF_BYPOSITION, static_cast<HBITMAP>(hBitmapPreview), static_cast<HBITMAP>(hBitmapPreview));
		indexMenu++;

		//add a separater 
		InsertMenu(hMenu, indexMenu++, MF_SEPARATOR | MF_BYPOSITION, -1, _T(""));
	}

    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, uID + 4 - idCmdFirst);
}

/**************************************************************************************\
*   FUNCTION: CRiptideContextMenuImpl::GetCommandString(UINT, UINT, LPUINT, 
*             LPSTR, UINT)
*
*   PURPOSE: If a user highlights one of the items added by a context menu 
*            handler, the handler's IContextMenu::GetCommandString method is 
*            called to request a Help text string that will be displayed on 
*            the Windows Explorer status bar. This method can also be called 
*            to request the verb string that is assigned to a command. 
*            Either ANSI or Unicode verb strings can be requested.
**************************************************************************************/
#if defined (_M_X64)

IFACEMETHODIMP CRiptideContextMenuImpl::GetCommandString(
    UINT_PTR idCommand, UINT uFlags, LPUINT lpReserved, LPSTR pszName, 
    UINT uMaxNameLen)
{
    HRESULT hr = E_INVALIDARG;
	USES_CONVERSION;

    switch (uFlags)
    {
    case GCS_HELPTEXTA:
		{		
        hr = StringCchCopyNA(pszName, 
            lstrlenA(pszName) / sizeof(pszName[0]), 
			CW2A(menuItemsHelp[(UINT)idCommand]),
            uMaxNameLen);
        break;
		}
    case GCS_HELPTEXTW:
        hr = StringCchCopyNW((LPWSTR)pszName, 
            lstrlenW((LPWSTR)pszName) / sizeof(pszName[0]), 
            menuItemsHelp[(UINT)idCommand],
            uMaxNameLen);
        break;

    case GCS_VERBA:
        hr = StringCchCopyNA(pszName, 
            lstrlenA(pszName) / sizeof(pszName[0]), 
            CW2A(menuItemsVerb[(UINT)idCommand]), uMaxNameLen);
        break;

    case GCS_VERBW:
        hr = StringCchCopyNW((LPWSTR)pszName, 
            lstrlenW((LPWSTR)pszName) / sizeof(pszName[0]), 
            menuItemsVerb[(UINT)idCommand], uMaxNameLen);
        break;

    default:
        hr = S_OK;
    }    

    // If the command (idCommand) is not supported by this context menu 
    // extension handler, return E_INVALIDARG.

    return hr;
}
#else

IFACEMETHODIMP CRiptideContextMenuImpl::GetCommandString(
    UINT idCommand, UINT uFlags, LPUINT lpReserved, LPSTR pszName, 
    UINT uMaxNameLen)
{
    HRESULT hr = E_INVALIDARG;
	USES_CONVERSION;

    switch (uFlags)
    {
    case GCS_HELPTEXTA:
        hr = StringCchCopyNA(pszName, 
            lstrlenA(pszName) / sizeof(pszName[0]), 
            CW2A(menuItemsHelp[(UINT)idCommand]),
            uMaxNameLen);
        break;

    case GCS_HELPTEXTW:
        hr = StringCchCopyNW((LPWSTR)pszName, 
            lstrlenW((LPWSTR)pszName) / sizeof(pszName[0]), 
            menuItemsHelp[(UINT)idCommand],
            uMaxNameLen);
        break;

    case GCS_VERBA:
        hr = StringCchCopyNA(pszName, 
            lstrlenA(pszName) / sizeof(pszName[0]), 
            CW2A(menuItemsVerb[(UINT)idCommand]), uMaxNameLen);
        break;

    case GCS_VERBW:
        hr = StringCchCopyNW((LPWSTR)pszName, 
            lstrlenW((LPWSTR)pszName) / sizeof(pszName[0]), 
            menuItemsVerb[(UINT)idCommand], uMaxNameLen);
        break;

    default:
        hr = S_OK;
    }    

    // If the command (idCommand) is not supported by this context menu 
    // extension handler, return E_INVALIDARG.

    return hr;
}

#endif

/**************************************************************************************\
*   FUNCTION: CRiptideContextMenuImpl::InvokeCommand(LPCMINVOKECOMMANDINFO)
*
*   PURPOSE: This method is called when a user clicks a menu item to tell 
*            the handler to run the associated command. The lpcmi parameter 
*            points to a structure that contains the needed information.
**************************************************************************************/
IFACEMETHODIMP CRiptideContextMenuImpl::InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi)
{
    BOOL fEx = FALSE;
    BOOL fUnicode = FALSE;

    // Determines which structure is being passed in, CMINVOKECOMMANDINFO or 
    // CMINVOKECOMMANDINFOEX based on the cbSize member of lpcmi. Although 
    // the lpcmi parameter is declared in Shlobj.h as a CMINVOKECOMMANDINFO 
    // structure, in practice it often points to a CMINVOKECOMMANDINFOEX 
    // structure. This struct is an extended version of CMINVOKECOMMANDINFO 
    // and has additional members that allow Unicode strings to be passed.
    if (lpcmi->cbSize == sizeof(CMINVOKECOMMANDINFOEX))
    {
        fEx = TRUE;
        if((lpcmi->fMask & CMIC_MASK_UNICODE))
        {
            fUnicode = TRUE;
        }
    }

    // Determines whether the command is identified by its offset or verb.
    // There are two ways to identify commands:
    //   1) The command's verb string 
    //   2) The command's identifier offset
    // If the high-order word of lpcmi->lpVerb (for the ANSI case) or 
    // lpcmi->lpVerbW (for the Unicode case) is nonzero, lpVerb or lpVerbW 
    // holds a verb string. If the high-order word is zero, the command 
    // offset is in the low-order word of lpcmi->lpVerb.

    // For the ANSI case, if the high-order word is not zero, the command's 
    // verb string is in lpcmi->lpVerb. 
    if (!fUnicode && HIWORD(lpcmi->lpVerb))
    {
		if (g_idCmdCount > menuItemsVerb.GetSize()) return E_FAIL;

        // Is the verb supported by this context menu extension?
		for (int i = 0; i < g_idCmdCount; i++)
		{
			if (StrCmpIA(lpcmi->lpVerb, CW2A(menuItemsVerb[i])) == 0)
			{
				OnAction(lpcmi->hwnd, i);
				return S_OK;
			}
		}

        // If the verb is not recognized by the context menu handler, it 
        // must return E_FAIL to allow it to be passed on to the other 
        // context menu handlers that might implement that verb.
        return E_FAIL;
    }

    // For the Unicode case, if the high-order word is not zero, the 
    // command's verb string is in lpcmi->lpVerbW. 
    else if(fUnicode && HIWORD(((CMINVOKECOMMANDINFOEX*)lpcmi)->lpVerbW))
    {
        // Is the verb supported by this context menu extension?
		if (g_idCmdCount > menuItemsVerb.GetSize()) return E_FAIL;

        // Is the verb supported by this context menu extension?
		for (int i = 0; i < g_idCmdCount; i++)
		{
			if (StrCmpIW(((CMINVOKECOMMANDINFOEX*)lpcmi)->lpVerbW, menuItemsVerb[i]) == 0)
			{
				OnAction(lpcmi->hwnd, i);
				return S_OK;
			}
		}
       
        // If the verb is not recognized by the context menu handler, it 
        // must return E_FAIL to allow it to be passed on to the other 
        // context menu handlers that might implement that verb.
        return E_FAIL;
    }

    // If the command cannot be identified through the verb string, then 
    // check the identifier offset.
    else
    {
        // Is the command identifier offset supported by this context menu 
        // extension?
        if (LOWORD(lpcmi->lpVerb) < g_idCmdCount)
        {
            OnAction(lpcmi->hwnd, LOWORD(lpcmi->lpVerb));
        }
        else
        {
            // If the verb is not recognized by the context menu handler, it 
            // must return E_FAIL to allow it to be passed on to the other 
            // context menu handlers that might implement that verb.
            return E_FAIL;
        }
    }

    return S_OK;
}
