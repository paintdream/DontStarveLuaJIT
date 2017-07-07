//////////////////////////////////////////////////////////////
// APIHook.cpp文件
#include "stdafx.h"
#include "APIHook.h"
#include "Tlhelp32.h"
#include <dbghelp.h>
// #include <ImageHlp.h>	// 为了调用ImageDirectoryEntryToData函数
// #pragma comment(lib, "ImageHlp")


// CAPIHook对象链表的头指针

CAPIHook* CAPIHook::sm_pHeader = NULL;

CAPIHook::CAPIHook(LPSTR pszModName, LPSTR pszFuncName, PROC pfnHook, BOOL bExcludeAPIHookMod)
{
	// 保存这个Hook函数的信息
	m_bExcludeAPIHookMod = bExcludeAPIHookMod;
	m_pszModName = pszModName;
	m_pszFuncName = pszFuncName;
	m_pfnHook = pfnHook;
	m_pfnOrig = ::GetProcAddress(::GetModuleHandleA(pszModName), pszFuncName);

	if (m_pfnOrig == NULL) return;
	// 将此对象添加到链表中
	m_pNext = sm_pHeader;
	sm_pHeader = this;
	
	// 在所有当前已加载的模块中HOOK这个函数
	ReplaceIATEntryInAllMods(m_pszModName, m_pfnOrig, m_pfnHook, bExcludeAPIHookMod); 
}

CAPIHook::~CAPIHook()
{
	if (m_pfnOrig == NULL) return;
	// 取消对所有模块中函数的HOOK
	ReplaceIATEntryInAllMods(m_pszModName, m_pfnHook, m_pfnOrig, m_bExcludeAPIHookMod);

	CAPIHook *p = sm_pHeader;

	// 从链表中移除此对象
	if(p == this)
	{
		sm_pHeader = p->m_pNext;
	}
	else
	{
		while(p != NULL)
		{
			if(p->m_pNext == this)
			{
				p->m_pNext = this->m_pNext;
				break;
			}
			p = p->m_pNext;
		}
	}

}

void CAPIHook::ReplaceIATEntryInOneMod(LPSTR pszExportMod, 
				   PROC pfnCurrent, PROC pfnNew, HMODULE hModCaller)
{
	// 取得模块的导入表（import descriptor）首地址。ImageDirectoryEntryToData函数可以直接返回导入表地址
	ULONG ulSize;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)
				::ImageDirectoryEntryToData(hModCaller, TRUE, 
					IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);
	if(pImportDesc == NULL)	// 这个模块没有导入节表
	{
		return;
	}

	// 查找包含pszExportMod模块中函数导入信息的导入表项
	while(pImportDesc->Name != 0)
	{
		LPSTR pszMod = (LPSTR)((DWORD)hModCaller + pImportDesc->Name);
		if(lstrcmpiA(pszMod, pszExportMod) == 0) // 找到
			break;

		pImportDesc++;
	}
	if(pImportDesc->Name == 0) // hModCaller模块没有从pszExportMod模块导入任何函数
	{
		return;
	}

	// 取得调用者的导入地址表（import address table, IAT）
	PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)(pImportDesc->FirstThunk + (DWORD)hModCaller);

	// 查找我们要HOOK的函数，将它的地址用新函数的地址替换掉
	while(pThunk->u1.Function)
	{
		// lpAddr指向的内存保存了函数的地址
		PDWORD lpAddr = (PDWORD)&(pThunk->u1.Function);
		if(*lpAddr == (DWORD)pfnCurrent)
		{
			// 修改页的保护属性
			DWORD dwOldProtect;
			MEMORY_BASIC_INFORMATION mbi;
			::VirtualQuery(lpAddr, &mbi, sizeof(mbi));
			::VirtualProtect(lpAddr, sizeof(DWORD), PAGE_READWRITE, &dwOldProtect);

			// 修改内存地址  相当于“*lpAddr = (DWORD)pfnNew;”
			::WriteProcessMemory(::GetCurrentProcess(), 
						lpAddr, &pfnNew, sizeof(DWORD), NULL);

			::VirtualProtect(lpAddr, sizeof(DWORD), dwOldProtect, 0);
			break;
		}
		pThunk++;
	}
}

