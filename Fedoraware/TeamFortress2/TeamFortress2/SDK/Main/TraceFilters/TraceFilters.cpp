#include "TraceFilters.h"

bool CTraceFilterHitscan::ShouldHitEntity(void *pEntityHandle, int nContentsMask)
{
	CBaseEntity *pEntity = reinterpret_cast<CBaseEntity *>(pEntityHandle);

	switch (pEntity->GetClientClass()->iClassID)
	{
		//wtf are these? =D
		case 55: //CFuncAreaPortalWindow
		case 64: //CFuncRespawnRoomVisualizer
		case 117: //CSniperDot
		case 225: //CTFKnife (This had something to do with medic's MvM shield, if I am not very fucking wrong)
		{
			return false;
		}
	}

	return (pEntityHandle != pSkip);
}

ETraceType CTraceFilterHitscan::GetTraceType() const
{
	return TRACE_EVERYTHING;
}

bool CTraceFilterWorldAndPropsOnly::ShouldHitEntity(void *pEntityHandle, int nContentsMask)
{
	CBaseEntity *pEntity = reinterpret_cast<CBaseEntity *>(pEntityHandle);

	switch (pEntity->GetClientClass()->iClassID)
	{
		case 55:
		case 64:
		case 117:
		case 225: {
			return false;
		}
	}

	switch (pEntity->GetClassID())
	{
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
		case ETFClassID::CZombie: {
			return false;
		}
	}

	return !(pEntityHandle == pSkip);
}

ETraceType CTraceFilterWorldAndPropsOnly::GetTraceType() const
{
	return TRACE_EVERYTHING;
}