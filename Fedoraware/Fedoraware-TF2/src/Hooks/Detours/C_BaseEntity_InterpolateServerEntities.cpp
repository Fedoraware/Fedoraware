#include "../Hooks.h"

MAKE_HOOK(C_BaseEntity_InterpolateServerEntities, g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 30 8B 0D ? ? ? ? 53 33 DB 89 5D DC 89 5D E0 8B 41 08 89 5D E4 89 5D E8 85 C0 74 41 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 53 53"), void, __fastcall,
	void* ecx, void* edx)
{
	return Hook.Original<FN>()(ecx, edx);
}
