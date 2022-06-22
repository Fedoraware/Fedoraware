#include "CheaterDetection.h"

void conLogDetection(const char* text) {
	static std::string clr({ '\x7', 'C', 'C', '0', '0', 'F', 'F' });
	I::CVars->ConsoleColorPrintf({ 204, 0, 255, 255 }, "[CheaterDetection] ");
	I::CVars->ConsoleColorPrintf({ 255, 255, 255, 255 }, text);
	I::ClientMode->m_pChatElement->ChatPrintf(0, tfm::format("%s[CheaterDetection] \x1 %s", clr, text).c_str());
}

bool CCheaterDetection::ShouldScan(int nIndex, int friendsID, CBaseEntity* pSuspect)
{
	if (g_EntityCache.IsFriend(nIndex) || G::IsIgnored(friendsID) || MarkedCheaters[friendsID]) { return false; } // dont rescan this player if we know they are cheating, a friend, or ignored
	if (pSuspect->GetDormant()) { return false; } // dont run this player if they are dormant
	if (!pSuspect->IsAlive() || pSuspect->IsAGhost() || pSuspect->IsTaunting()) { return false; } // dont run this player if they are dead / ghost or taunting
	return true;
}

//bool CCheaterDetection::IsSteamNameDifferent(PlayerInfo_t pInfo)
//{
//	const CSteamID SteamID = CSteamID(static_cast<uint64>(pInfo.friendsID + 0x0110000100000000));
//
//	// this can be falsely triggered by a person being nicknamed without being our steam friend (pending friend) or changing their steam name since joining the game
//	if (const char* steamName = g_SteamInterfaces.Friends015->GetFriendPersonaName(SteamID))
//	{
//		if (strcmp(pInfo.name, steamName) != 0)
//		{
//			for (int personaNum = 0; personaNum <= 5; personaNum++) {
//				if (const char* historicalName = g_SteamInterfaces.Friends015->GetFriendPersonaNameHistory(SteamID, personaNum)) {
//					if (strcmp(pInfo.name, historicalName) == 0) {
//						return false;
//					}
//				}
//				else {
//					break;
//				}
//			}
//			conLogDetection(tfm::format("%s was detected as name changing (%s => %s).\n", pInfo.name, pInfo.name, steamName).c_str());
//			return true;
//		}
//	}
//	return false;
//}

bool CCheaterDetection::IsPitchInvalid(CBaseEntity* pSuspect)
{
	const Vec3 suspectAngles = pSuspect->GetEyeAngles();
	if (!suspectAngles.IsZero())
	{
		if (suspectAngles.x >= 90.f || suspectAngles.x <= -90.f)
		{
			return true;
		}
		if (suspectAngles.z >= 51.f || suspectAngles.z <= -51.f)
		{
			return true;
		}
	}
	return false;
}

bool CCheaterDetection::IsTickCountManipulated(int currentTickCount)
{
	const int delta = I::GlobalVars->tickcount - currentTickCount;
	// delta should be 1 however it can be different me thinks (from looking it only gets to about 3 at its worst, maybe this is different with packet loss?)
	if (abs(delta) > 20) { return true; } // who knew players lagged that much
	return false;
}

bool CCheaterDetection::IsBhopping(CBaseEntity* pSuspect, PlayerData& pData)
{
	const bool onGround = pSuspect->m_fFlags() & FL_ONGROUND;
	bool doReport = false;
	if (onGround) {
		pData.GroundTicks++;
	}
	else if (pData.GroundTicks) {
		if (pData.GroundTicks <= 2) {
			pData.BHopSuspicion++;
		}
		pData.GroundTicks = 0;
	}

	if (pData.BHopSuspicion >= 5) {
		doReport = true;
		pData.BHopSuspicion = 0;
	}
	else if (pData.GroundTicks > 2) {
		pData.BHopSuspicion = 0;
		pData.GroundTicks = 0;
	}

	return doReport;
}

bool CCheaterDetection::IsAimbotting(CBaseEntity* pSuspect, PlayerData& pData) {
	const PlayerCache suspectCache = G::Cache[pSuspect][I::GlobalVars->tickcount - 1];	// get the cache info for one tick ago.
	if (suspectCache.eyePosition.IsZero()) {
		return false;
	}

	if ((suspectCache.playersPredictedTick - TIME_TO_TICKS(pSuspect->GetSimulationTime())) > 1) {
		pData.StoredAngle.Clear();
		return false;
	}

	const Vec3 oldViewAngles = suspectCache.eyePosition;
	const Vec3 curViewAngles = pSuspect->GetEyeAngles();

	const float oldViewYaw = abs(oldViewAngles.y);
	const float oldViewPitch = oldViewAngles.x;		// there is no reason to treat pitch the same as yaw as you can't go from 180 to -180 without cheats
	
	const float curViewYaw = abs(curViewAngles.y);
	const float curViewPitch = curViewAngles.x;

	const float aimDelta = abs(curViewYaw - oldViewYaw) + abs(curViewPitch - oldViewPitch);
	const float maxAimDelta = 90.f;
	
	if (aimDelta > maxAimDelta && pData.StoredAngle.IsZero()) {	// just look at the math
		pData.FlickTime = 0;		// consider our player as having flicked
		pData.StoredAngle = curViewAngles;
		return false;
	}

	if (!pData.StoredAngle.IsZero()) {
		if (pData.FlickTime > 1) {
			pData.StoredAngle.Clear();
		}
		else if ((pData.StoredAngle - curViewAngles).IsZero()) {
			pData.StoredAngle.Clear();
			return true;
		}
	}

	pData.FlickTime++;

	return false;
}

