#include "CheaterDetection.h"


bool CheaterDetection::shouldScan(int nIndex, int friendsID, CBaseEntity* pSuspect) {
	if (g_EntityCache.Friends[nIndex] || (g_GlobalInfo.ignoredPlayers.find(friendsID) != g_GlobalInfo.ignoredPlayers.end()) || markedcheaters[friendsID]) { return false; } // dont rescan this player if we know they are cheating, a friend, or ignored
	if (pSuspect->GetDormant()) { return false; } // dont run this player if they are dormant
	if (!pSuspect->IsAlive() || pSuspect->IsAGhost()) { return false; } // dont run this player if they are dead or ghost
	return true;
}

bool CheaterDetection::isSteamNameDifferent(PlayerInfo_t pInfo) {	// this can be falsely triggered by a person being nicknamed without being our steam friend (pending friend) or changing their steam name since joining the game
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

bool CheaterDetection::isTickCountManipulated(PlayerData SuspectData, int CurrentTickCount) {
	int delta = SuspectData.tickcount - CurrentTickCount; // delta should be 1 however it can be different me thinks (from looking it only gets to about 3 at its worst, maybe this is different with packet loss?)
	if (delta > 14) { return true; } // lets be honest if their tickcount changes by more than 14 they are probably cheating.
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
			if (index == pLocal->GetIndex() || !shouldScan(index, friendsID, pSuspect)) { UserData[friendsID].tickcount = 0; continue; }

			if (!UserData[friendsID].detections.steamname) {
				UserData[friendsID].detections.steamname = true; // to prevent false positives and needless rescanning, set this to true after the first scan.
				strikes[friendsID] += isSteamNameDifferent(pi) ? 1 : 0; // add a strike to this player if they are manipulating their in game name.
			}

			if (!!UserData[friendsID].detections.invalidpitch) {
				if (isPitchInvalid(pSuspect)) {
					UserData[friendsID].detections.invalidpitch = true;
					strikes[friendsID] += 3; // because this cannot be falsely triggered, anyone detected by it should be marked as a cheater instantly 
				}
			}

			int currenttickcount = TIME_TO_TICKS(pSuspect->GetSimulationTime());

			if (!UserData[friendsID].detections.tickcountmanip && UserData[friendsID].tickcount) {
				if (isTickCountManipulated(UserData[friendsID], currenttickcount)) {
					UserData[friendsID].detections.tickcountmanip = true;
					strikes[friendsID] += 3;
				}
			}

			UserData[friendsID].tickcount = TIME_TO_TICKS(pSuspect->GetSimulationTime()); // maybe this has to account for ping :thinking: // no it does not
			markedcheaters[friendsID] = strikes[friendsID];
		}
	}
}