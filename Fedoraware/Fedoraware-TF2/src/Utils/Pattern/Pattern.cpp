#include "Pattern.h"

#define IN_RANGE(x,a,b) (x >= a && x <= b)
#define GET_BITS(x) (IN_RANGE((x & (~0x20)),'A','F') ? ((x & (~0x20)) - 'A' + 0xA) : (IN_RANGE(x,'0','9') ? x - '0' : 0))
#define GET_BYTES(x) (GET_BITS(x[0]) << 4 | GET_BITS(x[1]))

DWORD CPattern::FindPattern(DWORD dwAddress, DWORD dwSize, LPCSTR szPattern)
{
	auto szPat = szPattern;
	DWORD dwFirstMatch = 0x0;
	const DWORD dwEnd = dwAddress + dwSize - strlen(szPattern);

	for (auto pCur = dwAddress; pCur < dwEnd; pCur++)
	{
		if (!*szPat)
		{
			return dwFirstMatch;
		}

		const auto pCurByte = *reinterpret_cast<const BYTE*>(pCur);
		const auto pBytePatt = *reinterpret_cast<const BYTE*>(szPat);

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
	while (true)
	{
		if (const auto hModule = GetModuleHandleA(szModuleName))
		{
			return hModule;
		}

		Sleep(10);
	}
}

DWORD CPattern::Find(LPCSTR szModuleName, LPCSTR szPattern)
{
	const auto modHandle = reinterpret_cast<DWORD>(GetModuleHandleSafe(szModuleName));
	if (!modHandle) { return 0x0; }

	const auto* ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(modHandle + reinterpret_cast<IMAGE_DOS_HEADER*>(modHandle)->e_lfanew);
	const auto* optHeader = &ntHeaders->OptionalHeader;

	return FindPattern(modHandle + optHeader->BaseOfCode, optHeader->SizeOfCode, szPattern);
}
