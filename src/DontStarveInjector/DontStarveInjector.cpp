// DontStarveInjector.cpp : Defines the exported functions for the DLL application.
//

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "DontStarveInjector.h"
#include "xde.h"
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <cassert>
#include <TCHAR.h>
#include <ImageHlp.h>
#include <TlHelp32.h>
#include "APIHook.h"
#pragma comment(lib, "dbghelp.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

#pragma region Relay

#define PROXY_STUB(name) \
static void* pfn##name = NULL; \
__declspec(naked)void name() { \
	_asm jmp pfn##name \
}

#define PROXY_INIT(name) \
pfn##name = ::GetProcAddress(hOrg, #name);

PROXY_STUB(mciExecute)
PROXY_STUB(CloseDriver)
PROXY_STUB(DefDriverProc)
PROXY_STUB(DriverCallback)
PROXY_STUB(DrvGetModuleHandle)
PROXY_STUB(GetDriverModuleHandle)
PROXY_STUB(NotifyCallbackData)
PROXY_STUB(OpenDriver)
PROXY_STUB(PlaySound)
PROXY_STUB(PlaySoundA)
PROXY_STUB(PlaySoundW)
PROXY_STUB(SendDriverMessage)
PROXY_STUB(WOW32DriverCallback)
PROXY_STUB(WOW32ResolveMultiMediaHandle)
PROXY_STUB(WOWAppExit)
PROXY_STUB(aux32Message)
PROXY_STUB(auxGetDevCapsA)
PROXY_STUB(auxGetDevCapsW)
PROXY_STUB(auxGetNumDevs)
PROXY_STUB(auxGetVolume)
PROXY_STUB(auxOutMessage)
PROXY_STUB(auxSetVolume)
PROXY_STUB(joy32Message)
PROXY_STUB(joyConfigChanged)
PROXY_STUB(joyGetDevCapsA)
PROXY_STUB(joyGetDevCapsW)
PROXY_STUB(joyGetNumDevs)
PROXY_STUB(joyGetPos)
PROXY_STUB(joyGetPosEx)
PROXY_STUB(joyGetThreshold)
PROXY_STUB(joyReleaseCapture)
PROXY_STUB(joySetCapture)
PROXY_STUB(joySetThreshold)
PROXY_STUB(mci32Message)
PROXY_STUB(mciDriverNotify)
PROXY_STUB(mciDriverYield)
PROXY_STUB(mciFreeCommandResource)
PROXY_STUB(mciGetCreatorTask)
PROXY_STUB(mciGetDeviceIDA)
PROXY_STUB(mciGetDeviceIDFromElementIDA)
PROXY_STUB(mciGetDeviceIDFromElementIDW)
PROXY_STUB(mciGetDeviceIDW)
PROXY_STUB(mciGetDriverData)
PROXY_STUB(mciGetErrorStringA)
PROXY_STUB(mciGetErrorStringW)
PROXY_STUB(mciGetYieldProc)
PROXY_STUB(mciLoadCommandResource)
PROXY_STUB(mciSendCommandA)
PROXY_STUB(mciSendCommandW)
PROXY_STUB(mciSendStringA)
PROXY_STUB(mciSendStringW)
PROXY_STUB(mciSetDriverData)
PROXY_STUB(mciSetYieldProc)
PROXY_STUB(mid32Message)
PROXY_STUB(midiConnect)
PROXY_STUB(midiDisconnect)
PROXY_STUB(midiInAddBuffer)
PROXY_STUB(midiInClose)
PROXY_STUB(midiInGetDevCapsA)
PROXY_STUB(midiInGetDevCapsW)
PROXY_STUB(midiInGetErrorTextA)
PROXY_STUB(midiInGetErrorTextW)
PROXY_STUB(midiInGetID)
PROXY_STUB(midiInGetNumDevs)
PROXY_STUB(midiInMessage)
PROXY_STUB(midiInOpen)
PROXY_STUB(midiInPrepareHeader)
PROXY_STUB(midiInReset)
PROXY_STUB(midiInStart)
PROXY_STUB(midiInStop)
PROXY_STUB(midiInUnprepareHeader)
PROXY_STUB(midiOutCacheDrumPatches)
PROXY_STUB(midiOutCachePatches)
PROXY_STUB(midiOutClose)
PROXY_STUB(midiOutGetDevCapsA)
PROXY_STUB(midiOutGetDevCapsW)
PROXY_STUB(midiOutGetErrorTextA)
PROXY_STUB(midiOutGetErrorTextW)
PROXY_STUB(midiOutGetID)
PROXY_STUB(midiOutGetNumDevs)
PROXY_STUB(midiOutGetVolume)
PROXY_STUB(midiOutLongMsg)
PROXY_STUB(midiOutMessage)
PROXY_STUB(midiOutOpen)
PROXY_STUB(midiOutPrepareHeader)
PROXY_STUB(midiOutReset)
PROXY_STUB(midiOutSetVolume)
PROXY_STUB(midiOutShortMsg)
PROXY_STUB(midiOutUnprepareHeader)
PROXY_STUB(midiStreamClose)
PROXY_STUB(midiStreamOpen)
PROXY_STUB(midiStreamOut)
PROXY_STUB(midiStreamPause)
PROXY_STUB(midiStreamPosition)
PROXY_STUB(midiStreamProperty)
PROXY_STUB(midiStreamRestart)
PROXY_STUB(midiStreamStop)
PROXY_STUB(mixerClose)
PROXY_STUB(mixerGetControlDetailsA)
PROXY_STUB(mixerGetControlDetailsW)
PROXY_STUB(mixerGetDevCapsA)
PROXY_STUB(mixerGetDevCapsW)
PROXY_STUB(mixerGetID)
PROXY_STUB(mixerGetLineControlsA)
PROXY_STUB(mixerGetLineControlsW)
PROXY_STUB(mixerGetLineInfoA)
PROXY_STUB(mixerGetLineInfoW)
PROXY_STUB(mixerGetNumDevs)
PROXY_STUB(mixerMessage)
PROXY_STUB(mixerOpen)
PROXY_STUB(mixerSetControlDetails)
PROXY_STUB(mmDrvInstall)
PROXY_STUB(mmGetCurrentTask)
PROXY_STUB(mmTaskBlock)
PROXY_STUB(mmTaskCreate)
PROXY_STUB(mmTaskSignal)
PROXY_STUB(mmTaskYield)
PROXY_STUB(mmioAdvance)
PROXY_STUB(mmioAscend)
PROXY_STUB(mmioClose)
PROXY_STUB(mmioCreateChunk)
PROXY_STUB(mmioDescend)
PROXY_STUB(mmioFlush)
PROXY_STUB(mmioGetInfo)
PROXY_STUB(mmioInstallIOProcA)
PROXY_STUB(mmioInstallIOProcW)
PROXY_STUB(mmioOpenA)
PROXY_STUB(mmioOpenW)
PROXY_STUB(mmioRead)
PROXY_STUB(mmioRenameA)
PROXY_STUB(mmioRenameW)
PROXY_STUB(mmioSeek)
PROXY_STUB(mmioSendMessage)
PROXY_STUB(mmioSetBuffer)
PROXY_STUB(mmioSetInfo)
PROXY_STUB(mmioStringToFOURCCA)
PROXY_STUB(mmioStringToFOURCCW)
PROXY_STUB(mmioWrite)
PROXY_STUB(mmsystemGetVersion)
PROXY_STUB(mod32Message)
PROXY_STUB(mxd32Message)
PROXY_STUB(sndPlaySoundA)
PROXY_STUB(sndPlaySoundW)
PROXY_STUB(tid32Message)
PROXY_STUB(timeBeginPeriod)
PROXY_STUB(timeEndPeriod)
PROXY_STUB(timeGetDevCaps)
PROXY_STUB(timeGetSystemTime)
PROXY_STUB(timeGetTime)
PROXY_STUB(timeKillEvent)
PROXY_STUB(timeSetEvent)
PROXY_STUB(waveInAddBuffer)
PROXY_STUB(waveInClose)
PROXY_STUB(waveInGetDevCapsA)
PROXY_STUB(waveInGetDevCapsW)
PROXY_STUB(waveInGetErrorTextA)
PROXY_STUB(waveInGetErrorTextW)
PROXY_STUB(waveInGetID)
PROXY_STUB(waveInGetNumDevs)
PROXY_STUB(waveInGetPosition)
PROXY_STUB(waveInMessage)
PROXY_STUB(waveInOpen)
PROXY_STUB(waveInPrepareHeader)
PROXY_STUB(waveInReset)
PROXY_STUB(waveInStart)
PROXY_STUB(waveInStop)
PROXY_STUB(waveInUnprepareHeader)
PROXY_STUB(waveOutBreakLoop)
PROXY_STUB(waveOutClose)
PROXY_STUB(waveOutGetDevCapsA)
PROXY_STUB(waveOutGetDevCapsW)
PROXY_STUB(waveOutGetErrorTextA)
PROXY_STUB(waveOutGetErrorTextW)
PROXY_STUB(waveOutGetID)
PROXY_STUB(waveOutGetNumDevs)
PROXY_STUB(waveOutGetPitch)
PROXY_STUB(waveOutGetPlaybackRate)
PROXY_STUB(waveOutGetPosition)
PROXY_STUB(waveOutGetVolume)
PROXY_STUB(waveOutMessage)
PROXY_STUB(waveOutOpen)
PROXY_STUB(waveOutPause)
PROXY_STUB(waveOutPrepareHeader)
PROXY_STUB(waveOutReset)
PROXY_STUB(waveOutRestart)
PROXY_STUB(waveOutSetPitch)
PROXY_STUB(waveOutSetPlaybackRate)
PROXY_STUB(waveOutSetVolume)
PROXY_STUB(waveOutUnprepareHeader)
PROXY_STUB(waveOutWrite)
PROXY_STUB(wid32Message)
PROXY_STUB(wod32Message)

