#include "../Hooks.h"

std::unordered_map<void*, std::pair<int, float>> pAnimatingInfo;

MAKE_HOOK(C_BaseAnimating_FrameAdvance, g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 14 56 8B F1 57 80 BE ? ? ? ? ? 0F 85 ? ? ? ? 83 BE ? ? ? ? ? 75 05 E8 ? ? ? ? 8B BE ? ? ? ? 85 FF 0F 84 ? ? ? ? F3 0F 10 45 ? 0F 57 D2 A1 ? ? ? ? 0F 2E C2 F3 0F 10 48 ? F3 0F 11 4D"), float, __fastcall,
		  void* ecx, void* edx, float flInterval)
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
