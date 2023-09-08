#include "../Hooks.h"
#include "../../Features/CritHack/CritHack.h"

MAKE_HOOK(C_BaseCombatWeapon_AddToCritBucket, S::C_BaseCombatWeapon_AddToCritBucket(), void, __fastcall, void* ecx, void* edx, float flAmount)
{
	if (F::CritHack.ProtectData)
	{
		return;
	}

	return Hook.Original<FN>()(ecx, edx, flAmount);
}