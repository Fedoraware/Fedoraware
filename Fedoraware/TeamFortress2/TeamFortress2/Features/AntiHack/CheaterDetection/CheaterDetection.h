#pragma once
#include "../../../SDK/SDK.h"

struct PlayerData {
	float PlayerSuspicion = 0.f;
	float OldPlayerSuspicion = 0.f;
	int GroundTicks = 0;				//	counts how many ticks our target has been on the ground
	int BHopSuspicion = 0;				//	maxes out at 5, resets, adds a strike
	int NonDormantTimer = 0;			//	stores how many ticks we have properly scanned this player for without striking them
	Vec3 OldAngles{0, 0, 0};			//	stores the eye angles from the last tick on this player
	std::pair<bool, Vec3> StoredEndFlick{false, {0, 0, 0}};
};

class CCheaterDetection {
	std::unordered_map<int, PlayerData> UserData;
	bool ShouldScan(int nIndex, int friendsID, CBaseEntity* pSuspect);
	bool IsPitchInvalid(CBaseEntity* pSuspect);
	bool IsBhopping(CBaseEntity* pSuspect, PlayerData& pData);
	bool TrustAngles(CBaseEntity* pSuspect, PlayerData& pData);

public:
	void OnTick();
	void ReportTickCount(CBaseEntity* pSuspect, const int iChange);
};

ADD_FEATURE(CCheaterDetection, BadActors)
