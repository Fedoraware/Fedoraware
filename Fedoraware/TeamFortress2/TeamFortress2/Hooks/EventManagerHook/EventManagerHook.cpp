#include "EventManagerHook.h"
#include "../../Features/Killstreak/Killstreak.h"
#include "../../Features/Fedworking/Fedworking.h"

bool __stdcall EventMangerHook::FireEventClientSide::Hook(CGameEvent* pEvent)
{
	const FNV1A_t uNameHash = FNV1A::Hash(pEvent->GetName());
	g_Killstreaker.FireEvents(pEvent, uNameHash);

	if (uNameHash == FNV1A::HashConst("party_chat")) {
		const auto msg = pEvent->GetString("text");

		// Handle networking
		if (Utils::StartsWith(msg, "FED@")) {
			if (Vars::Misc::PartyNetworking.m_Var) {
				g_Fedworking.HandleMessage(msg);
			}
			return false;
		}
	}

	return Table.Original<fn>(index)(g_Interfaces.GameEvent, pEvent);
}
