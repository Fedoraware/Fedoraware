#include "Pattern.h"

#include <thread>
#include <vector>

#define INRANGE(x,a,b)	(x >= a && x <= b)
#define GET_BITS(x)		(INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define GET_BYTES(x)		(GET_BITS(x[0]) << 4 | GET_BITS(x[1]))

DWORD CPattern::FindPattern(DWORD dwAddress, DWORD dwLength, LPCWSTR szPattern)
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

HMODULE CPattern::GetModuleHandleSafe(LPCWSTR szModuleName)
{
	HMODULE hModule = nullptr;

	while (!hModule)
	{
		hModule = GetModuleHandleW(szModuleName);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	return hModule;
}

DWORD CPattern::Find(LPCWSTR szModuleName, LPCWSTR szPattern)
{
	if (const auto hMod = reinterpret_cast<DWORD>(GetModuleHandleSafe(szModuleName)))
	{
		if (const auto pNT = reinterpret_cast<PIMAGE_NT_HEADERS>(hMod + reinterpret_cast<PIMAGE_DOS_HEADER>(hMod)->e_lfanew))
		{
			return FindPattern(hMod + pNT->OptionalHeader.BaseOfCode, hMod + pNT->OptionalHeader.SizeOfCode, szPattern);
		}
	}

	return 0x0;
}

DWORD CPattern::E8(LPCWSTR szModuleName, LPCWSTR szPattern)
{
	DWORD dwAddress = Find(szModuleName, szPattern) + 0x1;
	return *reinterpret_cast<PDWORD>(dwAddress) + dwAddress + 4;
}



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
		Sleep(250);
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