static void InitializeRelay() {
	TCHAR sysPath[MAX_PATH * 2];
	::GetSystemDirectory(sysPath, MAX_PATH);
	_tcscat(sysPath, _T("\\WINMM.DLL"));

	HMODULE hOrg = ::LoadLibrary(sysPath);
	if (hOrg != NULL) {
		PROXY_INIT(mciExecute);
		PROXY_INIT(CloseDriver);
		PROXY_INIT(DefDriverProc);
		PROXY_INIT(DriverCallback);
		PROXY_INIT(DrvGetModuleHandle);
		PROXY_INIT(GetDriverModuleHandle);
		PROXY_INIT(NotifyCallbackData);
		PROXY_INIT(OpenDriver);
		PROXY_INIT(PlaySound);
		PROXY_INIT(PlaySoundA);
		PROXY_INIT(PlaySoundW);
		PROXY_INIT(SendDriverMessage);
		PROXY_INIT(WOW32DriverCallback);
		PROXY_INIT(WOW32ResolveMultiMediaHandle);
		PROXY_INIT(WOWAppExit);
		PROXY_INIT(aux32Message);
		PROXY_INIT(auxGetDevCapsA);
		PROXY_INIT(auxGetDevCapsW);
		PROXY_INIT(auxGetNumDevs);
		PROXY_INIT(auxGetVolume);
		PROXY_INIT(auxOutMessage);
		PROXY_INIT(auxSetVolume);
		PROXY_INIT(joy32Message);
		PROXY_INIT(joyConfigChanged);
		PROXY_INIT(joyGetDevCapsA);
		PROXY_INIT(joyGetDevCapsW);
		PROXY_INIT(joyGetNumDevs);
		PROXY_INIT(joyGetPos);
		PROXY_INIT(joyGetPosEx);
		PROXY_INIT(joyGetThreshold);
		PROXY_INIT(joyReleaseCapture);
		PROXY_INIT(joySetCapture);
		PROXY_INIT(joySetThreshold);
		PROXY_INIT(mci32Message);
		PROXY_INIT(mciDriverNotify);
		PROXY_INIT(mciDriverYield);
		PROXY_INIT(mciFreeCommandResource);
		PROXY_INIT(mciGetCreatorTask);
		PROXY_INIT(mciGetDeviceIDA);
		PROXY_INIT(mciGetDeviceIDFromElementIDA);
		PROXY_INIT(mciGetDeviceIDFromElementIDW);
		PROXY_INIT(mciGetDeviceIDW);
		PROXY_INIT(mciGetDriverData);
		PROXY_INIT(mciGetErrorStringA);
		PROXY_INIT(mciGetErrorStringW);
		PROXY_INIT(mciGetYieldProc);
		PROXY_INIT(mciLoadCommandResource);
		PROXY_INIT(mciSendCommandA);
		PROXY_INIT(mciSendCommandW);
		PROXY_INIT(mciSendStringA);
		PROXY_INIT(mciSendStringW);
		PROXY_INIT(mciSetDriverData);
		PROXY_INIT(mciSetYieldProc);
		PROXY_INIT(mid32Message);
		PROXY_INIT(midiConnect);
		PROXY_INIT(midiDisconnect);
		PROXY_INIT(midiInAddBuffer);
		PROXY_INIT(midiInClose);
		PROXY_INIT(midiInGetDevCapsA);
		PROXY_INIT(midiInGetDevCapsW);
		PROXY_INIT(midiInGetErrorTextA);
		PROXY_INIT(midiInGetErrorTextW);
		PROXY_INIT(midiInGetID);
		PROXY_INIT(midiInGetNumDevs);
		PROXY_INIT(midiInMessage);
		PROXY_INIT(midiInOpen);
		PROXY_INIT(midiInPrepareHeader);
		PROXY_INIT(midiInReset);
		PROXY_INIT(midiInStart);
		PROXY_INIT(midiInStop);
		PROXY_INIT(midiInUnprepareHeader);
		PROXY_INIT(midiOutCacheDrumPatches);
		PROXY_INIT(midiOutCachePatches);
		PROXY_INIT(midiOutClose);
		PROXY_INIT(midiOutGetDevCapsA);
		PROXY_INIT(midiOutGetDevCapsW);
		PROXY_INIT(midiOutGetErrorTextA);
		PROXY_INIT(midiOutGetErrorTextW);
		PROXY_INIT(midiOutGetID);
		PROXY_INIT(midiOutGetNumDevs);
		PROXY_INIT(midiOutGetVolume);
		PROXY_INIT(midiOutLongMsg);
		PROXY_INIT(midiOutMessage);
		PROXY_INIT(midiOutOpen);
		PROXY_INIT(midiOutPrepareHeader);
		PROXY_INIT(midiOutReset);
		PROXY_INIT(midiOutSetVolume);
		PROXY_INIT(midiOutShortMsg);
		PROXY_INIT(midiOutUnprepareHeader);
		PROXY_INIT(midiStreamClose);
		PROXY_INIT(midiStreamOpen);
		PROXY_INIT(midiStreamOut);
		PROXY_INIT(midiStreamPause);
		PROXY_INIT(midiStreamPosition);
		PROXY_INIT(midiStreamProperty);
		PROXY_INIT(midiStreamRestart);
		PROXY_INIT(midiStreamStop);
		PROXY_INIT(mixerClose);
		PROXY_INIT(mixerGetControlDetailsA);
		PROXY_INIT(mixerGetControlDetailsW);
		PROXY_INIT(mixerGetDevCapsA);
		PROXY_INIT(mixerGetDevCapsW);
		PROXY_INIT(mixerGetID);
		PROXY_INIT(mixerGetLineControlsA);
		PROXY_INIT(mixerGetLineControlsW);
		PROXY_INIT(mixerGetLineInfoA);
		PROXY_INIT(mixerGetLineInfoW);
		PROXY_INIT(mixerGetNumDevs);
		PROXY_INIT(mixerMessage);
		PROXY_INIT(mixerOpen);
		PROXY_INIT(mixerSetControlDetails);
		PROXY_INIT(mmDrvInstall);
		PROXY_INIT(mmGetCurrentTask);
		PROXY_INIT(mmTaskBlock);
		PROXY_INIT(mmTaskCreate);
		PROXY_INIT(mmTaskSignal);
		PROXY_INIT(mmTaskYield);
		PROXY_INIT(mmioAdvance);
		PROXY_INIT(mmioAscend);
		PROXY_INIT(mmioClose);
		PROXY_INIT(mmioCreateChunk);
		PROXY_INIT(mmioDescend);
		PROXY_INIT(mmioFlush);
		PROXY_INIT(mmioGetInfo);
		PROXY_INIT(mmioInstallIOProcA);
		PROXY_INIT(mmioInstallIOProcW);
		PROXY_INIT(mmioOpenA);
		PROXY_INIT(mmioOpenW);
		PROXY_INIT(mmioRead);
		PROXY_INIT(mmioRenameA);
		PROXY_INIT(mmioRenameW);
		PROXY_INIT(mmioSeek);
		PROXY_INIT(mmioSendMessage);
		PROXY_INIT(mmioSetBuffer);
		PROXY_INIT(mmioSetInfo);
		PROXY_INIT(mmioStringToFOURCCA);
		PROXY_INIT(mmioStringToFOURCCW);
		PROXY_INIT(mmioWrite);
		PROXY_INIT(mmsystemGetVersion);
		PROXY_INIT(mod32Message);
		PROXY_INIT(mxd32Message);
		PROXY_INIT(sndPlaySoundA);
		PROXY_INIT(sndPlaySoundW);
		PROXY_INIT(tid32Message);
		PROXY_INIT(timeBeginPeriod);
		PROXY_INIT(timeEndPeriod);
		PROXY_INIT(timeGetDevCaps);
		PROXY_INIT(timeGetSystemTime);
		PROXY_INIT(timeGetTime);
		PROXY_INIT(timeKillEvent);
		PROXY_INIT(timeSetEvent);
		PROXY_INIT(waveInAddBuffer);
		PROXY_INIT(waveInClose);
		PROXY_INIT(waveInGetDevCapsA);
		PROXY_INIT(waveInGetDevCapsW);
		PROXY_INIT(waveInGetErrorTextA);
		PROXY_INIT(waveInGetErrorTextW);
		PROXY_INIT(waveInGetID);
		PROXY_INIT(waveInGetNumDevs);
		PROXY_INIT(waveInGetPosition);
		PROXY_INIT(waveInMessage);
		PROXY_INIT(waveInOpen);
		PROXY_INIT(waveInPrepareHeader);
		PROXY_INIT(waveInReset);
		PROXY_INIT(waveInStart);
		PROXY_INIT(waveInStop);
		PROXY_INIT(waveInUnprepareHeader);
		PROXY_INIT(waveOutBreakLoop);
		PROXY_INIT(waveOutClose);
		PROXY_INIT(waveOutGetDevCapsA);
		PROXY_INIT(waveOutGetDevCapsW);
		PROXY_INIT(waveOutGetErrorTextA);
		PROXY_INIT(waveOutGetErrorTextW);
		PROXY_INIT(waveOutGetID);
		PROXY_INIT(waveOutGetNumDevs);
		PROXY_INIT(waveOutGetPitch);
		PROXY_INIT(waveOutGetPlaybackRate);
		PROXY_INIT(waveOutGetPosition);
		PROXY_INIT(waveOutGetVolume);
		PROXY_INIT(waveOutMessage);
		PROXY_INIT(waveOutOpen);
		PROXY_INIT(waveOutPause);
		PROXY_INIT(waveOutPrepareHeader);
		PROXY_INIT(waveOutReset);

		PROXY_INIT(waveOutRestart);
		PROXY_INIT(waveOutSetPitch);
		PROXY_INIT(waveOutSetPlaybackRate);
		PROXY_INIT(waveOutSetVolume);
		PROXY_INIT(waveOutUnprepareHeader);
		PROXY_INIT(waveOutWrite);
		PROXY_INIT(wid32Message);
		PROXY_INIT(wod32Message);
	}
}

