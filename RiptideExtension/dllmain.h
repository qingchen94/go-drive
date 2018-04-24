// dllmain.h : Declaration of module class.

class CRiptideExtensionModule : public CAtlDllModuleT< CRiptideExtensionModule >
{
public :
	DECLARE_LIBID(LIBID_RiptideExtensionLib)
};

extern class CRiptideExtensionModule _AtlModule;
//extern class CRTStatusManager _rtManager;