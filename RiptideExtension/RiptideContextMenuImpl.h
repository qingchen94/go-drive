// RiptideContextMenuImpl.h : Declaration of the CRiptideContextMenuImpl

#pragma once
#include <atlstr.h>
#include <atlcoll.h>
#include "resource.h"
#include "RiptideExtension_i.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CRiptideContextMenuImpl
extern HINSTANCE GetResInstance();

class ATL_NO_VTABLE CRiptideContextMenuImpl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CRiptideContextMenuImpl, &CLSID_RiptideContextMenuImpl>,
	public IShellExtInit, 
	public IContextMenu
{
public:
	CRiptideContextMenuImpl()
	{	
		CComBSTR bstr;

		bstr.LoadString(GetResInstance(), IDS_PREVIEW);
		menuItems.Add(bstr);
		bstr.LoadString(GetResInstance(), IDS_DOWNLOAD);
		menuItems.Add(bstr);
		bstr.LoadString(GetResInstance(), IDS_REMOVE);
		menuItems.Add(bstr);
		bstr.LoadString(GetResInstance(), IDS_MOVETO_SHARE);
		menuItems.Add(bstr);

		bstr.LoadString(GetResInstance(), IDS_BROWSE_ITEM_ON_WEB_TIP);
		menuItemsHelp.Add(bstr);
		bstr.LoadString(GetResInstance(), IDS_GET_SECURE_LINK_TIP);
		menuItemsHelp.Add(bstr);		
		bstr.LoadString(GetResInstance(), IDS_GET_PUBLIC_LINK_TIP);
		menuItemsHelp.Add(bstr);
		bstr.LoadString(GetResInstance(), IDS_MOVETO_SHARE_TIP);
		menuItemsHelp.Add(bstr);


		menuItemsVerb.Add((CComBSTR) L"Preivew");
		menuItemsVerb.Add((CComBSTR) L"Download");
		menuItemsVerb.Add((CComBSTR) L"Remove");
		menuItemsVerb.Add((CComBSTR) L"MoveTo");


		menuItemsData.Add((CComBSTR) L"-p");
		menuItemsData.Add((CComBSTR) L"-d");
		menuItemsData.Add((CComBSTR) L"-r");
		menuItemsData.Add((CComBSTR) L"-moveto");
	}

DECLARE_REGISTRY_RESOURCEID(IDR_RIPTIDECONTEXTMENUIMPL)

DECLARE_NOT_AGGREGATABLE(CRiptideContextMenuImpl)

BEGIN_COM_MAP(CRiptideContextMenuImpl)
	COM_INTERFACE_ENTRY(IShellExtInit)
	COM_INTERFACE_ENTRY(IContextMenu)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{		
		hBitmapDownload = hBitmapRemove = hBitmapPreview = NULL;
		return S_OK;
	}

	void FinalRelease()
	{
		if (hBitmapRemove)
		{
			DeleteObject(hBitmapRemove);
			hBitmapRemove = NULL;
		}

		if (hBitmapDownload){
			 DeleteObject(hBitmapDownload);
			 hBitmapDownload = NULL;
		}

		if (hBitmapPreview){
			 DeleteObject(hBitmapPreview);
			 hBitmapPreview = NULL;
		}
	}

public:
    // IShellExtInit
    IFACEMETHODIMP Initialize(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

    // IContextMenu
#if defined (_M_X64)
    IFACEMETHODIMP GetCommandString(UINT_PTR, UINT, UINT*, LPSTR, UINT);
#else
	IFACEMETHODIMP GetCommandString(UINT, UINT, UINT*, LPSTR, UINT);
#endif
    IFACEMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO);
    IFACEMETHODIMP QueryContextMenu(HMENU, UINT, UINT, UINT, UINT);

protected:
    // The name of the first selected file
    TCHAR m_szFileName[MAX_PATH*4];
	_bstr_t m_ID;

    // The function that handles the "Sample" verb
    void OnAction(HWND hWnd, UINT index);

	bool IsLocal(const CString& path);
private:
	bool IsGoDriveItem(CString path);
	_bstr_t WriteFileNames(CAtlArray<CString>& files);
	bool VerifySyncFolderExist(void);
	CString GetSyncFolderPath(void);

private:
	CSimpleArray<CComBSTR> menuItems;
	CSimpleArray<CComBSTR> menuItemsHelp;
	CSimpleArray<CComBSTR> menuItemsVerb;
	CSimpleArray<CComBSTR> menuItemsData;
	CAtlArray<CString>  m_selectedFiles;

	HANDLE hBitmapDownload;
	HANDLE hBitmapRemove;
	HANDLE hBitmapPreview;
};

OBJECT_ENTRY_AUTO(__uuidof(RiptideContextMenuImpl), CRiptideContextMenuImpl)
