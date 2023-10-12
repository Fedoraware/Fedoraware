#pragma once
#include "../AutoGlobal/AutoGlobal.h"

class CAutoUber
{
	float m_flHealth = 0.0f;
	float m_flMaxHealth = 0.0f;
	std::vector<int> m_MedicCallers;

public:
	void Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd);

	void AddMedicCaller(int entityId)
	{
		m_MedicCallers.push_back(entityId);
	}
};

ADD_FEATURE(CAutoUber, AutoUber)