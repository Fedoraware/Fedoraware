#include "../Hooks.h"

//	xref C_BaseEntity_BaseInterpolatePart1
MAKE_HOOK(C_BaseAnimating_Interpolate, g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 2C 56 8B F1 83 BE ? ? ? ? ? 0F 85 ? ? ? ? F3 0F 10 86 ? ? ? ? 57 33 FF F3 0F 11 45 ? 80 BE ? ? ? ? ? 75 13 FF B6 ? ? ? ? E8 ? ? ? ? 8B 8E ? ? ? ? 88 01 8D 45 FC 8B CE 50"), bool, __fastcall,
		  void* ecx, void* edx, float currentTime)
{
	return Vars::Misc::DisableInterpolation.Value ? true : Hook.Original<FN>()(ecx, edx, currentTime);
}
