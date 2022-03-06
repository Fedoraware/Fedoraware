#include "EventManagerHook.h"
#include "../../Features/Killstreak/Killstreak.h"

bool __stdcall EventMangerHook::FireEventClientSide::Hook(CGameEvent* pEvent)
{
	const FNV1A_t uNameHash = FNV1A::Hash(pEvent->GetName());
	g_Killstreaker.FireEvents(pEvent, uNameHash);

	return Table.Original<fn>(index)(g_Interfaces.GameEvent, pEvent);
}
