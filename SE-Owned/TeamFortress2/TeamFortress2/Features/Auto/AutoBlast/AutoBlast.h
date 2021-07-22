#pragma once

#include "../../../SDK/SDK.h"

class CAutoAirblast
{
public:
	void Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd);
};

inline CAutoAirblast g_AutoAirblast;