#pragma endregion Relay

#pragma region OpenGL

typedef void(_stdcall *PFNGLBINDBUFFERARBPROC)(int target, unsigned int buffer);
typedef void(_stdcall *PFNGLBUFFERDATAARBPROC)(int target, int size, const void *data, int usage);
typedef void(_stdcall *PFNGLGENBUFFERSARBPROC)(int n, unsigned int* buffers);
typedef void(_stdcall *PFNGLDELETEBUFFERSARBPROC)(int n, const unsigned int* buffers);

extern CAPIHook hook_glBindBuffer;
extern CAPIHook hook_glBufferData;
extern CAPIHook hook_glGenBuffers;
extern CAPIHook hook_glDeleteBuffers;

struct BufferID {
	BufferID() : glBufferID(0), nextFreeIndex(0) {}
	int glBufferID;
	int nextFreeIndex;
};

unsigned int currentBufferID = 0;
unsigned int currentFreeIndex = 0; // 0 is reserved
unsigned int MAX_VISIT = 0x500;

std::vector<BufferID> bufferIDs;
std::tr1::unordered_map<std::string, unsigned int> mapDataToBufferID;

struct BufferRef {
	BufferRef() : ref(0), nextFreeIndex(0) {}
	int ref;
	int nextFreeIndex;
	std::tr1::unordered_map<std::string, unsigned int>::iterator iterator;
};

