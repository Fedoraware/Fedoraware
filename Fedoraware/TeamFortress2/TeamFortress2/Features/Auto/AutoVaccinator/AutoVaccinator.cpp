#include "AutoVaccinator.h"

void CAutoVaccinator::Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	if (!pLocal || !pWeapon || !pCmd)
	{
		return;
	}

	if (G::CurItemDefIndex != Medic_s_TheVaccinator)
	{
		return;
	}

	m_nBulletDanger = 0;

	if (const auto& pNetChannel = I::EngineClient->GetNetChannelInfo())
	{
		m_flPing = pNetChannel->GetLatency(FLOW_OUTGOING) + pNetChannel->GetLatency(FLOW_INCOMING);
	}

	/*
	0 = Bullet | 1 = Explosive | 2 = Fire
	*/
	const int nCurrentResistance = pWeapon->GetChargeResistType();

	m_pHealingTarget = pWeapon->GetHealingTarget();

	if (m_pHealingTarget)
	{
		m_vHealingTargetPosition = m_pHealingTarget->GetWorldSpaceCenter();
		m_bPreserveOnlySelf = false;
	}
	else
	{
		
		m_bPreserveOnlySelf = true;
	}
	
	m_vLocalPosition = pLocal->GetWorldSpaceCenter();

#ifdef _DEBUG
	if (GetAsyncKeyState('4') & 0x1)
	{
		I::Cvar->ConsolePrintf("Current resistance is: %d\n", nCurrentResistance);
	}

	if (GetAsyncKeyState(VK_F1) & 0x1)
	{
		m_nDesiredResistance = RESIST_BULLET;
	}

	if (GetAsyncKeyState(VK_F2) & 0x1)
	{
		m_nDesiredResistance = RESIST_EXPLOSIVE;
	}

	if (GetAsyncKeyState(VK_F3) & 0x1)
	{
		m_nDesiredResistance = RESIST_FIRE;
	}
#endif

	TestForArrows();

	if (m_nBulletDanger)
	{
		SetDesiredResistance(nCurrentResistance, RESIST_BULLET);
	}

	SetResistance(pLocal, pWeapon, pCmd, nCurrentResistance, m_nDesiredResistance);

	if (nCurrentResistance == m_nDesiredResistance && m_nBulletDanger)
	{
		pCmd->buttons |= IN_ATTACK2;
	}
}

void CAutoVaccinator::SetDesiredResistance(int nCurrentResistance, const EResistances nResistance)
{
	if (nResistance < 0 || nResistance > 2)
	{
		return;
	}

	m_nSwitchTimer = 5;
	m_nDesiredResistance = nResistance;

	if (nResistance == nCurrentResistance)
	{
		return;
	}

	m_nSwitchCounter = (nResistance > nCurrentResistance) ? (nResistance - nCurrentResistance) : (3 - nCurrentResistance + nResistance);
}

void CAutoVaccinator::SetResistance(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, int nCurrentResistance, int nResistance)
{
	if (m_nSwitchTimer)
	{
		m_nSwitchTimer--;
	}
	else
	{
		m_nSwitchTimer = 5;
		return;
	}

	if (nCurrentResistance == m_nDesiredResistance)
	{
		m_nSwitchTicks = 0;
		m_nSwitchCounter = 0;
		return;
	}

	if ((pCmd->buttons & IN_RELOAD))
	{
		m_nSwitchTicks = 8;
		return;
	}

	if (m_nSwitchTicks <= 0)
	{
		m_nSwitchCounter--;
		m_nSwitchTicks = 8;
		pCmd->buttons |= IN_RELOAD;
	}
	else
	{
		m_nSwitchTicks--;
	}
}

void CAutoVaccinator::CalculateBulletDangerFromPlayer(CBaseEntity* pLocal, CBaseEntity* pEntity, const PlayerInfo_t& pi)
{
	const auto& pPlayerWeapon = pEntity->GetActiveWeapon();

	if (!pPlayerWeapon)
	{
		return;
	}

	// Look at the function name...
	const EWeaponType playerWeaponType = Utils::GetWeaponType(pPlayerWeapon);
	if (playerWeaponType != EWeaponType::HITSCAN)
	{
		return;
	}
	
	const auto& vEntityPosition = pEntity->GetWorldSpaceCenter();

	// If the player isn't visible then why worry :-D
	if (Utils::VisPos(pLocal, pEntity, m_vLocalPosition, vEntityPosition))
	{
		m_nBulletDanger++;
	}
	else
	{
		return;
	}

	if (G::PlayerPriority[pi.friendsID].Mode == 4) // This player is a cheater
	{
		m_nBulletDanger += 5;
	}

	const int nPlayerClass = pEntity->GetClassNum();

	const float flDistanceToPlayer = m_vLocalPosition.DistTo(vEntityPosition);
	
	// TODO: Let the user change the minimum distance to be scared of these people

	// Scary ass
	if (flDistanceToPlayer < 500)
	{
		m_nBulletDanger++;
	}

	if (nPlayerClass == CLASS_SCOUT ||
		nPlayerClass == CLASS_HEAVY)
	{
		// Oh yeah you should be worried
		if (flDistanceToPlayer < 250)
		{
			m_nBulletDanger += 2;
		}
	}

	if (nPlayerClass == CLASS_SNIPER)
	{
		m_nBulletDanger++;
	}
}

void CAutoVaccinator::TestForArrows()
{
	for (const auto& Projectile : g_EntityCache.GetGroup(EGroupType::WORLD_PROJECTILES))
	{
		const ETFClassID nClassID = Projectile->GetClassID();

		if (nClassID != ETFClassID::CTFProjectile_Arrow)
		{
			continue;
		}

		const Vector& vPosition = Projectile->GetAbsOrigin();
		Vector vVelocity = Projectile->GetVelocity();

		// I don't wanna predict projectiles that are not moving
		if (vVelocity.Length2D() < 50)
		{
			continue;
		}

		const Vector vPredictedPosition = (vPosition + vVelocity.Scale(m_flPing / 1000.f));

#ifdef _DEBUG
		I::DebugOverlay->AddBoxOverlay(vPosition, { -1,-1,-1 }, { 1,1,1 }, { 0,0,0 }, 255, 0, 0, 255, 1.0f);
		I::DebugOverlay->AddBoxOverlay(vPredictedPosition, { -1,-1,-1 }, { 1,1,1 }, { 0,0,0 }, 0, 255, 0, 255, 1.0f);
#endif

		const float flDistanceToLocal = vPredictedPosition.DistTo(m_vLocalPosition);
		const float flDistanceToPatient = vPredictedPosition.DistTo(m_vHealingTargetPosition);

		float flShouldReactDistance = (vVelocity * (16 * I::GlobalVars->interval_per_tick)).Length2D();

		if (flDistanceToLocal < flShouldReactDistance || flDistanceToPatient < flShouldReactDistance)
		{
			I::DebugOverlay->AddBoxOverlay(m_vLocalPosition, { -1, -1, -1 }, { 1,1,1 }, { 0,0,0 }, 255, 0, 255, 0, 2.0f);
			m_nBulletDanger += 20;
		}	
	}
}

void CAutoVaccinator::FireGameEvent(FNV1A_t uNameHash, CGameEvent* pEvent)
{
}
