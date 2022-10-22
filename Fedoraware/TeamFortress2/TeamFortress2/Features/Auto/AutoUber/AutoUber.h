#pragma once

#include "../../../SDK/SDK.h"

class CAutoUber
{
public:
	void Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd);

private:
	float m_flHealth = 0.0f;
	float m_flMaxHealth = 0.0f;
};

ADD_FEATURE(CAutoUber, AutoUber)