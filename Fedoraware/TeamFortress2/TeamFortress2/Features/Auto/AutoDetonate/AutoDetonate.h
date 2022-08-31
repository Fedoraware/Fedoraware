#pragma once
#include "../AutoGlobal/AutoGlobal.h"

enum EDetTargets
{
	PLAYER = 1 << 0,
	BUILDING = 1 << 1,
	NPC = 1 << 2,
	BOMB = 1 << 3,
	STICKY = 1 << 4
};

class CAutoDetonate
{
	bool CheckDetonation(CBaseEntity* pLocal, const std::vector<CBaseEntity*>& entityGroup, float radius, CUserCmd* pCmd);

public:
	void Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd);
};

ADD_FEATURE(CAutoDetonate, AutoDetonate)