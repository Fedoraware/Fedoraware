#include "../Hooks.h"

#include "../../Features/Aimbot/AimbotProjectile/AimbotProjectile.h"

MAKE_HOOK(C_BaseViewModel_ShouldFlipViewModel, g_Pattern.Find(L"client.dll", L"8B 89 ? ? ? ? 85 C9 74 ? BA ? ? ? ? 83 F9 ? 74 ? 0F B7 D1 A1 ? ? ? ? C1 E2 ? 83 C0 ? 03 C2 74 ? C1 E9 ? 39 48 ? 75 ? 8B 08 85 C9 74 ? A1"), bool, __fastcall,
		  void* ecx, void* edx)
{
	if (!Vars::Misc::AntiViewmodelFlip.Value)
	{
		return Hook.Original<FN>()(ecx, edx);
	}

	return !F::AimbotProjectile.Flippy;
}