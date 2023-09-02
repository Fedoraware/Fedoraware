#include "TraceFilters.h"

bool CTraceFilterHitscan::ShouldHitEntity(void* pEntityHandle, int nContentsMask)
{
	CBaseEntity* pEntity = reinterpret_cast<CBaseEntity*>(pEntityHandle);
	CBaseEntity* pLocal = reinterpret_cast<CBaseEntity*>(this->pSkip);

	switch (pEntity->GetClassID())
	{
		case ETFClassID::CFuncAreaPortalWindow:
		case ETFClassID::CFuncRespawnRoomVisualizer:
		case ETFClassID::CSniperDot:
		case ETFClassID::CTFMedigunShield:
		case ETFClassID::CTFReviveMarker:
		{
			return false;
		}
	}

	if (pLocal && pLocal->GetClassNum() == CLASS_SNIPER)
	{
		switch (pEntity->GetClassID())
		{
			case ETFClassID::CTFPlayer:
			case ETFClassID::CObjectSentrygun:
			case ETFClassID::CObjectDispenser:
			case ETFClassID::CObjectTeleporter:
			{
				if (pLocal->GetTeamNum() == pEntity->GetTeamNum())
					return false;
			} break;
			default:
				break;
		}
	}

	return (pEntityHandle != pSkip);
}

ETraceType CTraceFilterHitscan::GetTraceType() const
{
	return TRACE_EVERYTHING;
}

bool CTraceFilterWorldAndPropsOnly::ShouldHitEntity(void* pEntityHandle, int nContentsMask)
{
	CBaseEntity* pEntity = reinterpret_cast<CBaseEntity*>(pEntityHandle);

	switch (pEntity->GetClassID())
	{
		case ETFClassID::CFuncAreaPortalWindow:
		case ETFClassID::CFuncRespawnRoomVisualizer:
		case ETFClassID::CSniperDot:
		case ETFClassID::CTFMedigunShield:
		case ETFClassID::CTFReviveMarker:
		case ETFClassID::CTFAmmoPack:
		case ETFClassID::CTFProjectile_Arrow:
		case ETFClassID::CTFProjectile_BallOfFire:
		case ETFClassID::CTFProjectile_Cleaver:
		case ETFClassID::CTFProjectile_Jar:
		case ETFClassID::CTFProjectile_JarMilk:
		case ETFClassID::CTFProjectile_EnergyBall:
		case ETFClassID::CTFProjectile_EnergyRing:
		case ETFClassID::CTFProjectile_Flare:
		case ETFClassID::CTFProjectile_HealingBolt:
		case ETFClassID::CTFProjectile_MechanicalArmOrb:
		case ETFClassID::CTFProjectile_Rocket:
		case ETFClassID::CTFProjectile_SentryRocket:
		case ETFClassID::CTFGrenadePipebombProjectile:
		case ETFClassID::CTFPlayer:
		case ETFClassID::CMerasmus:
		case ETFClassID::CMerasmusDancer:
		case ETFClassID::CEyeballBoss:
		case ETFClassID::CHeadlessHatman:
		case ETFClassID::CZombie:
		{
			return false;
		} break;
		default:
			break;
	}

	return !(pEntityHandle == pSkip);
}

ETraceType CTraceFilterWorldAndPropsOnly::GetTraceType() const
{
	return TRACE_EVERYTHING;
}