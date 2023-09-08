#include "../Hooks.h"
#include "../../Features/Vars.h"

MAKE_HOOK(C_BaseEntity_GetInterpolationAmount, g_Pattern.Find(L"client.dll", L"55 8B EC 51 56 57 8B F1 BF ? ? ? ? E8 ? ? ? ? 84 C0 B9 ? ? ? ? 0F 45 F9 80 BE ? ? ? ? ? 89 7D FC 0F 85 ? ? ? ? 8B 06 8B CE 8B"), float, __fastcall,
	void* ecx, void* edx, int flags)
{
	return Hook.Original<FN>()(ecx, edx, flags);
}
