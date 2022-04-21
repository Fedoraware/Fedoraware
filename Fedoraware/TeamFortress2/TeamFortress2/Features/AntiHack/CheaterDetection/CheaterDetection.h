#pragma once
#include "../../../SDK/SDK.h"

struct Detection {
	bool steamname;
	//bool aimbot;
	bool invalidpitch;
};

class CheaterDetection
{
private:
	std::map<int, int> strikes;
	std::map<int, Detection> isknown;
	bool shouldScan(int nIndex, int friendsID, CBaseEntity* pSuspect);
	bool isSteamNameDifferent(PlayerInfo_t pInfo);
	bool isPitchInvalid(CBaseEntity* pSuspect);
	
public:
	std::map<int, bool> markedcheaters;
	void OnTick();
};

inline CheaterDetection g_BadActors; // funny name f&ck you