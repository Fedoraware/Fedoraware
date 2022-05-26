#pragma once
#include "../../SDK/SDK.h"

class CAimbot
{
private:
	bool ShouldRun(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon);

public:
	void Run(CUserCmd* pCmd);
};

ADD_FEATURE(CAimbot, Aimbot)