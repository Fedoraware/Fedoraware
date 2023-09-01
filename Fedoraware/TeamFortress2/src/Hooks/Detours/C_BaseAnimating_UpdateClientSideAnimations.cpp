#include "../Hooks.h"

MAKE_HOOK(C_BaseAnimating_UpdateClientSideAnimations, g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 14 8B 0D ? ? ? ? 53 56 33 F6 33 DB 89 5D EC 89 75 F0 8B 41 08 89 75 F8 85 C0 74 3E 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 56 56 56 56 8D 4D EC 51 50 8B 40 50 FF D0 8B 45 F0 83 C4 30 8B 5D EC"), void, __fastcall,
	void* ecx, void* edx)
{
	return Hook.Original<FN>()(ecx, edx);
}
