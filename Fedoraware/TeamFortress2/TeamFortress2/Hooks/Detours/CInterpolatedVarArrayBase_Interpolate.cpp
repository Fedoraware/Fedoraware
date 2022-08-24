#include "../Hooks.h"

//	it's a template.
MAKE_HOOK(CInterpolatedVarArrayBase_Interpolate, g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 1C D9 45 0C 8D 45 FC 56 50 83 EC 08 C7 45 ? ? ? ? ? 8D 45 E4 8B F1 D9 5C 24 04 D9 45 08 D9 1C 24 50 E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? F6 46 2C 01 53 57 74 31 83 7D FC 00 B9 ? ? ? ? D9 45 08 B8 ? ? ? ? 0F 44 C1 8B CE 50 8B 06 83 EC 08 DD 1C 24 FF 50 28 50 68 ? ? ? ? FF 15 ? ? ? ? 83 C4 14 80 7D E4 00 74 60 0F B7 5E 0E 8B 55 F0 0F B7 4E 0C 03"), int, __fastcall,
	void* ecx, void* edx, float currentTime, float interpolation_amount)
{
	if (Vars::Misc::DisableInterpolation.Value) {return 1;}
	return Hook.Original<FN>()(ecx, edx, currentTime, interpolation_amount);
}
