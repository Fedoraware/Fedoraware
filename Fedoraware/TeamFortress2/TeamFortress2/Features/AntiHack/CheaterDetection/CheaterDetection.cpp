#include "CheaterDetection.h"

// TODO: make these vars
constexpr float FL_TICKCOUNT_MULTIPLIER = 0.1f;
constexpr float FL_BHOP_MULTIPLIER = 0.5f;
constexpr float FL_SNAP_MULTIPLIER = 0.1f;
constexpr float FL_SUSPICION_REMOVAL = 0.2f;
constexpr float FL_SUSPICION_GATE = 1.f;

void conLogDetection(const char* text) {
	if (Vars::Debug::DebugInfo.Value) {
		static std::string clr({ '\x7', 'C', 'C', '0', '0', 'F', 'F' });
		I::Cvar->ConsoleColorPrintf({ 204, 0, 255, 255 }, "[CheaterDetection] ");
		I::Cvar->ConsoleColorPrintf({ 255, 255, 255, 255 }, text);
		I::ClientModeShared->m_pChatElement->ChatPrintf(0, tfm::format("%s[CheaterDetection] \x1 %s", clr, text).c_str());
	}
}

bool CCheaterDetection::ShouldScan(int nIndex, int friendsID, CBaseEntity* pSuspect)
{
	if (g_EntityCache.IsFriend(nIndex) || G::IsIgnored(friendsID) || G::PlayerPriority[friendsID].Mode == 4) { return false; } // dont rescan this player if we know they are cheating, a friend, or ignored
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
		if (suspectAngles.z >= 51.f || suspectAngles.z <= -51.f)	//	used by roll exploit
		{
			return true;
		}
	}
	return false;
}

void CCheaterDetection::ReportTickCount(CBaseEntity* pSuspect, const int iChange){
	if (iChange < 7){return;}
	PlayerInfo_t pi{ };
	if (I::EngineClient->GetPlayerInfo(pSuspect->GetIndex(), &pi))
	{
		int friendsID = pi.friendsID;
		PlayerData& userData = UserData[friendsID];

		userData.PlayerSuspicion += Math::RemapValClamped((float)iChange, 0, 22, 0, 100) * FL_TICKCOUNT_MULTIPLIER;
		//conLogDetection(tfm::format("%s was detected as shifting their tickbase.\n", pi.name).c_str());
	}
}

bool CCheaterDetection::TrustAngles(CBaseEntity* pSuspect, PlayerData& pData){
	if (pData.NonDormantTimer < 1 || G::ChokeMap[pSuspect->GetIndex()] > 1) { return true; }	//	our old angle data will not be reliable
	const Vec3 oldAngles = pData.OldAngles;
	const Vec3 curAngles = pSuspect->GetEyeAngles();

	const Vec3 vDelta = oldAngles - curAngles;
	const float flDelta = sqrt(pow(vDelta.x, 2) + pow(vDelta.y, 2));

	if (flDelta > 20){	//	this was suspicious but we don't know if they are cheating yet
		pData.StoredEndFlick = {true, curAngles};
		return true;
	}

	if (pData.StoredEndFlick.first){	//	we know they flicked and want to check for inaccuracy
		const Vec3 vEndDelta = pData.StoredEndFlick.second - curAngles;
		const float flEndDelta = sqrt(pow(vEndDelta.x, 2) + pow(vEndDelta.y, 2));;

		if (flEndDelta < 5.f){
			pData.StoredEndFlick = {false, {0, 0, 0}};
			return false;	//	their mouse movement slowed to 1/4th of the speed in one tick after moving that fast? surely not.
		}
		pData.StoredEndFlick = {false, {0, 0, 0}};
	}
	return true;
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

void CCheaterDetection::OnTick()
{
	const auto pLocal = g_EntityCache.GetLocal();
	if (!pLocal || !I::EngineClient->IsConnected())
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

		if (const INetChannel* NetChannel = I::EngineClient->GetNetChannelInfo()) {
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
		if (I::EngineClient->GetPlayerInfo(index, &pi) && !pi.fakeplayer)
		{
			int friendsID = pi.friendsID;

			PlayerData& userData = UserData[friendsID];

			if (index == pLocal->GetIndex() || !ShouldScan(index, friendsID, pSuspect)) { userData.NonDormantTimer = 0; continue; }

			if (IsPitchInvalid(pSuspect))
			{
				conLogDetection(tfm::format("%s was detected for sending an OOB pitch.\n", pi.name).c_str());
				userData.PlayerSuspicion = 1.f; // because this cannot be falsely triggered, anyone detected by it should be marked as a cheater instantly 
			}

			if (IsBhopping(pSuspect, userData)) {
				conLogDetection(tfm::format("%s was detected as bhopping.\n", pi.name).c_str());
				userData.PlayerSuspicion += 1.f * FL_BHOP_MULTIPLIER;
			}

			if (!TrustAngles(pSuspect, userData)){
				conLogDetection(tfm::format("%s was detected as aim-snapping.\n", pi.name).c_str());
				userData.PlayerSuspicion += 1.f * FL_SNAP_MULTIPLIER;
			}

			if (userData.PlayerSuspicion >= FL_SUSPICION_GATE)
			{
				conLogDetection(tfm::format("%s was marked as a cheater.\n", pi.name).c_str());
				G::PlayerPriority[friendsID].Mode = 4; // Set priority to "Cheater"
				userData.PlayerSuspicion = 0.f;	//	reset suspicion
			}
			else if (userData.NonDormantTimer >= (67 * 30) && userData.OldPlayerSuspicion == userData.PlayerSuspicion && userData.PlayerSuspicion > 0.f) {
				userData.PlayerSuspicion -= FL_SUSPICION_REMOVAL;
				userData.NonDormantTimer = 0;
				conLogDetection(tfm::format("%s has had their suspicion reduced due to good behaviour.\n", pi.name).c_str());
			}

			if (userData.OldPlayerSuspicion != userData.PlayerSuspicion) {
				userData.NonDormantTimer = 0;
			}

			userData.OldAngles = pSuspect->GetEyeAngles();
			userData.NonDormantTimer++;
			userData.OldPlayerSuspicion = userData.PlayerSuspicion;
		}
	}
}
