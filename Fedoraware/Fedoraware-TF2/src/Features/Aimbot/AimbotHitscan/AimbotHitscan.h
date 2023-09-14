#pragma once

#include "../AimbotGlobal/AimbotGlobal.h"

class CAimbotHitscan
{
	std::vector<Vec3> GetRandomPoints(const Vec3 vMaxs, const Vec3 vMins, CBaseEntity* pLocal, const matrix3x4 &vTransform);
	std::vector<Vec3> GetStaticPoints(const Vec3 vMaxs, const Vec3 vMins, const matrix3x4& vTransform);
	int GetHitbox(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon);
	EGroupType GetGroupType(CBaseCombatWeapon* pWeapon);
	std::vector<Target_t> GetTargets(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon);
	bool ScanHitboxes(CBaseEntity* pLocal, Target_t& target);
	bool ScanBuildings(CBaseEntity* pLocal, Target_t& target);
	bool VerifyTarget(CBaseEntity* pLocal, Target_t& target);
	bool GetTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, Target_t& outTarget);
	void Aim(CUserCmd* pCmd, Vec3& vAngle);
	bool ShouldFire(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, const CUserCmd* pCmd, const Target_t& target);
	bool IsAttacking(const CUserCmd* pCmd, CBaseCombatWeapon* pWeapon);

	int PriorityHitbox = 1; // this is the first hitbox we want to scan, just ignore it.
	CBaseEntity* PrioTarget = nullptr;

public:
	void Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd);
};

ADD_FEATURE(CAimbotHitscan, AimbotHitscan)