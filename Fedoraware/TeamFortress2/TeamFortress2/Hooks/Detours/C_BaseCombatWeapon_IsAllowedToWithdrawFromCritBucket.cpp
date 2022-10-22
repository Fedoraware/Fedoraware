#include "../Hooks.h"
#include "../../Features/CritHack/CritHack.h"
#include "../../Features/Commands/Commands.h"

MAKE_HOOK(C_BaseCombatWeapon_IsAllowedToWithdrawFromCritBucket, g_Pattern.Find(L"client.dll", L"55 8B EC 56 8B F1 0F B7 86 ? ? ? ? FF 86 ? ? ? ? 50 E8 ? ? ? ? 83 C4 04 80 B8 ? ? ? ? ? 74 0A F3 0F 10 15"), bool, __fastcall,
		  void* ecx, void* edx, float flDamage)
{
	if (F::CritHack.ProtectData)
	{
		return true;
	}

	return Hook.Original<FN>()(ecx, edx, flDamage);
}