std::vector<BufferRef> bufferRefs;
unsigned int currentFreeBufferRef = 0;

void _stdcall Proxy_glBindBuffer(int target, unsigned int buffer) {
	currentBufferID = buffer;
	((PFNGLBINDBUFFERARBPROC)(PROC)hook_glBindBuffer)(target, bufferIDs[currentBufferID].glBufferID);
}

inline void CheckRef(unsigned int id) {
	unsigned int org = bufferIDs[currentBufferID].glBufferID;
	if (org != id) {
		bufferRefs[id].ref++;
		bufferIDs[currentBufferID].glBufferID = id;
		if (org != 0) {
			bufferRefs[org].ref--;
			if (bufferRefs[org].ref == 0) {
				//	printf("DROP INDEX: %d\n", org);
				bufferRefs[org].nextFreeIndex = currentFreeBufferRef;
				currentFreeBufferRef = org;
				mapDataToBufferID.erase(bufferRefs[org].iterator);
			}
		}
	}
}


void _stdcall Proxy_glBufferData(int target, int size, const void *data, int usage) {
	std::string content((const char*)data, size);
	content.append(std::string((const char*)&target, sizeof(target)));

	std::tr1::unordered_map<std::string, unsigned int>::iterator p = mapDataToBufferID.find(content);
	if (p != mapDataToBufferID.end()) {
		unsigned int id = p->second;
		((PFNGLBINDBUFFERARBPROC)(PROC)hook_glBindBuffer)(target, id);
		CheckRef(id);
		// printf("REUSE!!!! %d\n", id);
	} else {
		// Allocate gl buffer id
		unsigned int id = 0;
		if (currentFreeBufferRef != 0) {
			id = currentFreeBufferRef;
			currentFreeBufferRef = bufferRefs[id].nextFreeIndex;
		} else {
			((PFNGLGENBUFFERSARBPROC)(PROC)hook_glGenBuffers)(1, &id);
			if (bufferRefs.size() <= id) {
				bufferRefs.resize(id + 1);
			}
		}

		((PFNGLBINDBUFFERARBPROC)(PROC)hook_glBindBuffer)(target, id);
		((PFNGLBUFFERDATAARBPROC)(PROC)hook_glBufferData)(target, size, data, usage);

		// connect
		CheckRef(id);

		bufferRefs[id].iterator = mapDataToBufferID.insert(std::make_pair(content, id)).first;
		//	printf("ALLOCATE!!!! %d\n", id);
	}
}

