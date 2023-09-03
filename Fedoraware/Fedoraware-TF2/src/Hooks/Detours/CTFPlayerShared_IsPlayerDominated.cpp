#include "../Hooks.h"

MAKE_HOOK(CTFPlayerShared_IsPlayerDominated, S::CTFPlayerShared_IsPlayerDominated(), bool, __fastcall, void* ecx, void* edx, int index)
{
	const bool bResult = Hook.Original<FN>()(ecx, edx, index);

	if (!bResult)
	{
		static DWORD dwDesired = g_Pattern.Find(CLIENT_DLL, "84 C0 74 ? 80 7D ? ? 74 ? 8B 83");
		static DWORD dwJump = g_Pattern.Find(CLIENT_DLL, "89 45 BC E8 ? ? ? ? 3B C7 75 1D 80 7D F8 00 75 17 8B 4D C0");

		if (reinterpret_cast<DWORD>(_ReturnAddress()) == dwDesired)
		{
			*static_cast<uintptr_t*>(_AddressOfReturnAddress()) = dwJump;
		}
	}

	return bResult;
}