#pragma once

#include "../XorStr/XorStr.h"

#define _(s) XorStr(s).c_str()
#define KERNEL _("kernel32.dll")
#define USER32 _("USER32.dll")

#undef GetPrivateProfileString
#undef WritePrivateProfileString
#undef MessageBox
#undef GetUserName

//I do not know who to credit for all of these "core" functions
//I thought might aswell do this for the pub release.
namespace WinAPI
{
	//==============================================
	//Core functions for calling WinAPI functions
	//==============================================
	__inline DWORD GetKernel32Addr() {
		DWORD dwAddr = 0;

		__asm {
			mov ebx, fs: [0x30]
			mov ebx, [ebx + 0x0C]
			mov ebx, [ebx + 0x14]
			mov ebx, [ebx]
			mov ebx, [ebx]
			mov ebx, [ebx + 0x10]
			mov dwAddr, ebx }

		return dwAddr;
	}

	//Credits: Shebaw
	__inline void* GetProcessAddr(DWORD dwModule, const char* szProcess) {
		char* szMod = (char*)dwModule;

		IMAGE_DOS_HEADER* pDosHeader = (IMAGE_DOS_HEADER*)szMod;
		IMAGE_NT_HEADERS* pNTHeaders = (IMAGE_NT_HEADERS*)(szMod + pDosHeader->e_lfanew);
		IMAGE_OPTIONAL_HEADER* pOPTHeader = &pNTHeaders->OptionalHeader;
		IMAGE_DATA_DIRECTORY* pExpEntry = (IMAGE_DATA_DIRECTORY*)(&pOPTHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]);
		IMAGE_EXPORT_DIRECTORY* pExpDir = (IMAGE_EXPORT_DIRECTORY*)(szMod + pExpEntry->VirtualAddress);

		void** pFuncTable = (void**)(szMod + pExpDir->AddressOfFunctions);
		WORD* pOrdTable = (WORD*)(szMod + pExpDir->AddressOfNameOrdinals);
		char** szNameTable = (char**)(szMod + pExpDir->AddressOfNames);
		void* pAddress = NULL;

		DWORD i;
		if (((DWORD)szProcess >> 16) == 0) {
			WORD ordinal = LOWORD(szProcess);
			DWORD ord_base = pExpDir->Base;

			if (ordinal < ord_base || ordinal > ord_base + pExpDir->NumberOfFunctions)
				return NULL;

			pAddress = (void*)(szMod + (DWORD)pFuncTable[ordinal - ord_base]);
		}
		else {
			for (i = 0; i < pExpDir->NumberOfNames; i++) {
				if (strcmp(szProcess, szMod + (DWORD)szNameTable[i]) == 0)
					pAddress = (void*)(szMod + (DWORD)pFuncTable[pOrdTable[i]]);
			}
		}

		if ((char*)pAddress >= (char*)pExpDir && (char*)pAddress < (char*)pExpDir + pExpEntry->Size) {
			char* dll_name, * func_name;

			HMODULE frwd_module;

			dll_name = _strdup((char*)pAddress);

			if (!dll_name)
				return NULL;

			pAddress = NULL;

			func_name = strchr(dll_name, '.');
			*func_name++ = 0;

			DWORD dwGetModuleHandle = (DWORD)GetProcessAddr(GetKernel32Addr(), _("GetModuleHandleA"));

			__asm {
				push dll_name
				call dwGetModuleHandle
				mov frwd_module, eax }

			if (!frwd_module) {
				DWORD dwLoadLibrary = (DWORD)GetProcessAddr(GetKernel32Addr(), _("LoadLibraryA"));

				__asm {
					push dll_name
					call DWORD PTR dwLoadLibrary
					mov frwd_module, eax }
			}

			if (frwd_module) pAddress = GetProcessAddr((DWORD)frwd_module, func_name);
			free(dll_name);
		}

