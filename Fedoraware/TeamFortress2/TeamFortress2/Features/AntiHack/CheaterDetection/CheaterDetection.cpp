#include "CheaterDetection.h"


bool CheaterDetection::shouldScan(int nIndex, CBaseEntity* pSuspect) {
	if (markedcheaters[nIndex]) { return false; } // dont rescan this player if we know they are cheating
	if (pSuspect->GetDormant()) { return false; } // dont run this player if they are dormant
	if (!pSuspect->IsAlive() || pSuspect->IsAGhost()) { return false; } // dont run this player if they are dead or ghost
	return true;
}

//bool CheaterDetection::isSteamNameDifferent(int nIndex) {
//
//}
//
//bool CheaterDetection::isPitchInvalid(int nIndex) {
//
//}
//
//bool CheaterDetection::isAimSuspicious(int nIndex) {
//
//}
//
//bool CheaterDetection::confirmedAimHacks(int nIndex) {
//
//}

void CheaterDetection::OnTick() {
	auto pLocal = g_EntityCache.m_pLocal;
	if (!pLocal || !g_Interfaces.Engine->IsConnected()) {
		lasttick.clear(); markedcheaters.clear(); strikes.clear(); isknown.clear();
		return;
	}

	for (const auto& pSuspect : g_EntityCache.GetGroup(EGroupType::PLAYERS_ALL)) {
		if (!pSuspect) { continue; }
		int index = pSuspect->GetIndex();
		if (index == pLocal->GetIndex() || !shouldScan(index, pSuspect)) { continue; }

		if (lasttick[index].tickbase) {
			//if (!isknown[index].steamname) {
			//	isknown[index].steamname = true; // to prevent false positives and needless rescanning, set this to true after the first scan.
			//	strikes[index] += isSteamNameDifferent(index) ? 1 : 0; // add a strike to this player if they are manipulating their in game name.
			//}
			//if (!isknown[index].tickbase) {
			//	if ((pSuspect->m_nTickBase() - lasttick[index].tickbase) > 16) { // if the player has changed their tickbase by more than 16 ticks from our last scan, mark them.
			//		isknown[index].tickbase = true;
			//		strikes[index]++;
			//	}
			//}
			if (!isknown[index].choke) {
				if (g_GlobalInfo.chokeMap[index].ChokedTicks >= 14) { // if the player has choked 14 or more ticks, mark them
					isknown[index].choke = true;
					strikes[index]++;
				}
			}
		}

		lasttick[index] = { pSuspect->m_nTickBase(), pSuspect->GetEyeAngles() };
		markedcheaters[index] = strikes[index];
	}
}