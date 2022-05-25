#pragma once

#include <Windows.h>

class CPattern
{
	DWORD FindPattern(DWORD dwAddress, DWORD dwLength, LPCWSTR szPattern);
	HMODULE GetModuleHandleSafe(LPCWSTR szModuleName);

public:
	DWORD Find(LPCWSTR szModuleName, LPCWSTR szPattern);
};

inline CPattern g_Pattern;