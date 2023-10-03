#include "../Hooks.h"

MAKE_HOOK(CBaseAnimating_UpdateClientSideAnimation, S::CBaseAnimating_UpdateClientSideAnimation(), void, __fastcall, void* ecx, void* edx)
{
	CBaseEntity* pEntity = reinterpret_cast<CBaseEntity*>(ecx);
	if (!pEntity || Vars::Misc::DisableInterpolation.Value) {
		return Hook.Original<FN>()(ecx, edx);
	}

	const int iDelta = std::clamp(TIME_TO_TICKS(pEntity->GetSimulationTime() - pEntity->GetOldSimulationTime()), 0, 22);
	for (int i = 0; i < iDelta; i++) {
		Hook.Original<FN>()(ecx, edx);
	}
}
