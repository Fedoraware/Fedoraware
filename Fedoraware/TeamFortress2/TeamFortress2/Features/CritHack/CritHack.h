#pragma once
#include "../../SDK/SDK.h"

class CCritHack {
	bool AreRandomCritsEnabled();
	bool IsEnabled();
	float GetCritCap(CBaseCombatWeapon* pWeapon);
	std::pair<float, float> GetCritMultInfo(CBaseCombatWeapon* pWeapon);
	float GetWithdrawMult(CBaseCombatWeapon* pWeapon);
	float GetWithdrawAmount(CBaseCombatWeapon* pWeapon);
	bool CanWithdrawFromBucket(CBaseCombatWeapon* pWeapon, bool damage);
	bool CanWeaponCrit(CBaseCombatWeapon* pWeapon, bool draw);
	int GetShotsUntilCrit(CBaseCombatWeapon* pWeapon);
	int NextCritTick(const CUserCmd* pCmd, int loops);

	struct stats_t
	{
		float flCritBucket;	// 0xA54
		int iNumAttacks;	// 0xA58
		int iNumCrits;		// 0xA5C
	};

	float LastBucket = -1.f;
	int LastCritTick = -1;
	int LastWeapon = 0;

	int ShotsUntilCrit = 0;
	int AddedPerShot = 0;
	int ShotsToFill = 0;
	int TakenPerCrit = 0;

	static inline ConVar* tf_weapon_criticals;
	static inline ConVar* tf_weapon_criticals_bucket_cap;

public:
	void Run(CUserCmd* pCmd);
	void Draw();
	void FireEvent(CGameEvent* pEvent, const FNV1A_t uNameHash);
	void Init();
};

inline CCritHack g_CritHack;