#pragma once
#include "../../../SDK/SDK.h"

struct Detection {
	bool steamname;
	//bool aimbot;
	bool invalidpitch;
	bool tickcountmanip;
};

struct PlayerData {
	int aimSuspicion = 0;
	int tickcount;
	Detection detections;
};

class CheaterDetection
{
private:
	std::map<int, int> strikes;
	std::map<int, PlayerData> UserData;
	bool shouldScan(int nIndex, int friendsID, CBaseEntity* pSuspect);
	bool isSteamNameDifferent(PlayerInfo_t pInfo);
	bool isPitchInvalid(CBaseEntity* pSuspect);
	bool isTickCountManipulated(PlayerData SuspectData, int CurrentTickCount);
public:
	std::map<int, bool> markedcheaters;
	void OnTick();
};

inline CheaterDetection g_BadActors; // funny name f&ck you