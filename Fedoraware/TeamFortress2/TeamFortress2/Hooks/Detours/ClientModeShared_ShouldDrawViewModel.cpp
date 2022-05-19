#include "../Hooks.h"

#include "../../Features/Visuals/Visuals.h"

MAKE_HOOK(ClientModeShared_OverrideView, Utils::GetVFuncPtr(g_Interfaces.ClientMode, 24), bool, __fastcall, void* ecx, void* edx)
{
	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (pLocal->IsScoped() && Vars::Visuals::RemoveScope.m_Var && Vars::Visuals::RemoveZoom.m_Var && !g_Interfaces.
			Input->CAM_IsThirdPerson())
		{
			return true;
		}
	}

	return Hook.Original<FN>()(ecx, edx);
}