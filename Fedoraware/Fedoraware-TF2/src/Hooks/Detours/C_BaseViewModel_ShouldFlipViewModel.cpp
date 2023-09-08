#include "../Hooks.h"

#include "../../Features/Aimbot/AimbotProjectile/AimbotProjectile.h"

MAKE_HOOK(C_BaseViewModel_ShouldFlipViewModel, S::CBaseViewModel_ShouldFlipViewModel(), bool, __fastcall, void* ecx, void* edx)
{
	if (!Vars::Misc::AntiViewmodelFlip.Value)
	{
		return Hook.Original<FN>()(ecx, edx);
	}

	return !F::AimbotProjectile.Flippy;
}