#pragma once
#include "../AutoGlobal/AutoGlobal.h"

class CAutoStab
{
private:
	bool CanBackstabEx(const Vec3 vFrom, const Vec3 vTo, const Vec3 vTargetAng);
	bool CanBackstab(const Vec3& vSrc, const Vec3& vDst, Vec3 vWSCDelta);
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