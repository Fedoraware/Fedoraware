#pragma once
#include "../AutoGlobal/AutoGlobal.h"

class CAutoStab
{
private:
	bool CanBackstab(CBaseEntity* pLocal, CBaseEntity* pTarget, CBaseCombatWeapon* pWeapon, Vec3 vLocalAngles);
	bool CanBackstab(CBaseCombatWeapon* pWeapon, CBaseEntity* pTarget);
	CBaseEntity* TraceMelee(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, const Vec3& vViewAngles, const Vec3 vStart);
	bool IsEntityValid(CBaseEntity* pLocal, CBaseEntity* pEntity);
	void RunLegit(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd);
	void RunRage(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd);
	bool ShouldBacktrack = false;
	float PlayerSimTime = 0;
public:
	void Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd);

	bool m_bShouldDisguise = false;
};

ADD_FEATURE(CAutoStab, AutoStab)