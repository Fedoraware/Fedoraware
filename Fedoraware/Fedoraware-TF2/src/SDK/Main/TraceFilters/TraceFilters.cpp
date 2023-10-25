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
			return false;
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
			}
		}
	}

	return (pEntityHandle != pSkip);
}

ETraceType CTraceFilterHitscan::GetTraceType() const
{
	return TRACE_EVERYTHING;
}

bool CTraceFilterProjectile::ShouldHitEntity(void* pEntityHandle, int nContentsMask)
{
	CBaseEntity* pEntity = reinterpret_cast<CBaseEntity*>(pEntityHandle);

	switch (pEntity->GetClassID())
	{
		case ETFClassID::CFuncAreaPortalWindow:
		case ETFClassID::CFuncRespawnRoomVisualizer:
		case ETFClassID::CSniperDot:
		case ETFClassID::CTFReviveMarker:
		case ETFClassID::CTFProjectile_ThrowableBreadMonster:
		case ETFClassID::CTFProjectile_ThrowableBrick:
		case ETFClassID::CTFProjectile_ThrowableRepel:
		case ETFClassID::CTFProjectile_Throwable:
		case ETFClassID::CTFThrowable:
		case ETFClassID::CTFProjectile_MechanicalArmOrb:
		case ETFClassID::CTFProjectile_JarGas:
		case ETFClassID::CTFProjectile_Cleaver:
		case ETFClassID::CTFProjectile_JarMilk:
		case ETFClassID::CTFProjectile_Jar:
		case ETFClassID::CTFGrenadePipebombProjectile:
		case ETFClassID::CTFBall_Ornament:
		case ETFClassID::CTFStunBall:
		case ETFClassID::CTFProjectile_EnergyRing:
		case ETFClassID::CTFProjectile_Rocket:
		case ETFClassID::CTFProjectile_Flare:
		case ETFClassID::CTFProjectile_EnergyBall:
		case ETFClassID::CTFProjectile_GrapplingHook:
		case ETFClassID::CTFProjectile_HealingBolt:
		case ETFClassID::CTFProjectile_Arrow:
		case ETFClassID::CTFProjectile_SpellKartBats:
		case ETFClassID::CTFProjectile_SpellKartOrb:
		case ETFClassID::CTFProjectile_SpellLightningOrb:
		case ETFClassID::CTFProjectile_SpellTransposeTeleport:
		case ETFClassID::CTFProjectile_SpellMeteorShower:
		case ETFClassID::CTFProjectile_SpellSpawnBoss:
		case ETFClassID::CTFProjectile_SpellMirv:
		case ETFClassID::CTFProjectile_SpellPumpkin:
		case ETFClassID::CTFProjectile_SpellSpawnHorde:
		case ETFClassID::CTFProjectile_SpellSpawnZombie:
		case ETFClassID::CTFProjectile_SpellBats:
		case ETFClassID::CTFProjectile_SpellFireball:
		case ETFClassID::CTFProjectile_BallOfFire:
		case ETFClassID::CTFProjectile_SentryRocket:
			return false;
	}

	return pEntityHandle != pSkip;
}

ETraceType CTraceFilterProjectile::GetTraceType() const
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
			return false;
	}

	return pEntityHandle != pSkip;
}

ETraceType CTraceFilterWorldAndPropsOnly::GetTraceType() const
{
	return TRACE_EVERYTHING;
}