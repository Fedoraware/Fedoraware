#include "../Hooks.h"
#include "../../Features/Backtrack/Backtrack.h"

MAKE_HOOK(NetChannel_SendDatagram, g_Pattern.Find(L"engine.dll", L"55 8B EC B8 ? ? ? ? E8 ? ? ? ? A1 ? ? ? ? 53 56 8B D9"), int, __fastcall,
		  INetChannel* netChannel, void* edx, bf_write* datagram)
{
	if (!netChannel || datagram) {
		return Hook.Original<FN>()(netChannel, edx, datagram);
	}

	CBaseEntity* pLocal = g_EntityCache.GetLocal();

	if (!pLocal || !pLocal->IsAlive() || pLocal->IsAGhost() || !Vars::Backtrack::Enabled.Value || !Vars::Backtrack::Latency.Value) {
		F::Backtrack.bFakeLatency = false;
		return Hook.Original<FN>()(netChannel, edx, datagram);
	}

	F::Backtrack.bFakeLatency = true;

	const int inSequence = netChannel->m_nInSequenceNr;
	const int inState = netChannel->m_nInReliableState;

	F::Backtrack.AdjustPing(netChannel);

	const int original = Hook.Original<FN>()(netChannel, edx, datagram);
	netChannel->m_nInSequenceNr = inSequence;
	netChannel->m_nInReliableState = inState;

	return original;
}