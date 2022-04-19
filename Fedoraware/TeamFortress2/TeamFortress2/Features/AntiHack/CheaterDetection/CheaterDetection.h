#pragma once
#include "../../../SDK/SDK.h"

struct OnTickData {
	int tickbase;
	Vec3 aimangles;
};

struct Detection {
	bool steamname;
	bool choke;
	bool aimbot;
	bool invalidpitch;
};

class CheaterDetection
{
private:
	std::map<int, int> strikes;
	std::map<int, OnTickData> lasttick;
	std::map<int, Detection> isknown;
	bool shouldScan(int nIndex, CBaseEntity* pSuspect);
	//bool isAimSuspicious(int nIndex);
	//bool confirmedAimHacks(int nIndex);
	bool isSteamNameDifferent(int nIndex, PlayerInfo_t pInfo);
	//bool isPitchInvalid(int nIndex);
	
public:
	std::map<int, bool> markedcheaters;
	void OnTick();
};

inline CheaterDetection g_BadActors; // funny name f&ck you