// dllmain.h : Declaration of module class.

class CDontStarveInjectorModule : public CAtlDllModuleT< CDontStarveInjectorModule >
{
public :
	DECLARE_LIBID(LIBID_DontStarveInjectorLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_DONTSTARVEINJECTOR, "{98CFFE86-F368-44B0-A93E-8A675362F0C2}")
};

extern class CDontStarveInjectorModule _AtlModule;
