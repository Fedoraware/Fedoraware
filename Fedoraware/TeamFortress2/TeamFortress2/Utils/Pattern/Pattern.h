#pragma once

#include <Windows.h>

class CPattern
{
	DWORD FindPattern(DWORD dwAddress, DWORD dwLength, LPCWSTR szPattern);
	HMODULE GetModuleHandleSafe(LPCWSTR szModuleName);

public:
	DWORD Find(LPCWSTR szModuleName, LPCWSTR szPattern);
	void BytePatch(PVOID address, const char* bytes, int length);
};

inline CPattern g_Pattern;