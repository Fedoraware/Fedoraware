#include "../Hooks.h"

MAKE_HOOK(C_BaseAnimating_EnableAbsRecomputations, g_Pattern.Find(L"client.dll", L"55 8B EC FF 15 ? ? ? ? 0F B6 15 ? ? ? ? 84 C0 0F B6 4D 08 0F 45 D1 88 15 ? ? ? ? 5D C3"), void, __cdecl,
	bool bEnable)
{
	return Hook.Original<FN>()(bEnable);
}
