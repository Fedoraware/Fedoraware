#pragma once
#include "../../../SDK/SDK.h"

struct Detection {
	bool steamname;
	//bool aimbot;
	bool invalidpitch;
	bool invalidtext;
};

struct PlayerData {
	int aimSuspicion = 0;
	Detection detections;
	bool areTicksSafe = true;
};

class CCheaterDetection
{
private:
	std::map<int, int> strikes;
	std::map<int, PlayerData> UserData;
	bool shouldScan(int nIndex, int friendsID, CBaseEntity* pSuspect);
	bool isSteamNameDifferent(PlayerInfo_t pInfo);
	bool isPitchInvalid(CBaseEntity* pSuspect);
	bool isTickCountManipulated(int CurrentTickCount);
public:
	std::map<int, bool> markedcheaters, illegalchar;
	void OnTick();
};

ADD_FEATURE(CCheaterDetection, BadActors)