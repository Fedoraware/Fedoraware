#include "Aimbot.h"
#include "../Vars.h"

#include "AimbotHitscan/AimbotHitscan.h"
#include "AimbotProjectile/AimbotProjectile.h"
#include "AimbotMelee/AimbotMelee.h"
#include "../Misc/Misc.h"

bool CAimbot::ShouldRun(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon)
{
	// Don't run while freecam is active
	if (G::FreecamActive) { return false; }

	// Don't run if aimbot is disabled
	if (!Vars::Aimbot::Global::Active.Value) { return false; }

	// Don't run in menus
	if (I::EngineVGui->IsGameUIVisible() || I::VGuiSurface->IsCursorVisible()) { return false; }

	// Don't run if we are frozen in place.
	if (G::Frozen) { return false; }

	if (!pLocal->IsAlive()
		|| pLocal->IsTaunting()
		|| pLocal->IsBonked()
		|| pLocal->GetFeignDeathReady()
		|| pLocal->IsCloaked()
		|| pLocal->IsInBumperKart()
		|| pLocal->IsAGhost())
	{
		return false;
	}

	//	0 damage weapons that we still want to aimbot with
	if (pWeapon->GetWeaponID() == TF_WEAPON_BUILDER) {
		return true;
	}

	//	weapon data check for null damage
	if (CTFWeaponInfo* sWeaponInfo = pWeapon->GetTFWeaponInfo())
	{
		WeaponData_t sWeaponData = sWeaponInfo->m_WeaponData[0];
		if (sWeaponData.m_nDamage < 1)
		{
			return false;
		}
	}

	return true;
}

void CAimbot::Run(CUserCmd* pCmd)
{
	//G::CurrentTargetIdx = 0;
	G::PredictedPos = Vec3();
	G::HitscanRunning = false;
	G::HitscanSilentActive = false;
	G::AimPos = Vec3();

	if (F::Misc.bMovementStopped || F::Misc.bFastAccel) { return; }

	const auto pLocal = g_EntityCache.GetLocal();
	const auto pWeapon = g_EntityCache.GetWeapon();
	if (!pLocal || !pWeapon) { return; }

	if (!ShouldRun(pLocal, pWeapon)) { return; }

	if (SandvichAimbot::bIsSandvich = SandvichAimbot::IsSandvich())
	{
		G::CurWeaponType = EWeaponType::HITSCAN;
	}

	switch (G::CurWeaponType)
	{
		case EWeaponType::HITSCAN:
		{
			F::AimbotHitscan.Run(pLocal, pWeapon, pCmd);
			break;
		}

		case EWeaponType::PROJECTILE:
		{
			F::AimbotProjectile.Run(pLocal, pWeapon, pCmd);
			break;
		}

		case EWeaponType::MELEE:
		{
			F::AimbotMelee.Run(pLocal, pWeapon, pCmd);
			break;
		}

		default: break;
	}
}