void _stdcall Proxy_glGenBuffers(int n, unsigned int* buffers) {
	while (n-- > 0) {
		if (currentFreeIndex != 0) {
			unsigned int p = currentFreeIndex;
			*buffers++ = p;
			currentFreeIndex = bufferIDs[p].nextFreeIndex;
			bufferIDs[p].nextFreeIndex = 0;
		} else {
			// generate new one
			*buffers++ = bufferIDs.size();
			bufferIDs.push_back(BufferID());
		}
	}
}

void _stdcall Proxy_glDeleteBuffers(int n, const unsigned int* buffers) {
	for (int i = 0; i < n; i++) {
		unsigned int id = buffers[i];
		bufferIDs[id].nextFreeIndex = currentFreeIndex;
		currentFreeIndex = id;
	}

	// ((PFNGLDELETEBUFFERSARBPROC)(PROC)hook_glDeleteBuffers)(n, buffers);
}

CAPIHook hook_glBindBuffer("libglesv2.dll", "glBindBuffer", (PROC)Proxy_glBindBuffer);
CAPIHook hook_glBufferData("libglesv2.dll", "glBufferData", (PROC)Proxy_glBufferData);
CAPIHook hook_glGenBuffers("libglesv2.dll", "glGenBuffers", (PROC)Proxy_glGenBuffers);
CAPIHook hook_glDeleteBuffers("libglesv2.dll", "glDeleteBuffers", (PROC)Proxy_glDeleteBuffers);

