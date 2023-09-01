#include "../Hooks.h"

MAKE_HOOK(C_BaseEntity_MarkAimEntsDirty, g_Pattern.Find(L"client.dll", L"8B 15 ? ? ? ? 33 C0 85 D2 7E 25 8D 64 24 00 8B 0D ? ? ? ? 8B 0C 81 F7 41 ? ? ? ? ? 74 0A 81 89 ? ? ? ? ? ? ? ? 40"), void, __cdecl,
	)
{
	return Hook.Original<FN>()();
}
