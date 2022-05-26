#pragma once
#include "../../SDK/SDK.h"

class CKillstreaker
{
private:
	std::unordered_map<int, int> KillstreakMap;
	int Killstreak;
public:
	void ResetKillstreak();
	int	 GetCurrentStreak();
	int  GetCurrentWeaponStreak();
	void ApplyKillstreak();
private:
	void PlayerDeath(CGameEvent* pEvent);
	void PlayerSpawn(CGameEvent* pEvent);
public:
	void FireEvents(CGameEvent* pEvent, const FNV1A_t uNameHash);
};

ADD_FEATURE(CKillstreaker, Killstreaker)