void CAPIHook::ReplaceIATEntryInAllMods(LPSTR pszExportMod, 
					PROC pfnCurrent, PROC pfnNew, BOOL bExcludeAPIHookMod)
{
	// 取得当前模块的句柄
	HMODULE hModThis = NULL;
	if(bExcludeAPIHookMod)
	{
		MEMORY_BASIC_INFORMATION mbi;
		if(::VirtualQuery(ReplaceIATEntryInAllMods, &mbi, sizeof(mbi)) != 0)
			hModThis = (HMODULE)mbi.AllocationBase;
	}

	// 取得本进程的模块列表
	HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ::GetCurrentProcessId());

	// 遍历所有模块，分别对它们调用ReplaceIATEntryInOneMod函数，修改导入地址表
	MODULEENTRY32 me = { sizeof(MODULEENTRY32) };
	BOOL bOK = ::Module32First(hSnap, &me);
	while(bOK)
	{
		// 注意：我们不HOOK当前模块的函数
		if(me.hModule != hModThis)
			ReplaceIATEntryInOneMod(pszExportMod, pfnCurrent, pfnNew, me.hModule);

		bOK = ::Module32Next(hSnap, &me);
	}
	::CloseHandle(hSnap);
}


// 挂钩LoadLibrary和GetProcAddress函数，以便在这些函数被调用以后，挂钩的函数也能够被正确的处理
/*
CAPIHook CAPIHook::sm_LoadLibraryA("Kernel32.dll", "LoadLibraryA",   
					(PROC)CAPIHook::LoadLibraryA, TRUE);

CAPIHook CAPIHook::sm_LoadLibraryW("Kernel32.dll", "LoadLibraryW",   
					(PROC)CAPIHook::LoadLibraryW, TRUE);

CAPIHook CAPIHook::sm_LoadLibraryExA("Kernel32.dll", "LoadLibraryExA", 
					(PROC)CAPIHook::LoadLibraryExA, TRUE);

CAPIHook CAPIHook::sm_LoadLibraryExW("Kernel32.dll", "LoadLibraryExW", 
					(PROC)CAPIHook::LoadLibraryExW, TRUE);

CAPIHook CAPIHook::sm_GetProcAddress("Kernel32.dll", "GetProcAddress", 
					(PROC)CAPIHook::GetProcAddress, TRUE);

*/

void WINAPI CAPIHook::HookNewlyLoadedModule(HMODULE hModule, DWORD dwFlags)
{
	// 如果一个新的模块被加载，挂钩各CAPIHook对象要求的API函数
	if((hModule != NULL) && ((dwFlags&LOAD_LIBRARY_AS_DATAFILE) == 0))
	{
		CAPIHook *p = sm_pHeader;
		while(p != NULL)
		{
			ReplaceIATEntryInOneMod(p->m_pszModName, p->m_pfnOrig, p->m_pfnHook, hModule);
			p = p->m_pNext;
		}
	}
}


HMODULE WINAPI CAPIHook::LoadLibraryA(PCSTR pszModulePath) 
{
	HMODULE hModule = ::LoadLibraryA(pszModulePath);
	HookNewlyLoadedModule(hModule, 0);
	return(hModule);
}

HMODULE WINAPI CAPIHook::LoadLibraryW(PCWSTR pszModulePath) 
{
	HMODULE hModule = ::LoadLibraryW(pszModulePath);
	HookNewlyLoadedModule(hModule, 0);
	return(hModule);
}

HMODULE WINAPI CAPIHook::LoadLibraryExA(PCSTR pszModulePath, HANDLE hFile, DWORD dwFlags) 
{
	HMODULE hModule = ::LoadLibraryExA(pszModulePath, hFile, dwFlags);
	HookNewlyLoadedModule(hModule, dwFlags);
	return(hModule);
}

HMODULE WINAPI CAPIHook::LoadLibraryExW(PCWSTR pszModulePath, HANDLE hFile, DWORD dwFlags) 
{
	HMODULE hModule = ::LoadLibraryExW(pszModulePath, hFile, dwFlags);
	HookNewlyLoadedModule(hModule, dwFlags);
	return(hModule);
}

FARPROC WINAPI CAPIHook::GetProcAddress(HMODULE hModule, PCSTR pszProcName)
{
	// 得到这个函数的真实地址
	FARPROC pfn = ::GetProcAddress(hModule, pszProcName);

	// 看它是不是我们要hook的函数
	CAPIHook *p = sm_pHeader;
	while(p != NULL)
	{
		if(p->m_pfnOrig == pfn)
		{
			pfn = p->m_pfnHook;
			break;
		}

		p = p->m_pNext;
	}

	return pfn;
}