void RedirectOpenGLEntries() {
	bufferIDs.push_back(BufferID()); // reserved.
	bufferRefs.push_back(BufferRef());
}

#pragma endregion OpenGL

#pragma region Attach

enum { PROC_ALIGN = 16, INSTR_SIZE = 64, INSTR_MATCH_COUNT = 24, MAX_SINGLE_INSTR_LENGTH = 24 };
struct Entry {
	bool operator < (const Entry& rhs) const {
		return instr < rhs.instr;
	}

	BYTE instr[INSTR_SIZE];
	int lengthHist[MAX_SINGLE_INSTR_LENGTH];
	size_t validLength;
	std::string name;
	BYTE* address;
};

static std::tr1::unordered_set<std::string> missingFuncs;
std::set<Entry> entries;

static void DumpHex(const BYTE* p, size_t size) {
	if (p == NULL) {
		printf("(NULL)\n");
	} else {
		for (size_t i = 0; i < size; i++) {
			printf("%02X ", p[i]);
			if ((i + 1) % 25 == 0) {
				printf("\n");
			}
		}
		printf("\n");
	}
}

static Entry ParseEntry(const std::string& name, const BYTE* c) {
	Entry entry;
	entry.name = name;
	entry.address = (BYTE*)c;
	BYTE* target = entry.instr;
	memset(target, 0x00, INSTR_SIZE);
	memset(entry.lengthHist, 0, sizeof(entry.lengthHist));
	xde_instr instr;
	size_t validLength = 0;
	bool edge = false;
	while (target < entry.instr + INSTR_SIZE) {
		xde_disasm((BYTE*)c, &instr);
		int len = instr.len;
		if (len == 0)
			break;

		if (instr.opcode == 0x68 || instr.addrsize == 4) {
			// read memory data
			PVOID addr = instr.opcode == 0x68 ? *(PVOID*)(c + 1) : (PVOID)instr.addr_l[0];
			char buf[16];
			memset(buf, 0, sizeof(buf));

			BYTE temp[16];
			if (instr.opcode == 0x68) {
				DWORD dwRead;
				::ReadProcessMemory(GetCurrentProcess(), addr, buf, 4, &dwRead);
				memcpy(temp, c, instr.len);
				*(PVOID*)(temp + 1) = *(PVOID*)buf;
			} else {
				instr.addr_l[0] = *(long*)buf;

				xde_asm(temp, &instr);
			}
			memcpy(target, temp, len + target > entry.instr + INSTR_SIZE ? entry.instr + INSTR_SIZE - target : len);
		} else {
			memcpy(target, c, len + target > entry.instr + INSTR_SIZE ? entry.instr + INSTR_SIZE - target : len);
		}

		c += len;
		target += len;
		if (!edge)
			validLength += len + target > entry.instr + INSTR_SIZE ? entry.instr + INSTR_SIZE - target : len;
		entry.lengthHist[len]++;

		if (*c == 0xCC) {
			edge = true;
		}
	}

	entry.validLength = validLength;
	return entry;
}

