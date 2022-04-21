#include "CheaterDetection.h"


bool CheaterDetection::shouldScan(int nIndex, int friendsID, CBaseEntity* pSuspect) {
	if (g_EntityCache.Friends[nIndex] || (g_GlobalInfo.ignoredPlayers.find(friendsID) != g_GlobalInfo.ignoredPlayers.end()) || markedcheaters[friendsID]) { return false; } // dont rescan this player if we know they are cheating, a friend, or ignored
	if (pSuspect->GetDormant()) { return false; } // dont run this player if they are dormant
	if (!pSuspect->IsAlive() || pSuspect->IsAGhost()) { return false; } // dont run this player if they are dead or ghost
	return true;
}

bool CheaterDetection::isSteamNameDifferent(PlayerInfo_t pInfo) {
	if (const char* steam_name = g_SteamInterfaces.Friends015->GetFriendPersonaName(CSteamID(static_cast<uint64>(pInfo.friendsID + 0x0110000100000000))))
	{
		if (strcmp(pInfo.name, steam_name))
			return true;
	}
	return false;
}

bool CheaterDetection::isPitchInvalid(CBaseEntity* pSuspect) {
	Vec3 suspectAngles = pSuspect->GetEyeAngles();
	if (!suspectAngles.IsZero()) {
		if (suspectAngles.x >= 90.f || suspectAngles.x <= -90.f) {
			return true;
		}
	}
	return false;
}

void CheaterDetection::OnTick() {
	auto pLocal = g_EntityCache.m_pLocal;
	if (!pLocal || !g_Interfaces.Engine->IsConnected()) {
		return;
	}

	for (const auto& pSuspect : g_EntityCache.GetGroup(EGroupType::PLAYERS_ALL)) {
		if (!pSuspect) { continue; }
		int index = pSuspect->GetIndex();

		PlayerInfo_t pi;
		if (g_Interfaces.Engine->GetPlayerInfo(index, &pi) && !pi.fakeplayer) {
			int friendsID = pi.friendsID;
			//if (index == pLocal->GetIndex() || !shouldScan(index, friendsID, pSuspect)) { continue; }

			if (!isknown[friendsID].steamname) {
				isknown[friendsID].steamname = true; // to prevent false positives and needless rescanning, set this to true after the first scan.
				strikes[friendsID] += isSteamNameDifferent(pi) ? 1 : 0; // add a strike to this player if they are manipulating their in game name.
			}

			if (!isknown[friendsID].invalidpitch) {
				if (isPitchInvalid(pSuspect)) {
					isknown[friendsID].invalidpitch = true;
					strikes[friendsID]++;
				}
			}

			markedcheaters[friendsID] = strikes[friendsID];
		}
	}
}