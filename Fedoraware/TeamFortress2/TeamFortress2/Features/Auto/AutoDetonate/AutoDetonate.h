#pragma once
#include "../AutoGlobal/AutoGlobal.h"

class CAutoDetonate
{
	bool CheckDetonation(CBaseEntity* pLocal, const std::vector<CBaseEntity*>& entityGroup, float radius);

public:
	void Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd);
};

ADD_FEATURE(CAutoDetonate, AutoDetonate)