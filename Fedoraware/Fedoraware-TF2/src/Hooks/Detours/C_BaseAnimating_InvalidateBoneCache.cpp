#include "../Hooks.h"

MAKE_HOOK(C_BaseAnimating_InvalidateBoneCache, g_Pattern.Find(L"client.dll", L"55 8B EC A1 ? ? ? ? D9 45 0C 48 56 8B F1 51 D9 1C 24 89 86 ? ? ? ? 8B 06 C7 86 ? ? ? ? ? ? ? ? FF 90 ? ? ? ? D9 45 0C 8D 4E 04 8B 01 51 D9 1C 24 68"), void, __fastcall,
	void* ecx, void* edx)
{
	return Hook.Original<FN>()(ecx, edx);
}
