#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <TCHAR.h>


/*
timeBeginPeriod
timeEndPeriod
mixerClose
mixerGetControlDetailsA
mixerGetDevCapsA
mixerGetLineControlsA
mixerGetLineInfoA
mixerOpen
mixerSetControlDetails
waveInClose
waveInOpen
waveOutClose
waveOutGetDevCapsA
waveOutMessage
waveOutOpen
*/

#define ENTRY(name) \
static void* pfn##name = NULL; \
__declspec(naked)void name() { \
	_asm jmp pfn##name \
}

#define INIT(name) \
pfn##name = ::GetProcAddress(hOrg, #name);

ENTRY(mciExecute)
ENTRY(CloseDriver)
ENTRY(DefDriverProc)
ENTRY(DriverCallback)
ENTRY(DrvGetModuleHandle)
ENTRY(GetDriverModuleHandle)
ENTRY(NotifyCallbackData)
ENTRY(OpenDriver)
ENTRY(PlaySound)
ENTRY(PlaySoundA)
ENTRY(PlaySoundW)
ENTRY(SendDriverMessage)
ENTRY(WOW32DriverCallback)
ENTRY(WOW32ResolveMultiMediaHandle)
ENTRY(WOWAppExit)
ENTRY(aux32Message)
ENTRY(auxGetDevCapsA)
ENTRY(auxGetDevCapsW)
ENTRY(auxGetNumDevs)
ENTRY(auxGetVolume)
ENTRY(auxOutMessage)
ENTRY(auxSetVolume)
ENTRY(joy32Message)
ENTRY(joyConfigChanged)
ENTRY(joyGetDevCapsA)
ENTRY(joyGetDevCapsW)
ENTRY(joyGetNumDevs)
ENTRY(joyGetPos)
ENTRY(joyGetPosEx)
ENTRY(joyGetThreshold)
ENTRY(joyReleaseCapture)
ENTRY(joySetCapture)
ENTRY(joySetThreshold)
ENTRY(mci32Message)
ENTRY(mciDriverNotify)
ENTRY(mciDriverYield)
ENTRY(mciFreeCommandResource)
ENTRY(mciGetCreatorTask)
ENTRY(mciGetDeviceIDA)
ENTRY(mciGetDeviceIDFromElementIDA)
ENTRY(mciGetDeviceIDFromElementIDW)
ENTRY(mciGetDeviceIDW)
ENTRY(mciGetDriverData)
ENTRY(mciGetErrorStringA)
ENTRY(mciGetErrorStringW)
ENTRY(mciGetYieldProc)
ENTRY(mciLoadCommandResource)
ENTRY(mciSendCommandA)
ENTRY(mciSendCommandW)
ENTRY(mciSendStringA)
ENTRY(mciSendStringW)
ENTRY(mciSetDriverData)
ENTRY(mciSetYieldProc)
ENTRY(mid32Message)
ENTRY(midiConnect)
ENTRY(midiDisconnect)
ENTRY(midiInAddBuffer)
ENTRY(midiInClose)
ENTRY(midiInGetDevCapsA)
ENTRY(midiInGetDevCapsW)
ENTRY(midiInGetErrorTextA)
ENTRY(midiInGetErrorTextW)
ENTRY(midiInGetID)
ENTRY(midiInGetNumDevs)
ENTRY(midiInMessage)
ENTRY(midiInOpen)
ENTRY(midiInPrepareHeader)
ENTRY(midiInReset)
ENTRY(midiInStart)
ENTRY(midiInStop)
ENTRY(midiInUnprepareHeader)
ENTRY(midiOutCacheDrumPatches)
ENTRY(midiOutCachePatches)
ENTRY(midiOutClose)
ENTRY(midiOutGetDevCapsA)
ENTRY(midiOutGetDevCapsW)
ENTRY(midiOutGetErrorTextA)
ENTRY(midiOutGetErrorTextW)
ENTRY(midiOutGetID)
ENTRY(midiOutGetNumDevs)
ENTRY(midiOutGetVolume)
ENTRY(midiOutLongMsg)
ENTRY(midiOutMessage)
ENTRY(midiOutOpen)
ENTRY(midiOutPrepareHeader)
ENTRY(midiOutReset)
ENTRY(midiOutSetVolume)
ENTRY(midiOutShortMsg)
ENTRY(midiOutUnprepareHeader)
ENTRY(midiStreamClose)
ENTRY(midiStreamOpen)
ENTRY(midiStreamOut)
ENTRY(midiStreamPause)
ENTRY(midiStreamPosition)
ENTRY(midiStreamProperty)
ENTRY(midiStreamRestart)
ENTRY(midiStreamStop)
ENTRY(mixerClose)
ENTRY(mixerGetControlDetailsA)
ENTRY(mixerGetControlDetailsW)
ENTRY(mixerGetDevCapsA)
ENTRY(mixerGetDevCapsW)
ENTRY(mixerGetID)
ENTRY(mixerGetLineControlsA)
ENTRY(mixerGetLineControlsW)
ENTRY(mixerGetLineInfoA)
ENTRY(mixerGetLineInfoW)
ENTRY(mixerGetNumDevs)
ENTRY(mixerMessage)
ENTRY(mixerOpen)
ENTRY(mixerSetControlDetails)
ENTRY(mmDrvInstall)
ENTRY(mmGetCurrentTask)
ENTRY(mmTaskBlock)
ENTRY(mmTaskCreate)
ENTRY(mmTaskSignal)
ENTRY(mmTaskYield)
ENTRY(mmioAdvance)
ENTRY(mmioAscend)
ENTRY(mmioClose)
ENTRY(mmioCreateChunk)
ENTRY(mmioDescend)
ENTRY(mmioFlush)
ENTRY(mmioGetInfo)
ENTRY(mmioInstallIOProcA)
ENTRY(mmioInstallIOProcW)
ENTRY(mmioOpenA)
ENTRY(mmioOpenW)
ENTRY(mmioRead)
ENTRY(mmioRenameA)
ENTRY(mmioRenameW)
ENTRY(mmioSeek)
ENTRY(mmioSendMessage)
ENTRY(mmioSetBuffer)
ENTRY(mmioSetInfo)
ENTRY(mmioStringToFOURCCA)
ENTRY(mmioStringToFOURCCW)
ENTRY(mmioWrite)
ENTRY(mmsystemGetVersion)
ENTRY(mod32Message)
ENTRY(mxd32Message)
ENTRY(sndPlaySoundA)
ENTRY(sndPlaySoundW)
ENTRY(tid32Message)
ENTRY(timeBeginPeriod)
ENTRY(timeEndPeriod)
ENTRY(timeGetDevCaps)
ENTRY(timeGetSystemTime)
ENTRY(timeGetTime)
ENTRY(timeKillEvent)
ENTRY(timeSetEvent)
ENTRY(waveInAddBuffer)
ENTRY(waveInClose)
ENTRY(waveInGetDevCapsA)
ENTRY(waveInGetDevCapsW)
ENTRY(waveInGetErrorTextA)
ENTRY(waveInGetErrorTextW)
ENTRY(waveInGetID)
ENTRY(waveInGetNumDevs)
ENTRY(waveInGetPosition)
ENTRY(waveInMessage)
ENTRY(waveInOpen)
ENTRY(waveInPrepareHeader)
ENTRY(waveInReset)
ENTRY(waveInStart)
ENTRY(waveInStop)
ENTRY(waveInUnprepareHeader)
ENTRY(waveOutBreakLoop)
ENTRY(waveOutClose)
ENTRY(waveOutGetDevCapsA)
ENTRY(waveOutGetDevCapsW)
ENTRY(waveOutGetErrorTextA)
ENTRY(waveOutGetErrorTextW)
ENTRY(waveOutGetID)
ENTRY(waveOutGetNumDevs)
ENTRY(waveOutGetPitch)
ENTRY(waveOutGetPlaybackRate)
ENTRY(waveOutGetPosition)
ENTRY(waveOutGetVolume)
ENTRY(waveOutMessage)
ENTRY(waveOutOpen)
ENTRY(waveOutPause)
ENTRY(waveOutPrepareHeader)
ENTRY(waveOutReset)
ENTRY(waveOutRestart)
ENTRY(waveOutSetPitch)
ENTRY(waveOutSetPlaybackRate)
ENTRY(waveOutSetVolume)
ENTRY(waveOutUnprepareHeader)
ENTRY(waveOutWrite)
ENTRY(wid32Message)
ENTRY(wod32Message)



