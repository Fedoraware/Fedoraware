#pragma once

#include <Windows.h>

class CPattern
{
	DWORD FindPattern(DWORD dwAddress, DWORD dwLength, LPCWSTR szPattern);
	HMODULE GetModuleHandleSafe(LPCWSTR szModuleName);

	DWORD FindPattern(const DWORD& dwAddress, const DWORD& dwLength, LPCSTR szPattern);
	HMODULE GetModuleHandleSafe(LPCSTR szModuleName);

public:
	DWORD Find(LPCWSTR szModuleName, LPCWSTR szPattern);
	// Cry about it myzarfin
	DWORD E8(LPCWSTR szModuleName, LPCWSTR szPattern);

	// New
	DWORD Find(LPCSTR szModuleName, LPCSTR szPattern);

};

inline CPattern g_Pattern;