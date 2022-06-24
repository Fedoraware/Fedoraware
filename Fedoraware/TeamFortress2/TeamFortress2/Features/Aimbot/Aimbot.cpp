#include "Aimbot.h"
#include "../Vars.h"

#include "AimbotHitscan/AimbotHitscan.h"
#include "AimbotProjectile/AimbotProjectile.h"
#include "AimbotMelee/AimbotMelee.h"

bool CAimbot::ShouldRun(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon)
{
	if (G::FreecamActive)
		return false;

	if (!Vars::Aimbot::Global::Active.Value)
		return false;

	if (I::EngineVGui->IsGameUIVisible() || I::Surface->IsCursorVisible())
		return false;

	if (!pLocal->IsAlive()
		|| pLocal->IsTaunting()
		|| pLocal->IsBonked()
		|| pLocal->GetFeignDeathReady()
		|| pLocal->IsCloaked()
		|| pLocal->IsInBumperKart()
		|| pLocal->IsAGhost())
		return false;

	switch (G::CurItemDefIndex)
	{
	case Soldier_m_RocketJumper:
	case Demoman_s_StickyJumper: return false;
	default: break;
	}

	switch (pWeapon->GetWeaponID())
	{
	case TF_WEAPON_PDA:
	case TF_WEAPON_PDA_ENGINEER_BUILD:
	case TF_WEAPON_PDA_ENGINEER_DESTROY:
	case TF_WEAPON_PDA_SPY:
	case TF_WEAPON_PDA_SPY_BUILD:
	case TF_WEAPON_BUILDER:
	case TF_WEAPON_INVIS:
	case TF_WEAPON_BUFF_ITEM:
	case TF_WEAPON_GRAPPLINGHOOK:
		{
			return false;
		}

	default: break;
	}

	return true;
}

void CAimbot::Run(CUserCmd* pCmd)
{
	G::CurrentTargetIdx = 0;
	G::PredictedPos = Vec3();
	G::HitscanRunning = false;
	G::HitscanSilentActive = false;
	G::ProjectileSilentActive = false;
	G::AimPos = Vec3();

	auto pLocal = I::EntityList->GetClientEntity(I::Engine->GetLocalPlayer());

	if (pLocal)
	{
		auto pWeapon = pLocal->GetActiveWeapon();

		if (!pWeapon)
		{
			return;
		}

		if (!ShouldRun(pLocal, pWeapon))
			return;

		SandvichAimbot::IsSandvich();
		if (SandvichAimbot::bIsSandvich) {
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
}
