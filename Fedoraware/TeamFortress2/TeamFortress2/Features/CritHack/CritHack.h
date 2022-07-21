#pragma once
#include "../../SDK/SDK.h"

class CCritHack {
private:
	bool AreRandomCritsEnabled();
	bool IsEnabled();
	bool ShouldCrit();
	bool IsAttacking(const CUserCmd* pCmd, CBaseCombatWeapon* pWeapon);
	void ScanForCrits(const CUserCmd* pCmd, int loops = 10);
	int LastGoodCritTick(const CUserCmd* pCmd);

	std::vector<int> critTicks{};
	float CritBucketBP = 0;

	struct stats_t
	{
		float flCritBucket;	// 0xA54
		int iNumAttacks;	// 0xA58
		int iNumCrits;		// 0xA5C
	};

public:
	void Run(CUserCmd* pCmd);
	void Draw();
	
	bool bProtectData = false;
};

ADD_FEATURE(CCritHack, CritHack)