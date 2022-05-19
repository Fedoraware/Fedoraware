#include "../Hooks.h"

#include "../../Features/Killstreak/Killstreak.h"
#include "../../Features/Fedworking/Fedworking.h"

MAKE_HOOK(GameEventManager_FireEventClientSide, Utils::GetVFuncPtr(g_Interfaces.GameEvent, 8), bool, __fastcall,
		  void* ecx, void* edx, CGameEvent* pEvent)
{
	const FNV1A_t uNameHash = FNV1A::Hash(pEvent->GetName());
	g_Killstreaker.FireEvents(pEvent, uNameHash);

	if (uNameHash == FNV1A::HashConst("party_chat"))
	{
		const auto msg = pEvent->GetString("text");

		// Handle networking
		if (Utils::StartsWith(msg, "FED@"))
		{
			if (Vars::Misc::PartyNetworking.m_Var)
			{
				g_Fedworking.HandleMessage(msg);
			}
			return false;
		}
	}

	return Hook.Original<FN>()(ecx, edx, pEvent);
}