#pragma once
#include "../AutoGlobal/AutoGlobal.h"

class CAutoShoot
{
private:
	bool IsAimingAtValidTarget(CBaseEntity* pLocal, CUserCmd* pCmd, float* pSimTime);
	bool ShouldFire(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon);

public:
	void Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd);
};

ADD_FEATURE(CAutoShoot, AutoShoot)