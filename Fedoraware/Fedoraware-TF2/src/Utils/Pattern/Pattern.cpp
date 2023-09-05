#include "Pattern.h"

#define INRANGE(x,a,b) (x >= a && x <= b)
#define GET_BITS(x) (INRANGE(x,'0','9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xa))
#define GET_BYTES(x) (GET_BITS(x[0]) << 4 | GET_BITS(x[1]))

DWORD CPattern::FindPattern(const DWORD& dwAddress, const DWORD& dwLength, LPCSTR szPattern)
{
	auto szPat = szPattern;
	DWORD dwFirstMatch = 0x0;

	for (auto pCur = dwAddress; pCur < dwLength; pCur++)
	{
		if (!*szPat)
		{
			return dwFirstMatch;
		}

		const auto pCurByte = *(BYTE*)pCur;
		const auto pBytePatt = *(BYTE*)szPat;

		if (pBytePatt == '\?' || pCurByte == GET_BYTES(szPat))
		{
			if (!dwFirstMatch)
			{
				dwFirstMatch = pCur;
			}

			//Found
			if (!szPat[2])
			{
				return dwFirstMatch;
			}

			szPat += (pBytePatt == '\?\?' || pBytePatt != '\?') ? 3 : 2;
		}
		else
		{
			szPat = szPattern;
			dwFirstMatch = 0x0;
		}
	}

	//Failed to find, return NULL
	return 0x0;
}

HMODULE CPattern::GetModuleHandleSafe(LPCSTR szModuleName)
{
	HMODULE hModule = nullptr;
	while (!hModule)
	{
		hModule = GetModuleHandleA(szModuleName);
		if (hModule) { return hModule; }

		Sleep(10);
	}

	return hModule;
}

DWORD CPattern::Find(LPCSTR szModuleName, LPCSTR szPattern)
{
	const auto modHandle = reinterpret_cast<DWORD>(GetModuleHandleSafe(szModuleName));
	if (!modHandle) { return 0x0; }

	const auto ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(modHandle + reinterpret_cast<PIMAGE_DOS_HEADER>(modHandle)->e_lfanew);
	if (!ntHeaders) { return 0x0; }

	return FindPattern(modHandle + ntHeaders->OptionalHeader.BaseOfCode, modHandle + ntHeaders->OptionalHeader.SizeOfCode, szPattern);
}