void CCheaterDetection::OnTick()
{
	const auto pLocal = g_EntityCache.GetLocal();
	if (!pLocal || !I::Engine->IsConnected())
	{
		return;
	}

	{
		// dont scan anybody if we are running at an fps lower than our updaterate (we dont check this against 66 for servers that run at a higher tickrate :D)
		static float lastFrameTime = I::GlobalVars->realtime;
		if (g_ConVars.cl_updaterate && lastFrameTime)
		{
			const float realFrameTime = I::GlobalVars->realtime - lastFrameTime;
			lastFrameTime = I::GlobalVars->realtime;
			const int realFPS = static_cast<int>(1.0f / realFrameTime);

			if (realFPS < g_ConVars.cl_updaterate->GetInt())
			{
				return;
			}
		}

		if (const INetChannel* NetChannel = I::Engine->GetNetChannelInfo()) {
			const float lastReceivedUpdate = NetChannel->GetTimeSinceLastReceived();
			const float maxReceiveTime = I::GlobalVars->interval_per_tick * 2;
			const bool isTimingOut = NetChannel->IsTimingOut();
			const bool shouldSkip = (lastReceivedUpdate > maxReceiveTime) || isTimingOut;

			if (shouldSkip) {
				return;
			}
		}
	}

	for (const auto& pSuspect : g_EntityCache.GetGroup(EGroupType::PLAYERS_ALL))
	{
		if (!pSuspect) { continue; }
		int index = pSuspect->GetIndex();

		PlayerInfo_t pi{ };
		if (I::Engine->GetPlayerInfo(index, &pi) && !pi.fakeplayer)
		{
			int friendsID = pi.friendsID;

			if (index == pLocal->GetIndex() || !ShouldScan(index, friendsID, pSuspect)) { continue; }

			PlayerData& userData = UserData[friendsID];

			//if (userData.Detections.SteamName)
			//{
			//	userData.Detections.SteamName = true; // to prevent false positives and needless rescanning, set this to true after the first scan.
			//	Strikes[friendsID] += IsSteamNameDifferent(pi) ? 5 : 0; // add 5 strikes to this player if they are manipulating their in game name.
			//}

			if (userData.Detections.InvalidPitch)
			{
				if (IsPitchInvalid(pSuspect))
				{
					conLogDetection(tfm::format("%s was detected for sending an OOB pitch.\n", pi.name).c_str());
					userData.Detections.InvalidPitch = true;
					Strikes[friendsID] += 5; // because this cannot be falsely triggered, anyone detected by it should be marked as a cheater instantly 
				}
			}

			//if (!userData.Detections.InvalidText)
			//{
			//	if (IllegalChar[index])
			//	{
			//		conLogDetection(tfm::format("%s was detected as sending an illegal character.\n", pi.name).c_str());
			//		userData.Detections.InvalidText = true;
			//		Strikes[friendsID] += 5;
			//		IllegalChar[index] = false;
			//	}
			//}

			const int currenttickcount = TIME_TO_TICKS(pSuspect->GetSimulationTime());

			if (I::GlobalVars->tickcount)
			{
				if (IsTickCountManipulated(currenttickcount))
				{
					if (userData.AreTicksSafe)
					{
						conLogDetection(tfm::format("%s was detected as shifting their tickbase.\n", pi.name).c_str());
						Strikes[friendsID]++;
						userData.AreTicksSafe = false;
					}
				}
				else
				{
					userData.AreTicksSafe = true;
				}
			}

			if (IsBhopping(pSuspect, userData)) {
				conLogDetection(tfm::format("%s was detected as bhopping.\n", pi.name).c_str());
				Strikes[friendsID]++;
			}

			//if (IsAimbotting(pSuspect, userData)) {
			//	conLogDetection(tfm::format("%s was detected as aimbotting.\n", pi.name).c_str());
			//	Strikes[friendsID]++;
			//}

			if (Strikes[friendsID] > 4)
			{
				conLogDetection(tfm::format("%s was marked as a cheater.\n", pi.name).c_str());
				MarkedCheaters[friendsID] = true;
				G::PlayerPriority[friendsID].Mode = 4; // Set priority to "Cheater"
			}
			else if (userData.NonDormantTimer >= (67 * 120) && userData.OldStrikes == Strikes[friendsID] && Strikes[friendsID]) {
				Strikes[friendsID]--;
				userData.NonDormantTimer = 0;
				conLogDetection(tfm::format("%s has had their suspicion reduced due to good behaviour.\n", pi.name).c_str());
			}

			if (userData.OldStrikes != Strikes[friendsID]) {
				userData.NonDormantTimer = 0;
			}

			userData.NonDormantTimer++;
			userData.OldStrikes = Strikes[friendsID];
		}
	}
}

void CCheaterDetection::Event(CGameEvent* pEvent) {
	const FNV1A_t uNameHash = FNV1A::Hash(pEvent->GetName());
	const CBaseEntity* pLocal = g_EntityCache.GetLocal();
	if (!pLocal) { return; }
	int entID{};

	if (uNameHash == FNV1A::HashConst("player_hurt")) {
		entID = I::Engine->GetPlayerForUserID(pEvent->GetInt("attacker"));
	}
	else if (uNameHash == FNV1A::HashConst("player_death")) {
		entID = pEvent->GetInt("inflictor_entindex");
	}

	if (CBaseEntity* pInflictor = I::EntityList->GetClientEntity(entID)) {
		if (pInflictor == pLocal) { return; }
		PlayerCache suspectCache = G::Cache[pInflictor][I::GlobalVars->tickcount];	// get the cache info for our current tick
		suspectCache.didDamage = true;
	}
}