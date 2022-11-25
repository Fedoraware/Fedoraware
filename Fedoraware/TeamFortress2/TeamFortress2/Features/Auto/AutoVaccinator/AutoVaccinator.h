#pragma once

#include "../../../SDK/SDK.h"

enum EResistances
{
	RESIST_BULLET = 0,
	RESIST_EXPLOSIVE = 1,
	RESIST_FIRE = 2
};

constexpr int SWITCH_TICKS = 8;

class CAutoVaccinator
{
public:
	void Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd);

public:
	float m_flPing = 0;
	CBaseEntity* m_pHealingTarget = nullptr;
	Vector m_vHealingTargetPosition = { 0,0,0 };
	Vector m_vLocalPosition = { 0,0,0 };
	bool m_bPreserveOnlySelf = false;

// Resistances
public:
	int m_nDesiredResistance	= RESIST_BULLET;
	int m_nSwitchCounter		= 0;
	int m_nSwitchTicks			= 0;
	int m_nSwitchTimer			= 0;
	void SetDesiredResistance(int nCurrentResistance, const EResistances nResistance);
	void SetResistance(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, int nCurrentResistance, int nResistance);

public:
	int m_nBulletDanger = 0;
	void CalculateBulletDangerFromPlayer(CBaseEntity* pLocal, CBaseEntity* pEntity, const PlayerInfo_t& pi);
	void TestForArrows();
	void FireGameEvent(FNV1A_t uNameHash, CGameEvent* pEvent);
};

ADD_FEATURE(CAutoVaccinator, AutoVaccinator);
