#include "CheaterDetection.h"

bool CCheaterDetection::ShouldScan(int nIndex, int friendsID, CBaseEntity* pSuspect)
{
	if (g_EntityCache.IsFriend(nIndex) || G::IsIgnored(friendsID) || MarkedCheaters[friendsID]) { return false; } // dont rescan this player if we know they are cheating, a friend, or ignored
	if (pSuspect->GetDormant()) { return false; } // dont run this player if they are dormant
	if (!pSuspect->IsAlive() || pSuspect->IsAGhost() || pSuspect->IsTaunting()) { return false; } // dont run this player if they are dead / ghost or taunting
	return true;
}

bool CCheaterDetection::IsSteamNameDifferent(PlayerInfo_t pInfo)
{
	// this can be falsely triggered by a person being nicknamed without being our steam friend (pending friend) or changing their steam name since joining the game
	if (const char* steamName = g_SteamInterfaces.Friends015->GetFriendPersonaName(CSteamID(static_cast<uint64>(pInfo.friendsID + 0x0110000100000000))))
	{
		if (strcmp(pInfo.name, steamName) != 0)
		{
			return true;
		}
	}
	return false;
}

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
	if (abs(delta) > 14) { return true; } // lets be honest if their tickcount changes by more than 14 they are probably cheating.
	return false;
}

void CCheaterDetection::OnTick()
{
	const auto pLocal = g_EntityCache.m_pLocal;
	if (!pLocal || !I::Engine->IsConnected() || !Vars::ESP::Players::CheaterDetection.Value)
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

			if (!UserData[friendsID].Detections.SteamName)
			{
				UserData[friendsID].Detections.SteamName = true; // to prevent false positives and needless rescanning, set this to true after the first scan.
				Strikes[friendsID] += IsSteamNameDifferent(pi) ? 1 : 0; // add a strike to this player if they are manipulating their in game name.
			}

			if (!UserData[friendsID].Detections.InvalidPitch)
			{
				if (IsPitchInvalid(pSuspect))
				{
					UserData[friendsID].Detections.InvalidPitch = true;
					Strikes[friendsID] += 5; // because this cannot be falsely triggered, anyone detected by it should be marked as a cheater instantly 
				}
			}

			if (!UserData[friendsID].Detections.InvalidText)
			{
				if (IllegalChar[index])
				{
					UserData[friendsID].Detections.InvalidText = true;
					Strikes[friendsID] += 5;
					IllegalChar[index] = false;
				}
			}

			const int currenttickcount = TIME_TO_TICKS(pSuspect->GetSimulationTime());

			if (I::GlobalVars->tickcount)
			{
				if (IsTickCountManipulated(currenttickcount))
				{
					if (UserData[friendsID].AreTicksSafe)
					{
						I::ClientMode->m_pChatElement->ChatPrintf(pLocal->GetIndex(), tfm::format("[Debug] Tickcount for player %s was detected as changing.", pi.name).c_str());
						Strikes[friendsID] += 1;
						UserData[friendsID].AreTicksSafe = false;
					}
				}
				else
				{
					UserData[friendsID].AreTicksSafe = true;
				}
			}

			if (Strikes[friendsID] > 4)
			{
				MarkedCheaters[friendsID] = true;
				G::PlayerPriority[friendsID].Mode = 4; // Set priority to "Cheater"
			}
		}
	}
}