		return pAddress;
	}

	__inline void* CallAPI(char* pszModule, char* pszFunction, int arguments, ...) {
		HANDLE hModule;

		DWORD dwLoadLibrary = (DWORD)GetProcessAddr(GetKernel32Addr(), _("LoadLibraryA"));
		PVOID pRet;

		__asm {
			push pszModule
			call dwLoadLibrary
			mov hModule, eax }

		if (!hModule)
			return NULL;

		DWORD dwFunctionAddr = (DWORD)GetProcessAddr((DWORD)hModule, pszFunction);

		if (!dwFunctionAddr)
			return NULL;

#ifdef _DEBUG
		//printf("Kernel32.dll: 0x%x \n", get_kernel32_address());
		//printf("LoadLibrary: 0x%x \n", dwLoadLibrary);
		printf("%s: 0x%x", pszModule, (DWORD)hModule);
		printf("%s: 0x%x", pszFunction, dwFunctionAddr);
#endif

		va_list params;
		void* pParam;
		va_start(params, arguments);

		for (int ax = 0; ax < arguments; ax++) {
			pParam = va_arg(params, void*);
			__asm { push pParam }
		}

		__asm {
			call DWORD PTR dwFunctionAddr
			mov pRet, eax }

		va_end(params);
		return pRet;
	}

	//==============================================
	//WinAPI calls
	//==============================================
	__inline void FreeLibraryAndExitThread(HMODULE hLibModule, DWORD dwExitCode) {
		CallAPI(KERNEL, _("FreeLibraryAndExitThread"), 2, dwExitCode, hLibModule);
	}

	__inline void GetPrivateProfileString(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString, DWORD nSize, LPCSTR lpFileName) {
		CallAPI(KERNEL, _("GetPrivateProfileStringA"), 6, lpFileName, nSize, lpReturnedString, lpDefault, lpKeyName, lpAppName);
	}

	__inline void WritePrivateProfileString(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpString, LPCSTR lpFileName) {
		CallAPI(KERNEL, _("WritePrivateProfileStringA"), 4, lpFileName, lpString, lpKeyName, lpAppName);
	}

	__inline BOOL CloseHandle(HANDLE hObject) {
		return reinterpret_cast<BOOL>(CallAPI(KERNEL, _("CloseHandle"), 1, hObject));
	}

	__inline BOOL PathFileExists(LPCSTR pszPath) {
		return reinterpret_cast<BOOL>(CallAPI(_("Shlwapi.dll"), _("PathFileExistsA"), 1, pszPath));
	}

	__inline BOOL GetUserName(LPTSTR lpBuffer, LPDWORD lpnSize) {
		return reinterpret_cast<BOOL>(CallAPI(_("Advapi32.dll"), _("GetUserNameA"), 2, &lpnSize, lpBuffer));
	}

	__inline HMODULE GetModuleHandleW(LPCWSTR lpModuleName) {
		return reinterpret_cast<HMODULE>(CallAPI(KERNEL, _("GetModuleHandleW"), 1, lpModuleName));
	}

	__inline HANDLE CreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId) {
		return reinterpret_cast<HANDLE>(CallAPI(KERNEL, _("CreateThread"), 6, lpThreadId, dwCreationFlags, lpParameter, lpStartAddress, dwStackSize, lpThreadAttributes));
	}

	__inline HANDLE CreateRemoteThread(HANDLE hProcess, LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId) {
		return reinterpret_cast<HANDLE>(CallAPI(KERNEL, _("CreateRemoteThread"), 7, lpThreadId, dwCreationFlags, lpParameter, lpStartAddress, dwStackSize, lpThreadAttributes, hProcess));
	}

	__inline HWND FindWindowW(LPCWSTR lpClassName, LPCWSTR lpWindowName) {
		return reinterpret_cast<HWND>(CallAPI(USER32, _("FindWindowW"), 2, lpWindowName, lpClassName));
	}

	__inline HWND GetForegroundWindow() {
		return reinterpret_cast<HWND>(CallAPI(USER32, _("GetForegroundWindow"), 0));
	}

	__inline DWORD GetLastError() {
		return reinterpret_cast<DWORD>(CallAPI(KERNEL, _("GetLastError"), 0));
	}

	__inline int MessageBoxA(HWND hWnd, char* lpText, char* lpCaption, UINT uType) {
		return reinterpret_cast<int>(CallAPI(USER32, _("MessageBoxA"), 4, uType, lpCaption, lpText, hWnd));
	}

	__inline int MessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType) {
		return reinterpret_cast<int>(CallAPI(USER32, _("MessageBoxW"), 4, uType, lpCaption, lpText, hWnd));
	}
}