#include "../Hooks.h"

MAKE_HOOK(C_BaseAnimating_SetAbsQueriesValid, g_Pattern.Find(L"client.dll", L"55 8B EC FF 15 ? ? ? ? 84 C0 74 0B 80 7D 08 00 0F 95 05 ? ? ? ? 5D C3"), void, __cdecl,
	bool bValid)
{
	return Hook.Original<FN>()(bValid);
}
