#pragma once

#include "../AimbotGlobal/AimbotGlobal.h"

class CAimbotHitscan
{
private:
	int priorityhitbox = 1; // this is the first hitbox we want to scan, just ignore it.
	int GetHitbox(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon);
	ESortMethod GetSortMethod();
	bool GetTargets(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon);
	bool ScanHitboxes(CBaseEntity* pLocal, Target_t& Target);
	bool ScanBuildings(CBaseEntity* pLocal, Target_t& Target);
	bool VerifyTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, Target_t& Target);
	bool GetTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, Target_t& Out);
	void Aim(CUserCmd* pCmd, Vec3& vAngle);
	bool ShouldFire(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, const Target_t& Target);
	bool IsAttacking(CUserCmd* pCmd, CBaseCombatWeapon* pWeapon);

public:
	void Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd);
};

inline CAimbotHitscan g_AimbotHitscan;
