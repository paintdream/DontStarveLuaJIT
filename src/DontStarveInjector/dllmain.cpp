// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "DontStarveInjector_i.h"
#include "dllmain.h"
#include "compreg.h"
#include "dlldatax.h"

CDontStarveInjectorModule _AtlModule;

class CDontStarveInjectorApp : public CWinApp
{
public:

// Overrides
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CDontStarveInjectorApp, CWinApp)
END_MESSAGE_MAP()

CDontStarveInjectorApp theApp;

BOOL CDontStarveInjectorApp::InitInstance()
{
#ifdef _MERGE_PROXYSTUB
	if (!PrxDllMain(m_hInstance, DLL_PROCESS_ATTACH, NULL))
		return FALSE;
#endif
	return CWinApp::InitInstance();
}

int CDontStarveInjectorApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
