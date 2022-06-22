#pragma once
#include "../../../SDK/SDK.h"

struct Detection {
	//bool SteamName;
	bool InvalidPitch;
	//bool InvalidText;
};

struct PlayerData {
	Vec3 StoredAngle = Vec3(0, 0, 0);	//	stores our suspects "flick" angle
	int FlickTime = 0;					//	counts how many ticks since our target has flicked, only values of 1 are important
	int GroundTicks = 0;				//	counts how many ticks our target has been on the ground
	int BHopSuspicion = 0;				//	maxes out at 5, resets, adds a strike
	Detection Detections{ };			//	
	bool AreTicksSafe = true;			//	stops us from striking a player multiple times for the same tickshift
	int NonDormantTimer = 0;			//	stores how many ticks we have properly scanned this player for without striking them
	int OldStrikes = 0;					//	this is awful, stores how many strikes the player had on our previous check
};

class CCheaterDetection {
	std::unordered_map<int, int> Strikes;
	std::unordered_map<int, PlayerData> UserData;
	bool ShouldScan(int nIndex, int friendsID, CBaseEntity* pSuspect);
	//bool IsSteamNameDifferent(PlayerInfo_t pInfo);
	bool IsPitchInvalid(CBaseEntity* pSuspect);
	bool IsTickCountManipulated(int currentTickCount);
	bool IsBhopping(CBaseEntity* pSuspect, PlayerData& pData);
	bool IsAimbotting(CBaseEntity* pSuspect, PlayerData& pData);

public:
	std::unordered_map<int, bool> MarkedCheaters, IllegalChar;
	void OnTick();
	void Event(CGameEvent* pEvent);
};

ADD_FEATURE(CCheaterDetection, BadActors)
