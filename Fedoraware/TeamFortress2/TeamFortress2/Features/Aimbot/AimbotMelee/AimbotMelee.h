#pragma once

#include "../AimbotGlobal/AimbotGlobal.h"

class CAimbotMelee
{
	ESortMethod GetSortMethod();
	bool CanMeleeHit(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, const Vec3& vecViewAngles, int nTargetIndex);
	bool GetTargets(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon);
	bool VerifyTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, Target_t& target);
	bool GetTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, Target_t& Out);
	void Aim(CUserCmd* pCmd, Vec3& vAngle);
	bool ShouldSwing(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, const Target_t& Target);
	bool IsAttacking(CUserCmd* pCmd, CBaseCombatWeapon* pWeapon);
public:
	void Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd);
};

ADD_FEATURE(CAimbotMelee, AimbotMelee)