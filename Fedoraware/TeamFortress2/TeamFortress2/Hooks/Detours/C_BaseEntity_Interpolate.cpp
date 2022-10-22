#include "../Hooks.h"

//	xref C_BaseEntity_BaseInterpolatePart1
MAKE_HOOK(C_BaseEntity_Interpolate, g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 28 53 57 8D 45 FC 8B F9 50 8D 45 F0 50 8D 45 E4 50 8D 45 D8 50 8D 45 08 50 E8 ? ? ? ? 83 7D FC 00 8B D8 74 46 0F B7 8F ? ? ? ? B8 ? ? ? ? 66 3B C8"), bool, __fastcall,
		  void* ecx, void* edx, float currentTime)
{
	return Vars::Misc::DisableInterpolation.Value ? true : Hook.Original<FN>()(ecx, edx, currentTime);
}