static void GetEntries(HMODULE instance, std::set<Entry>& entries) {
	ULONG size;
	PIMAGE_EXPORT_DIRECTORY expt = (PIMAGE_EXPORT_DIRECTORY)::ImageDirectoryEntryToData(instance, TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT, &size);
	if (expt != NULL) {
		const char** names = (const char**)((BYTE*)instance + expt->AddressOfNames);
		ULONG* addresses = (DWORD*)((BYTE*)instance + expt->AddressOfFunctions);
		WORD* ordinals = (WORD*)((BYTE*)instance + expt->AddressOfNameOrdinals);

		for (size_t i = 0; i < expt->NumberOfNames; i++) {
			const char* name = (const char*)instance + (long)names[i];
			if (missingFuncs.count(name) != 0) continue;
			if ((memcmp(name, "lua_", 4) == 0 || memcmp(name, "luaopen_", 8) == 0) || memcmp(name, "luaL_", 5) == 0) {
				DWORD index = ordinals[i];
				BYTE* c = (BYTE*)instance + addresses[index];
				//	printf("[%p] FIND %s - %p\n", instance, name, c);

				Entry entry = ParseEntry(name, c);

				if (entries.count(entry) == 0) {
					entries.insert(entry);
				}
			}
		}
	}
}

static void Hook(BYTE* from, BYTE* to) {
	// prepare inline hook
	unsigned char code[5] = { 0xe9, 0x00, 0x00, 0x00, 0x00 };
	*(DWORD*)(code + 1) = (DWORD)to - (DWORD)from - 5;
	DWORD oldProtect;
	::VirtualProtect(from, 5, PAGE_READWRITE, &oldProtect);
	::memcpy(from, code, 5);
	::VirtualProtect(from, 5, oldProtect, &oldProtect);
}

static int CommonLength(const BYTE* x, int xlen, const BYTE* y, int ylen) {
	int opt[INSTR_SIZE + 1][INSTR_SIZE + 1];
	memset(opt, 0, sizeof(opt));

	for (int i = 1; i <= xlen; i++) {
		for (int j = 1; j <= ylen; j++) {
			if (x[i - 1] == y[j - 1])
				opt[i][j] = opt[i - 1][j - 1] + 1;
			else
				opt[i][j] = opt[i - 1][j] > opt[i][j - 1] ? opt[i - 1][j] : opt[i][j - 1];
		}
	}

	return opt[xlen][ylen];
}

void Redirect(HMODULE from, HMODULE to) {
	std::set<Entry> toEntries, fromEntries;
	// printf("Get from entry\n");
	GetEntries(from, fromEntries);
	// printf("Get to entry\n");
	GetEntries(to, toEntries);

	std::tr1::unordered_map<std::string, BYTE*> mapAddress;
	for (std::set<Entry>::iterator it = toEntries.begin(); it != toEntries.end(); ++it) {
		mapAddress[(*it).name] = (*it).address;
		// printf("Function Refs (%s)\n", (*it).name.c_str());
	}

	printf("Start detection\n");
	if (fromEntries.empty()) {
		// search 
		IMAGE_NT_HEADERS* header = ::ImageNtHeader(from);
		printf("Image header: %p\n", header);
		printf("Text base: %p size %p\n", (BYTE*)from + header->OptionalHeader.BaseOfCode, header->OptionalHeader.SizeOfCode);

		std::set<BYTE*> marked;
		std::tr1::unordered_map<ULONG, std::list<Entry> > mapEntries;
		BYTE* addrMin = (BYTE*)0xFFFFFFFF;
		BYTE* addrMax = (BYTE*)0x0;

		std::vector<std::pair<BYTE*, BYTE*> > hookList;
		bool success = true;
		for (std::set<Entry>::iterator q = entries.begin(); q != entries.end(); ++q) {
			BYTE* address = mapAddress[(*q).name];
			if (address != NULL && (*q).validLength > 12) {
				int maxCount = 0;
				int maxTotalCount = 0;
				BYTE* best = NULL;
				for (BYTE* p = (BYTE*)from + header->OptionalHeader.BaseOfCode + 0x170000; p < (BYTE*)from + header->OptionalHeader.SizeOfCode - INSTR_SIZE; p += PROC_ALIGN) {
					if (*(p - 1) != 0xcc && *(p - 1) != 0xc3) continue;
					// find nearest
					const BYTE* left = p;
					Entry entry = ParseEntry((*q).name, left);

					int count = CommonLength(entry.instr, entry.validLength, (*q).instr, (*q).validLength);

					if (count > maxCount) {
						maxCount = count;
						maxTotalCount = CommonLength(entry.instr, INSTR_SIZE, (*q).instr, INSTR_SIZE);
						best = p;
					} else if (count == maxCount) {
						int total = CommonLength(entry.instr, INSTR_SIZE, (*q).instr, INSTR_SIZE);
						if (total >= maxTotalCount) {
							best = p;
							maxTotalCount = total;
						}
					}
				}


				if (best != NULL && maxTotalCount > INSTR_MATCH_COUNT) {
					if (marked.count(best) != NULL) {
						success = false;
						printf("ADDR %p Already registered. Please report this bug to me.\n", best);
					} else {
						//	Hook(best, address);
						hookList.push_back(std::make_pair(best, address));
					}
					// Hook(best, address);
					printf("[%p] [%d/%d] - Hooked function (%p) to (%p) %s\n", best - (BYTE*)from - header->OptionalHeader.BaseOfCode, maxTotalCount, INSTR_SIZE, best, address, (*q).name.c_str());

					addrMax = best > addrMax ? best : addrMax;
					addrMin = best < addrMin ? best : addrMin;

				} else {
					success = false;
					printf("[%p] [%d/%d] - Missing function (%p) to (%p) %s\n", best - (BYTE*)from - header->OptionalHeader.BaseOfCode, maxTotalCount, INSTR_SIZE, best, address, (*q).name.c_str());
					// Dump
					/*
					printf("Template: \n");
					DumpHex((*q).instr, INSTR_SIZE);
					printf("Target:   \n");
					DumpHex(best, INSTR_SIZE);
					*/
				}
				marked.insert(best);
			} else {
				printf("Entry: %s Missing!\n", (*q).name.c_str());
			}
		}

		printf("VALID RANGE: [%p] - [%p]\n", addrMin - ((BYTE*)from + header->OptionalHeader.BaseOfCode), addrMax - ((BYTE*)from + header->OptionalHeader.BaseOfCode));

		if (success) {
			for (std::vector<std::pair<BYTE*, BYTE*> >::const_iterator x = hookList.begin(); x != hookList.end(); ++x) {
				Hook((*x).first, (*x).second);
			}
			printf("Installation complete.\n");
		} else {
			printf("Installation failed, please report bug to me.\n");
		}
	} else {
		for (std::set<Entry>::iterator p = fromEntries.begin(); p != fromEntries.end(); ++p) {
			const std::string& name = (*p).name;
			for (std::set<Entry>::iterator q = toEntries.begin(); q != toEntries.end(); ++q) {
				const std::string& compare = (*q).name;
				if (compare == name) {
					//	Hook((*q).address, (*p).address);
				}
			}
		}
	}
}

