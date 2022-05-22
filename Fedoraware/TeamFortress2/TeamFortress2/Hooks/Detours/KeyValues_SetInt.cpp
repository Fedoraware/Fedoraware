#include "../Hooks.h"

MAKE_HOOK(KeyValues_SetInt, g_Pattern.Find(L"client.dll", L"55 8B EC 6A 01 FF 75 08 E8 ? ? ? ? 85 C0 74 0A 8B 4D 0C 89 48 0C C6 40 10 02"), void, __fastcall,
		  void* ecx, void* edx, const char* szKeyName, int iValue)
{
	Hook.Original<FN>()(ecx, edx, szKeyName, iValue);

	static DWORD dwDesired = g_Pattern.Find(L"client.dll", L"8B 03 8B CB 56 57 FF 90 ? ? ? ? 6A");
	static DWORD dwJump = g_Pattern.Find(L"client.dll", L"89 45 BC E8 ? ? ? ? 3B C7 75 1D 80 7D F8 00 75 17 8B 4D C0");

	if (reinterpret_cast<DWORD>(_ReturnAddress()) == dwDesired && std::string_view(szKeyName).find("nemesis") !=
		std::string_view::npos)
	{
		*static_cast<uintptr_t*>(_AddressOfReturnAddress()) = dwJump;
	}
}