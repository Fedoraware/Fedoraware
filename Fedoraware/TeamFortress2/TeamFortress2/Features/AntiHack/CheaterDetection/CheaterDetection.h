#pragma once
#include "../../../SDK/SDK.h"

struct Detection {
	bool SteamName;
	//bool Aimbot;
	bool InvalidPitch;
	bool InvalidText;
};

struct PlayerData {
	int AimSuspicion = 0;
	Detection Detections{ };
	bool AreTicksSafe = true;
};

class CCheaterDetection {
	std::unordered_map<int, int> Strikes;
	std::unordered_map<int, PlayerData> UserData;
	bool ShouldScan(int nIndex, int friendsID, CBaseEntity* pSuspect);
	bool IsSteamNameDifferent(PlayerInfo_t pInfo);
	bool IsPitchInvalid(CBaseEntity* pSuspect);
	bool IsTickCountManipulated(int currentTickCount);

public:
	std::unordered_map<int, bool> MarkedCheaters, IllegalChar;
	void OnTick();
};

ADD_FEATURE(CCheaterDetection, BadActors)