void RedirectLuaProviderEntries(HMODULE from, HMODULE to, HMODULE refer) {
	printf("Entries: From %p, To %p, Refer: %p\n", from, to, refer);
	GetEntries(refer, entries);
	Redirect(from, to);
	::FreeLibrary(refer);
}

#pragma endregion Attach

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD ul_reason_for_call, LPVOID reserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		InitializeRelay();

		const char* funcs[] = {
			"luaL_addlstring", "luaL_addstring", "luaL_addvalue", "luaL_argerror",
			"luaL_buffinit", "luaL_callmeta", "luaL_checkany", "luaL_checkinteger",
			"luaL_checklstring", "luaL_checknumber", "luaL_checkoption", "luaL_checkstack",
			"luaL_checkudata", "luaL_findtable", "luaL_getmetafield", "luaL_gsub",
			"luaL_loadbuffer", "luaL_loadfile", "luaL_loadstring", "luaL_newmetatable",
			"luaL_newstate", "luaL_openlib", "luaL_openlibs", "luaL_optinteger",
			"luaL_optlstring", "luaL_optnumber", "luaL_prepbuffer", "luaL_pushresult",
			"luaL_ref", "luaL_register", "luaL_unref", "luaL_where",
			"lua_cpcall", "lua_equal", "lua_getallocf", "lua_gethook",
			"lua_gethookcount", "lua_gethookmask", "lua_getupvalue", "lua_isuserdata",
			"lua_pushvfstring", "lua_setallocf", "lua_setupvalue", "lua_status", "luaopen_io"
		};

		for (size_t i = 0; i < sizeof(funcs) / sizeof(funcs[0]); i++) {
			missingFuncs.insert(funcs[i]);
		}

		// TODO: code your application's behavior here.
		TCHAR filePath[MAX_PATH];
		::GetModuleFileName(NULL, filePath, MAX_PATH);

		if (_tcsstr(filePath, _T("dontstarve")) != NULL) {
			BOOL enableConsole = ::GetFileAttributes(_T("Debug.config")) != INVALID_FILE_ATTRIBUTES;

			FILE* fout, *fin;
			if (enableConsole) {
				::AllocConsole();
				fout = freopen("CONOUT$", "w+t", stdout);
				fin = freopen("CONIN$", "r+t", stdin);
			}

			RedirectLuaProviderEntries(::GetModuleHandle(NULL), ::LoadLibrary(_T("lua51.dll")), ::LoadLibrary(_T("lua51DS.dll")));
			RedirectOpenGLEntries();

			if (enableConsole) {
				system("pause");

				fclose(fout);
				fclose(fin);
				::FreeConsole();
			}
		}

		// system("pause");
		break;
	}

	return TRUE;
}
