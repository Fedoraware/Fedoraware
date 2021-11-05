#pragma once
#include "../AutoGlobal/AutoGlobal.h"

class CAutoDetonate
{
public:
	void Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd);

private:
	float m_flRadius;
	bool  m_bDetonated;
};

inline CAutoDetonate g_AutoDetonate;