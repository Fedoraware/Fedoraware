#include "../Hooks.h"

std::unordered_map<void*, std::pair<int, float>> pAnimatingInfo;

MAKE_HOOK(C_BaseAnimating_FrameAdvance, S::CBaseAnimating_FrameAdvance(), float, __fastcall, void* ecx, void* edx, float flInterval)
{
	if (const auto pEntity = static_cast<CBaseEntity*>(ecx))
	{
		if (pEntity->IsPlayer())
		{
			if (pEntity->GetSimulationTime() == pEntity->GetOldSimulationTime() || I::GlobalVars->tickcount == pAnimatingInfo[ecx].first)
			{
				pAnimatingInfo[ecx].second += flInterval;
				return 0.0f;
			}
		}
	}

	flInterval = pAnimatingInfo[ecx].second; pAnimatingInfo[ecx].second = 0.0f; pAnimatingInfo[ecx].first = I::GlobalVars->tickcount;
	return Hook.Original<FN>()(ecx, edx, flInterval);
}
