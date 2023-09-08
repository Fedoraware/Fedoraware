#include "../Hooks.h"

MAKE_HOOK(C_BaseEntity_ToolRecordEntities, g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 14 8B 0D ? ? ? ? 53 56 33 F6 33 DB 89 5D EC 89 75 F0 8B 41 08 89 75 F4 85 C0 74 3E 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68"), void, __cdecl,
	)
{
	return Hook.Original<FN>()();
}
