#pragma once
#include "../../SDK/SDK.h"

class CCritHack {
	bool AreRandomCritsEnabled();
	bool IsEnabled();
	bool ShouldCrit();
	bool IsAttacking(const CUserCmd* pCmd, CBaseCombatWeapon* pWeapon);
	int NextCritTick(const CUserCmd* pCmd, int loops);

	struct stats_t
	{
		float flCritBucket;	// 0xA54
		int iNumAttacks;	// 0xA58
		int iNumCrits;		// 0xA5C
	};

public:
	void Run(CUserCmd* pCmd);
	void Draw();
	void FireEvent(CGameEvent* pEvent, const FNV1A_t uNameHash);
	void Init();
};

ADD_FEATURE(CCritHack, CritHack)