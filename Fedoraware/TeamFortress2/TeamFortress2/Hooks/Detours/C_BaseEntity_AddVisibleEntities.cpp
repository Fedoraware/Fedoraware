#include "../Hooks.h"

MAKE_HOOK(C_BaseEntity_AddVisibleEntities, g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 18 8B 0D ? ? ? ? 53 33 DB 89 5D E8 89 5D EC 8B 41 08 89 5D F4 89 5D F8 56 57 85 C0 74 41 68 ? ? ? ? 68"), void, __cdecl,
	)
{
	return Hook.Original<FN>()();
}