/*
typedef ULONG (WINAPI* PFNGetAdaptersAddresses)(ULONG Family, ULONG Flags, PVOID Reserved, PVOID AdapterAddresses, PULONG SizePointer);
static PFNGetAdaptersAddresses pfnGetAdaptersAddresses = NULL;

typedef DWORD (WINAPI* PFNNotifyAddrChange)(PHANDLE Handle, LPOVERLAPPED overlapped);
static PFNNotifyAddrChange pfnNotifyAddrChange = NULL;

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

ULONG WINAPI GetAdaptersAddresses(ULONG Family, ULONG Flags, PVOID Reserved, PVOID AdapterAddresses, PULONG SizePointer) {
	return pfnGetAdaptersAddresses(Family, Flags, Reserved, AdapterAddresses, SizePointer);
}

DWORD WINAPI NotifyAddrChange(PHANDLE Handle, LPOVERLAPPED overlapped) {
	return pfnNotifyAddrChange(Handle, overlapped);
}
*/

void Init() {
	/*
	TCHAR sysPath[MAX_PATH * 2];
	::GetSystemDirectory(sysPath, MAX_PATH);
	_tcscat(sysPath, _T("\\IPHLPAPI.DLL"));

	HMODULE hOrg = ::LoadLibrary(sysPath);
	if (hOrg != NULL) {
		pfnGetIpAddrTable = (PFNGetIpAddrTable)::GetProcAddress(hOrg, "GetIpAddrTable");
		pfnGetBestRoute = (PFNGetBestRoute)::GetProcAddress(hOrg, "GetBestRoute");
		pfnGetAdaptersInfo = (PFNGetAdaptersInfo)::GetProcAddress(hOrg, "GetAdaptersInfo");
		pfnGetAdaptersAddresses = (PFNGetAdaptersAddresses)::GetProcAddress(hOrg, "GetAdaptersAddresses");
		pfnNotifyAddrChange = (PFNNotifyAddrChange)::GetProcAddress(hOrg, "NotifyAddrChange");
	}*/

	TCHAR sysPath[MAX_PATH * 2];
	::GetSystemDirectory(sysPath, MAX_PATH);
	_tcscat(sysPath, _T("\\WINMM.DLL"));

	HMODULE hOrg = ::LoadLibrary(sysPath);
	if (hOrg != NULL) {
INIT(mciExecute)
INIT(CloseDriver)
INIT(DefDriverProc)
INIT(DriverCallback)
INIT(DrvGetModuleHandle)
INIT(GetDriverModuleHandle)
INIT(NotifyCallbackData)
INIT(OpenDriver)
INIT(PlaySound)
INIT(PlaySoundA)
INIT(PlaySoundW)
INIT(SendDriverMessage)
INIT(WOW32DriverCallback)
INIT(WOW32ResolveMultiMediaHandle)
INIT(WOWAppExit)
INIT(aux32Message)
INIT(auxGetDevCapsA)
INIT(auxGetDevCapsW)
INIT(auxGetNumDevs)
INIT(auxGetVolume)
INIT(auxOutMessage)
INIT(auxSetVolume)
INIT(joy32Message)
INIT(joyConfigChanged)
INIT(joyGetDevCapsA)
INIT(joyGetDevCapsW)
INIT(joyGetNumDevs)
INIT(joyGetPos)
INIT(joyGetPosEx)
INIT(joyGetThreshold)
INIT(joyReleaseCapture)
INIT(joySetCapture)
INIT(joySetThreshold)
INIT(mci32Message)
INIT(mciDriverNotify)
INIT(mciDriverYield)
INIT(mciFreeCommandResource)
INIT(mciGetCreatorTask)
INIT(mciGetDeviceIDA)
INIT(mciGetDeviceIDFromElementIDA)
INIT(mciGetDeviceIDFromElementIDW)
INIT(mciGetDeviceIDW)
INIT(mciGetDriverData)
INIT(mciGetErrorStringA)
INIT(mciGetErrorStringW)
INIT(mciGetYieldProc)
INIT(mciLoadCommandResource)
INIT(mciSendCommandA)
INIT(mciSendCommandW)
INIT(mciSendStringA)
INIT(mciSendStringW)
INIT(mciSetDriverData)
INIT(mciSetYieldProc)
INIT(mid32Message)
INIT(midiConnect)
INIT(midiDisconnect)
INIT(midiInAddBuffer)
INIT(midiInClose)
INIT(midiInGetDevCapsA)
INIT(midiInGetDevCapsW)
INIT(midiInGetErrorTextA)
INIT(midiInGetErrorTextW)
INIT(midiInGetID)
INIT(midiInGetNumDevs)
INIT(midiInMessage)
INIT(midiInOpen)
INIT(midiInPrepareHeader)
INIT(midiInReset)
INIT(midiInStart)
INIT(midiInStop)
INIT(midiInUnprepareHeader)
INIT(midiOutCacheDrumPatches)
INIT(midiOutCachePatches)
INIT(midiOutClose)
INIT(midiOutGetDevCapsA)
INIT(midiOutGetDevCapsW)
INIT(midiOutGetErrorTextA)
INIT(midiOutGetErrorTextW)
INIT(midiOutGetID)
INIT(midiOutGetNumDevs)
INIT(midiOutGetVolume)
INIT(midiOutLongMsg)
INIT(midiOutMessage)
INIT(midiOutOpen)
INIT(midiOutPrepareHeader)
INIT(midiOutReset)
INIT(midiOutSetVolume)
INIT(midiOutShortMsg)
INIT(midiOutUnprepareHeader)
INIT(midiStreamClose)
INIT(midiStreamOpen)
INIT(midiStreamOut)
INIT(midiStreamPause)
INIT(midiStreamPosition)
INIT(midiStreamProperty)
INIT(midiStreamRestart)
INIT(midiStreamStop)
INIT(mixerClose)
INIT(mixerGetControlDetailsA)
INIT(mixerGetControlDetailsW)
INIT(mixerGetDevCapsA)
INIT(mixerGetDevCapsW)
INIT(mixerGetID)
INIT(mixerGetLineControlsA)
INIT(mixerGetLineControlsW)
INIT(mixerGetLineInfoA)
INIT(mixerGetLineInfoW)
INIT(mixerGetNumDevs)
INIT(mixerMessage)
INIT(mixerOpen)
INIT(mixerSetControlDetails)
INIT(mmDrvInstall)
INIT(mmGetCurrentTask)
INIT(mmTaskBlock)
INIT(mmTaskCreate)
INIT(mmTaskSignal)
INIT(mmTaskYield)
INIT(mmioAdvance)
INIT(mmioAscend)
INIT(mmioClose)
INIT(mmioCreateChunk)
INIT(mmioDescend)
INIT(mmioFlush)
INIT(mmioGetInfo)
INIT(mmioInstallIOProcA)
INIT(mmioInstallIOProcW)
INIT(mmioOpenA)
INIT(mmioOpenW)
INIT(mmioRead)
INIT(mmioRenameA)
INIT(mmioRenameW)
INIT(mmioSeek)
INIT(mmioSendMessage)
INIT(mmioSetBuffer)
INIT(mmioSetInfo)
INIT(mmioStringToFOURCCA)
INIT(mmioStringToFOURCCW)
INIT(mmioWrite)
INIT(mmsystemGetVersion)
INIT(mod32Message)
INIT(mxd32Message)
INIT(sndPlaySoundA)
INIT(sndPlaySoundW)
INIT(tid32Message)
INIT(timeBeginPeriod)
INIT(timeEndPeriod)
INIT(timeGetDevCaps)
INIT(timeGetSystemTime)
INIT(timeGetTime)
INIT(timeKillEvent)
INIT(timeSetEvent)
INIT(waveInAddBuffer)
INIT(waveInClose)
INIT(waveInGetDevCapsA)
INIT(waveInGetDevCapsW)
INIT(waveInGetErrorTextA)
INIT(waveInGetErrorTextW)
INIT(waveInGetID)
INIT(waveInGetNumDevs)
INIT(waveInGetPosition)
INIT(waveInMessage)
INIT(waveInOpen)
INIT(waveInPrepareHeader)
INIT(waveInReset)
INIT(waveInStart)
INIT(waveInStop)
INIT(waveInUnprepareHeader)
INIT(waveOutBreakLoop)
INIT(waveOutClose)
INIT(waveOutGetDevCapsA)
INIT(waveOutGetDevCapsW)
INIT(waveOutGetErrorTextA)
INIT(waveOutGetErrorTextW)
INIT(waveOutGetID)
INIT(waveOutGetNumDevs)
INIT(waveOutGetPitch)
INIT(waveOutGetPlaybackRate)
INIT(waveOutGetPosition)
INIT(waveOutGetVolume)
INIT(waveOutMessage)
INIT(waveOutOpen)
INIT(waveOutPause)
INIT(waveOutPrepareHeader)
INIT(waveOutReset)
INIT(waveOutRestart)
INIT(waveOutSetPitch)
INIT(waveOutSetPlaybackRate)
INIT(waveOutSetVolume)
INIT(waveOutUnprepareHeader)
INIT(waveOutWrite)
INIT(wid32Message)
INIT(wod32Message)


	}

	::LoadLibrary(_T("DINPUT8.DLL"));
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD ul_reason_for_call, LPVOID reserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		Init();
		// system("pause");
		break;
	}

	return TRUE;
}