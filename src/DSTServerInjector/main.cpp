#include <windows.h>
#include <TCHAR.h>

typedef DWORD (WINAPI* PFNGetIpAddrTable)(PVOID pIpAddrTable, PULONG pdwSize, BOOL bOrder);
static PFNGetIpAddrTable pfnGetIpAddrTable = NULL;

typedef DWORD (WINAPI* PFNGetBestRoute)(DWORD dwDestAddr, DWORD dwSourceAddr, PVOID pBestRoute);
static PFNGetBestRoute pfnGetBestRoute = NULL;

typedef DWORD (WINAPI* PFNGetAdaptersInfo)(PVOID pAdapterInfo, PULONG pOutBufLen);
static PFNGetAdaptersInfo pfnGetAdaptersInfo = NULL;

DWORD WINAPI GetIpAddrTable(PVOID pIpAddrTable, PULONG pdwSize, BOOL bOrder) {
	return pfnGetIpAddrTable(pIpAddrTable, pdwSize, bOrder);
}

DWORD WINAPI GetBestRoute(DWORD dwDestAddr, DWORD dwSourceAddr, PVOID pBestRoute) {
	return pfnGetBestRoute(dwDestAddr, dwSourceAddr, pBestRoute);
}

DWORD WINAPI GetAdaptersInfo(PVOID pAdapterInfo, PULONG pOutBufLen) {
	return pfnGetAdaptersInfo(pAdapterInfo, pOutBufLen);
}

void Init() {
	TCHAR sysPath[MAX_PATH * 2];
	::GetSystemDirectory(sysPath, MAX_PATH);
	_tcscat(sysPath, _T("\\IPHLPAPI.DLL"));

	HMODULE hOrg = ::LoadLibrary(sysPath);
	if (hOrg != NULL) {
		pfnGetIpAddrTable = (PFNGetIpAddrTable)::GetProcAddress(hOrg, "GetIpAddrTable");
		pfnGetBestRoute = (PFNGetBestRoute)::GetProcAddress(hOrg, "GetBestRoute");
		pfnGetAdaptersInfo = (PFNGetAdaptersInfo)::GetProcAddress(hOrg, "GetAdaptersInfo");
	}

	::LoadLibrary(_T("DINPUT8.DLL"));
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD ul_reason_for_call, LPVOID reserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		Init();
		break;
	}

	return TRUE;
}