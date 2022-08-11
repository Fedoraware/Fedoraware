#pragma once

#include "../AimbotGlobal/AimbotGlobal.h"

class CAimbotMelee
{
	EGroupType GetGroupType(CBaseCombatWeapon* pWeapon);
	bool AimFriendlyBuilding(CBaseObject* pBuilding);
	bool CanMeleeHit(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, const Vec3& vecViewAngles, int nTargetIndex);
	std::vector<Target_t> GetTargets(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon);
	bool VerifyTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, Target_t& target);
	bool GetTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, Target_t& outTarget);
	void Aim(CUserCmd* pCmd, Vec3& vAngle);
	bool ShouldSwing(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, const Target_t& Target);
	bool IsAttacking(const CUserCmd* pCmd, CBaseCombatWeapon* pWeapon);
public:
	void Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd);
};

ADD_FEATURE(CAimbotMelee, AimbotMelee)