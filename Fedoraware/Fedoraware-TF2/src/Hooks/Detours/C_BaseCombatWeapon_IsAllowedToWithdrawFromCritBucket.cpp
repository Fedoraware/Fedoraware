#include "../Hooks.h"
#include "../../Features/CritHack/CritHack.h"

MAKE_HOOK(C_BaseCombatWeapon_IsAllowedToWithdrawFromCritBucket, S::C_BaseCombatWeapon_IsAllowedToWithdrawFromCritBucket(), bool, __fastcall, void* ecx, void* edx, float flDamage)
{
	if (F::CritHack.ProtectData)
	{
		return true;
	}

	return Hook.Original<FN>()(ecx, edx, flDamage);
}