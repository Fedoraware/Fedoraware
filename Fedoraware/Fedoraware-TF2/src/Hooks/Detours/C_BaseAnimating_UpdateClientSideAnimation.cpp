#include "../Hooks.h"
#include "../../Features/Backtrack/Backtrack.h"

MAKE_HOOK(CBaseAnimating_UpdateClientSideAnimation, S::CBaseAnimating_UpdateClientSideAnimation(), void, __fastcall, void* ecx, void* edx)
{
	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	if (ecx == pLocal){
		if (!pLocal->InCond(TF_COND_HALLOWEEN_KART)) {
			if (CBaseCombatWeapon* pWeapon = g_EntityCache.GetWeapon()) {
				pWeapon->UpdateAllViewmodelAddons(); //credits: KGB
			}
			return;
		}
		else {
			return Hook.Original<FN>()(ecx, edx);
		}
	}
	
	if (!F::Backtrack.bUpdatingAnims) {
		return;
	}

	return Hook.Original<FN>()(ecx, edx);
}
