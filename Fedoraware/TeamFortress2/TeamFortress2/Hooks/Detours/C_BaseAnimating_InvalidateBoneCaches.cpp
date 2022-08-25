#include "../Hooks.h"

MAKE_HOOK(C_BaseAnimating_InvalidateBoneCaches, g_Pattern.E8(L"client.dll", L"E8 ? ? ? ? 6A 01 6A 01 8D 4D 0F"), void, __cdecl,
	)
{
	return Hook.Original<FN>()();
}
