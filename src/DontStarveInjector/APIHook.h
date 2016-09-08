/////////////////////////////////////////////////////////
// APIHook.h文件
// From: Windows 程序设计 王艳平版光盘


#ifndef __APIHOOK_H__
#define __APIHOOK_H__

#include <windows.h>

class CAPIHook  
{
public:
	CAPIHook(LPSTR pszModName, 
		LPSTR pszFuncName, PROC pfnHook, BOOL bExcludeAPIHookMod = TRUE);
	virtual ~CAPIHook();
	operator PROC() { return m_pfnOrig; }

// 实现
private:
	LPSTR m_pszModName;		// 导出要HOOK函数的模块的名字
	LPSTR m_pszFuncName;		// 要HOOK的函数的名字
	PROC m_pfnOrig;			// 原API函数地址
	PROC m_pfnHook;			// HOOK后函数的地址
	BOOL m_bExcludeAPIHookMod;	// 是否将HOOK API的模块排除在外

private:
	static void ReplaceIATEntryInAllMods(LPSTR pszExportMod, PROC pfnCurrent, 
				PROC pfnNew, BOOL bExcludeAPIHookMod);
	static void ReplaceIATEntryInOneMod(LPSTR pszExportMod, 
				PROC pfnCurrent, PROC pfnNew, HMODULE hModCaller);


// 下面的代码用来解决其它模块动态加载DLL的问题
private:
	// 这两个指针用来将所有的CAPIHook对象连在一起
	static CAPIHook *sm_pHeader;
	CAPIHook *m_pNext;

private:
	// 当一个新的DLL被加载时，调用此函数
	static void WINAPI HookNewlyLoadedModule(HMODULE hModule, DWORD dwFlags);

	// 用来跟踪当前进程加载新的DLL
	static HMODULE WINAPI LoadLibraryA(PCSTR  pszModulePath);
	static HMODULE WINAPI LoadLibraryW(PCWSTR pszModulePath);
	static HMODULE WINAPI LoadLibraryExA(PCSTR  pszModulePath, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI LoadLibraryExW(PCWSTR pszModulePath, HANDLE hFile, DWORD dwFlags);
	
	// 如果请求已HOOK的API函数，则返回用户自定义函数的地址
	static FARPROC WINAPI GetProcAddress(HMODULE hModule, PCSTR pszProcName);
private:
	// 自动对这些函数进行挂钩
	/*
	static CAPIHook sm_LoadLibraryA;
	static CAPIHook sm_LoadLibraryW;
	static CAPIHook sm_LoadLibraryExA;
	static CAPIHook sm_LoadLibraryExW;
	static CAPIHook sm_GetProcAddress;
	*/
};

#endif // __APIHOOK_H__