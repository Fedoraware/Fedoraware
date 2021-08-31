#pragma once

#include "../../../SDK/SDK.h"

class CAutoAirblast
{
public:
	void Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd);
	int id;
};

inline CAutoAirblast g_AutoAirblast;