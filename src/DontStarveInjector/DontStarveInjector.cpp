// DontStarveInjector.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To merge the proxy/stub code into the object DLL, add the file 
//      dlldatax.c to the project.  Make sure precompiled headers 
//      are turned off for this file, and add _MERGE_PROXYSTUB to the 
//      defines for the project.  
//
//      If you are not running WinNT4.0 or Win95 with DCOM, then you
//      need to remove the following define from dlldatax.c
//      #define _WIN32_WINNT 0x0400
//
//      Further, if you are running MIDL without /Oicf switch, you also 
//      need to remove the following define from dlldatax.c.
//      #define USE_STUBLESS_PROXY
//
//      Modify the custom build rule for DontStarveInjector.idl by adding the following 
//      files to the Outputs.
//          DontStarveInjector_p.c
//          dlldata.c
//      To build a separate proxy/stub DLL, 
//      run nmake -f DontStarveInjectorps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "DontStarveInjector.h"
#include "dlldatax.h"

#include "DontStarveInjector_i.c"
#include "xde.h"
#include <ImageHlp.h>
#include <TlHelp32.h>
#include <string>
#include <vector>
#include <set>
#include <list>
#include <map>

#pragma comment(lib, "dbghelp.lib")

#ifdef _MERGE_PROXYSTUB
extern "C" HINSTANCE hProxyDll;
#endif

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

class CDontStarveInjectorApp : public CWinApp
{
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDontStarveInjectorApp)
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDontStarveInjectorApp)
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CDontStarveInjectorApp, CWinApp)
	//{{AFX_MSG_MAP(CDontStarveInjectorApp)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CDontStarveInjectorApp theApp;

void DumpHex(const BYTE* p, size_t size) {
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

class Matcher {
public:
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
		std::list<std::pair<PVOID, std::string> > stringList;
	};

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
				if (addr != NULL && ::ReadProcessMemory(::GetCurrentProcess(), addr, buf, 4, NULL)) {
					entry.stringList.push_back(std::make_pair(addr, std::string(buf)));
				}

				BYTE temp[16];
				if (instr.opcode == 0x68) {
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
				if (memcmp(name, "lua_", 4) == 0 || memcmp(name, "luaopen_", 8) == 0) {
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

	Matcher(HMODULE instance) {
		GetEntries(instance, entries);
		for (std::set<Entry>::const_iterator it = entries.begin(); it != entries.end(); ++it) {
			printf("[%p] FINISH %s - %p\n", instance, (*it).name.c_str(), (*it).address);
			
			for (std::list<std::pair<PVOID, std::string> >::const_iterator p = (*it).stringList.begin(); p != (*it).stringList.end(); ++p) {
				printf("Str(%p): %s\n", (*p).first, (*p).second.c_str());
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
		printf("Get from entry\n");
		GetEntries(from, fromEntries);
		printf("Get to entry\n");
		GetEntries(to, toEntries);

		std::map<std::string, BYTE*> mapAddress;
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
			std::map<ULONG, std::list<Entry> > mapEntries;
			BYTE* addrMin = (BYTE*)0xFFFFFFFF;
			BYTE* addrMax = (BYTE*)0x0;

			std::vector<std::pair<BYTE*, BYTE*> > hookList;
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
						/*
						if (entry.validLength != (*q).validLength) {
						continue;
						}*/

						/*
						int count = 0;
						for (int j = 0; j < MAX_SINGLE_INSTR_LENGTH; j++) {
							if (entry.lengthHist[j] == (*q).lengthHist[j]) {
								count++;
							}
						}*/

						/*
						for (int i = 0; i < INSTR_SIZE; i++) {
						count += (entry.instr[i] - (*q).instr[i]) == 0 ? 1 : 0;
						}*/

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

			for (std::vector<std::pair<BYTE*, BYTE*> >::const_iterator x = hookList.begin(); x != hookList.end(); ++x) {
				Hook((*x).first, (*x).second);
			}

			printf("VALID RANGE: [%p] - [%p]\n", addrMin - ((BYTE*)from + header->OptionalHeader.BaseOfCode), addrMax - ((BYTE*)from + header->OptionalHeader.BaseOfCode));
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

	std::set<Entry> entries;
};

void RedirectLuaProviderEntries(HMODULE from, HMODULE to, HMODULE refer) {
	printf("Entries: From %p, To %p, Refer: %p\n", from, to, refer);
	Matcher matcher(refer);
	matcher.Redirect(from, to);
}

typedef HRESULT(WINAPI *PFNDirectInput8Create)(HINSTANCE hinst,
											   DWORD dwVersion,
											   REFIID riidltf,
											   LPVOID *ppvOut,
											   LPUNKNOWN punkOuter
											   );

static PFNDirectInput8Create pfnDirectInput8Create = NULL;

BOOL CDontStarveInjectorApp::InitInstance() {
#ifdef _MERGE_PROXYSTUB
	hProxyDll = m_hInstance;
#endif
	_Module.Init(ObjectMap, m_hInstance, &LIBID_DontStarveInjectorLib);

	TCHAR filePath[MAX_PATH];
	::GetModuleFileName(NULL, filePath, MAX_PATH);

	if (_tcsstr(filePath, _T("dontstarve")) != NULL) {
		// check parent
		TCHAR systemPath[MAX_PATH];
		::GetSystemDirectory(systemPath, MAX_PATH);

		HMODULE hInput = ::LoadLibrary(CString(systemPath) + _T("\\DINPUT8.DLL"));
		if (hInput != NULL) {
			pfnDirectInput8Create = (PFNDirectInput8Create)::GetProcAddress(hInput, "DirectInput8Create");

			TCHAR filePath[MAX_PATH];
			::GetModuleFileName(NULL, filePath, MAX_PATH);
			::AllocConsole();
			FILE* fout = freopen("CONOUT$", "w+t", stdout);
			FILE* fin = freopen("CONIN$", "r+t", stdin);
			RedirectLuaProviderEntries(::GetModuleHandle(NULL), ::LoadLibrary(_T("luajit.dll")), ::LoadLibrary(_T("lua51.dll")));
			// system("pause");
			fclose(fout);
			fclose(fin);
			::FreeConsole();
		}
	}

	return CWinApp::InitInstance();
}

int CDontStarveInjectorApp::ExitInstance() {
	_Module.Term();
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void) {
#ifdef _MERGE_PROXYSTUB
	if (PrxDllCanUnloadNow() != S_OK)
		return S_FALSE;
#endif
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return (AfxDllCanUnloadNow() == S_OK && _Module.GetLockCount() == 0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) {
#ifdef _MERGE_PROXYSTUB
	if (PrxDllGetClassObject(rclsid, riid, ppv) == S_OK)
		return S_OK;
#endif
	return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void) {
#ifdef _MERGE_PROXYSTUB
	HRESULT hRes = PrxDllRegisterServer();
	if (FAILED(hRes))
		return hRes;
#endif
	// registers object, typelib and all interfaces in typelib
	return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void) {
#ifdef _MERGE_PROXYSTUB
	PrxDllUnregisterServer();
#endif
	return _Module.UnregisterServer(TRUE);
}


HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter) {
	return pfnDirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
}