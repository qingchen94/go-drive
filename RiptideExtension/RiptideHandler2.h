// RiptideHandler2.h : Declaration of the CRiptideHandler2

#pragma once
#include "resource.h"       // main symbols

#include "RiptideExtension_i.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CRiptideHandler2

class ATL_NO_VTABLE CRiptideHandler2 :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CRiptideHandler2, &CLSID_RiptideHandler2>,
	public IShellIconOverlayIdentifier
{
public:
	CRiptideHandler2()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_RIPTIDEHANDLER2)

DECLARE_NOT_AGGREGATABLE(CRiptideHandler2)

BEGIN_COM_MAP(CRiptideHandler2)
	COM_INTERFACE_ENTRY(IShellIconOverlayIdentifier)
END_COM_MAP()


	IFACEMETHODIMP GetOverlayInfo(LPWSTR pwszIconFile,int cchMax,int *pIndex,DWORD* pdwFlags);
	IFACEMETHODIMP GetPriority(int* pPriority);
	IFACEMETHODIMP IsMemberOf(LPCWSTR pwszPath,DWORD dwAttrib);

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
};

OBJECT_ENTRY_AUTO(__uuidof(RiptideHandler2), CRiptideHandler2)
