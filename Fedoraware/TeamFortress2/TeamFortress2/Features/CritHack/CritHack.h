#pragma once
#include "../../SDK/SDK.h"

class CCritHack {
private:
	bool AreRandomCritsEnabled();
	bool IsEnabled();
	int NextCritTick(const CUserCmd* pCmd, int loops);

	int PreviousCrit = 0;
	int PreviousWeapon = 0;

	struct stats_t
	{
		float flCritBucket;	// 0xA54
		int iNumAttacks;	// 0xA58
		int iNumCrits;		// 0xA5C
	};

public:
	void Run(CUserCmd* pCmd);
	void Draw();
};

inline CCritHack g_CritHack;