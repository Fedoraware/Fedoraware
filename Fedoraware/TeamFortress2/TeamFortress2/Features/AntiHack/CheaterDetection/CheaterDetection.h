#pragma once
#include "../../../SDK/SDK.h"

struct Detection {
	bool SteamName;
	//bool Aimbot;
	bool InvalidPitch;
	bool InvalidText;
	bool BHop;
};

struct PlayerData {
	int AimSuspicion = 0;		//	unused
	int GroundTicks = 0;		//	counts how many ticks our target has been on the ground
	int BHopSuspicion = 0;		//	maxes out at 5, resets, adds a strike
	Detection Detections{ };	//	
	bool AreTicksSafe = true;	//	stops us from striking a player multiple times for the same tickshift
};

class CCheaterDetection {
	std::unordered_map<int, int> Strikes;
	std::unordered_map<int, PlayerData> UserData;
	bool ShouldScan(int nIndex, int friendsID, CBaseEntity* pSuspect);
	bool IsSteamNameDifferent(PlayerInfo_t pInfo);
	bool IsPitchInvalid(CBaseEntity* pSuspect);
	bool IsTickCountManipulated(int currentTickCount);
	bool IsBhopping(CBaseEntity* pSuspect, PlayerData pData);

public:
	std::unordered_map<int, bool> MarkedCheaters, IllegalChar;
	void OnTick();
};

ADD_FEATURE(CCheaterDetection, BadActors)
