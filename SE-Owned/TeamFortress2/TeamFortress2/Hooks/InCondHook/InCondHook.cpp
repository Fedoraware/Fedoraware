#include "InCondHook.h"

#include "../../Features/Vars.h"

//Credits to Bertti
bool __fastcall InCondHook::Hook(void* ecx, void* edx, ETFCond nCond)
{
	if (nCond == ETFCond::TF_COND_ZOOMED)
	{
		static const auto dwPlayerShouldDraw = g_Pattern.Find(_(L"client.dll"), _(L"E8 ? ? ? ? 84 C0 75 C5")) + 0x5;
		static const auto dwWearableShouldDraw = g_Pattern.Find(_(L"client.dll"), _(L"E8 ? ? ? ? 84 C0 75 E1 6A 03")) + 0x5;

		const auto dwRetAddr = reinterpret_cast<DWORD>(_ReturnAddress());

		if (dwRetAddr == dwPlayerShouldDraw || dwRetAddr == dwWearableShouldDraw)
			return false;
	}

	auto GetOuter = [&ecx]() -> CBaseEntity* {
		static const auto dwShared = g_NetVars.get_offset(_("DT_TFPlayer"), _("m_Shared"));
		static const auto dwBombHeadStage = g_NetVars.get_offset(_("DT_TFPlayer"), _("m_Shared"), _("m_nHalloweenBombHeadStage"));
		static const auto dwOff = (dwBombHeadStage - dwShared) + 0x4;
		return *reinterpret_cast<CBaseEntity**>(reinterpret_cast<DWORD>(ecx) + dwOff);
	};

	//Compare team's, removing team's taunt is useless
	if (nCond == ETFCond::TF_COND_TAUNTING && Vars::Visuals::RemoveTaunts.m_Var)
	{
		if (const auto& pLocal = g_EntityCache.m_pLocal)
			if (const auto& pEntity = GetOuter())
				if (pEntity->GetTeamNum() != pLocal->GetTeamNum())
					return false;
	}

	//Just compare entity ptr's, filtering out local is enough. Also prevents T pose.
	if (nCond == ETFCond::TF_COND_DISGUISED && Vars::Visuals::RemoveDisguises.m_Var && g_EntityCache.m_pLocal != GetOuter())
		return false;

	return Func.Original<fn>()(ecx, edx, nCond);
}