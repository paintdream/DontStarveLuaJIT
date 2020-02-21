// DontStarveInjector.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DontStarveInjector.h"
#include "xde.h"
#include <ImageHlp.h>
#include <TlHelp32.h>
#include <string>
#include <vector>
#include <set>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>
#include <cassert>

#include "APIHook.h"
#pragma comment(lib, "dbghelp.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;



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

typedef void (_stdcall * PFNGLBINDBUFFERARBPROC) (int target, unsigned int buffer);
typedef void (_stdcall * PFNGLBUFFERDATAARBPROC) (int target, int size, const void *data, int usage);
typedef void (_stdcall * PFNGLGENBUFFERSARBPROC) (int n, unsigned int* buffers);
typedef void (_stdcall * PFNGLDELETEBUFFERSARBPROC) (int n, const unsigned int* buffers);

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

std::vector<BufferID> bufferIDs;
unsigned int MAX_VISIT = 0x500;

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

static std::tr1::unordered_set<std::string> nonexistFuncs;

static bool g_isDST = false;
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
				if (nonexistFuncs.count(name) != 0) continue;
				if ((memcmp(name, "lua_", 4) == 0 || memcmp(name, "luaopen_", 8) == 0) || (g_isDST && memcmp(name, "luaL_", 5) == 0)) {
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
		/*
		for (std::set<Entry>::const_iterator it = entries.begin(); it != entries.end(); ++it) {
			printf("[%p] FINISH %s - %p\n", instance, (*it).name.c_str(), (*it).address);
			
			for (std::list<std::pair<PVOID, std::string> >::const_iterator p = (*it).stringList.begin(); p != (*it).stringList.end(); ++p) {
				printf("Str(%p): %s\n", (*p).first, (*p).second.c_str());
			}
		}*/
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
	::FreeLibrary(refer);
}

typedef HRESULT(WINAPI *PFNDirectInput8Create)(HINSTANCE hinst,
											   DWORD dwVersion,
											   REFIID riidltf,
											   LPVOID *ppvOut,
											   LPUNKNOWN punkOuter
											   );

static PFNDirectInput8Create pfnDirectInput8Create = NULL;

bool CheckDST() {
	ULONG ulSize;
	HMODULE hModCaller = ::GetModuleHandle(NULL);
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)::ImageDirectoryEntryToData(hModCaller, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);

	while(pImportDesc->Name != 0) {
		LPSTR pszMod = (LPSTR)((DWORD)hModCaller + pImportDesc->Name);
		if(lstrcmpiA(pszMod, "SHLWAPI.DLL") == 0)
			return true;

		pImportDesc++;
	}

	return false;
}

HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter) {
	return pfnDirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
}

class Init {
public:
	Init() {
		nonexistFuncs.insert("luaL_addlstring");
		nonexistFuncs.insert("luaL_addstring");
		nonexistFuncs.insert("luaL_addvalue");
		nonexistFuncs.insert("luaL_argerror");
		nonexistFuncs.insert("luaL_buffinit");
		nonexistFuncs.insert("luaL_callmeta");
		nonexistFuncs.insert("luaL_checkany");
		nonexistFuncs.insert("luaL_checkinteger");
		nonexistFuncs.insert("luaL_checklstring");
		nonexistFuncs.insert("luaL_checknumber");
		nonexistFuncs.insert("luaL_checkoption");
		nonexistFuncs.insert("luaL_checkstack");
		nonexistFuncs.insert("luaL_checkudata");
		nonexistFuncs.insert("luaL_findtable");
		nonexistFuncs.insert("luaL_getmetafield");
		nonexistFuncs.insert("luaL_gsub");
		nonexistFuncs.insert("luaL_loadbuffer");
		nonexistFuncs.insert("luaL_loadfile");
		nonexistFuncs.insert("luaL_loadstring");
		nonexistFuncs.insert("luaL_newmetatable");
		nonexistFuncs.insert("luaL_newstate");
		nonexistFuncs.insert("luaL_openlib");
		nonexistFuncs.insert("luaL_openlibs");
		nonexistFuncs.insert("luaL_optinteger");
		nonexistFuncs.insert("luaL_optlstring");
		nonexistFuncs.insert("luaL_optnumber");
		nonexistFuncs.insert("luaL_prepbuffer");
		nonexistFuncs.insert("luaL_pushresult");
		nonexistFuncs.insert("luaL_ref");
		nonexistFuncs.insert("luaL_register");
		nonexistFuncs.insert("luaL_unref");
		nonexistFuncs.insert("luaL_where");
		nonexistFuncs.insert("lua_cpcall");
		nonexistFuncs.insert("lua_equal");
		nonexistFuncs.insert("lua_getallocf");
		nonexistFuncs.insert("lua_gethook");
		nonexistFuncs.insert("lua_gethookcount");
		nonexistFuncs.insert("lua_gethookmask");
		nonexistFuncs.insert("lua_getupvalue");
		nonexistFuncs.insert("lua_isuserdata");
		nonexistFuncs.insert("lua_pushvfstring");
		nonexistFuncs.insert("lua_setallocf");
		nonexistFuncs.insert("lua_setupvalue");
		nonexistFuncs.insert("lua_status");
		// TODO: code your application's behavior here.
		TCHAR filePath[MAX_PATH];
		::GetModuleFileName(NULL, filePath, MAX_PATH);

		if (_tcsstr(filePath, _T("dontstarve")) != NULL) {
			// check parent
			TCHAR systemPath[MAX_PATH];
			::GetSystemDirectory(systemPath, MAX_PATH);

			HMODULE hInput = ::LoadLibrary(CString(systemPath) + _T("\\DINPUT8.DLL"));
			if (hInput != NULL) {
				pfnDirectInput8Create = (PFNDirectInput8Create)::GetProcAddress(hInput, "DirectInput8Create");

				BOOL enableConsole = ::GetFileAttributes(_T("Debug.config")) != INVALID_FILE_ATTRIBUTES;
				
				FILE* fout, *fin;
				if (enableConsole)
				{
					::AllocConsole();
					fout = freopen("CONOUT$", "w+t", stdout);
					fin = freopen("CONIN$", "r+t", stdin);
				}

				g_isDST = CheckDST();
				printf("Application: %s\n", g_isDST ? "Don't Starve Together" : "Don't Starve");
				RedirectLuaProviderEntries(::GetModuleHandle(NULL), ::LoadLibrary(_T("lua51.dll")), g_isDST ? ::LoadLibrary(_T("lua51DST.dll")) : ::LoadLibrary(_T("lua51DS.dll")));
				RedirectOpenGLEntries();

				if (enableConsole)
				{
					system("pause");

					fclose(fout);
					fclose(fin);
					::FreeConsole();
				}
			}
		}
	}
} theInit;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
	}

	return nRetCode;
}
