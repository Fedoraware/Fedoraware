#include "../Hooks.h"

MAKE_HOOK(C_BaseEntity_CalcAimEntPositions, g_Pattern.Find(L"client.dll", L"55 8B EC 81 EC ? ? ? ? A1 ? ? ? ? 53 33 DB 89 45 D8 89 5D DC 85 C0 0F 8E ? ? ? ? 56 57 A1 ? ? ? ? 8B 1C 98 F6 43 7C 01 0F 84 ? ? ? ? 80 3D ? ? ? ? ? 0F 84"), void, __cdecl,
	)
{
	return Hook.Original<FN>()();
}
