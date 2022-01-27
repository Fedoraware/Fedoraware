#include "Aimbot.h"
#include "../Vars.h"

#include "AimbotHitscan/AimbotHitscan.h"
#include "AimbotProjectile/AimbotProjectile.h"
#include "AimbotMelee/AimbotMelee.h"

bool CAimbot::ShouldRun(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon)
{
	if (!Vars::Aimbot::Global::Active.m_Var)
		return false;

	if (g_Interfaces.EngineVGui->IsGameUIVisible() || g_Interfaces.Surface->IsCursorVisible())
		return false;

	if (!pLocal->IsAlive()
		|| pLocal->IsTaunting()
		|| pLocal->IsBonked()
		|| pLocal->GetFeignDeathReady()
		|| pLocal->IsCloaked()
		|| pLocal->IsInBumperKart()
		|| pLocal->IsAGhost())
		return false;

	switch (g_GlobalInfo.m_nCurItemDefIndex) {
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
	case TF_WEAPON_LUNCHBOX:
	case TF_WEAPON_BUFF_ITEM:
	case TF_WEAPON_GRAPPLINGHOOK:
	case TF_WEAPON_MEDIGUN: {
		return false;
	}

	default: break;
	}

	return true;
}

void CAimbot::Run(CUserCmd* pCmd)
{
	g_GlobalInfo.m_nCurrentTargetIdx = 0;
	g_GlobalInfo.m_flCurAimFOV = 0.0f;
	g_GlobalInfo.m_vPredictedPos = Vec3();
	g_GlobalInfo.m_bHitscanRunning = false;
	g_GlobalInfo.m_bHitscanSilentActive = false;
	g_GlobalInfo.m_bProjectileSilentActive = false;
	g_GlobalInfo.m_vAimPos = Vec3();

	auto pLocal = g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetLocalPlayer());

	if (pLocal)
	{
		auto pWeapon = pLocal->GetActiveWeapon();
		g_Draw.String(FONT_MENU, g_ScreenSize.c, g_ScreenSize.h / 2, { 255,255,255,255 }, ALIGN_CENTERHORIZONTAL, ShouldRun(pLocal, pWeapon) ? "aimbot running" : "aimbot not running");
		if (!ShouldRun(pLocal, pWeapon))
			return;

		switch (g_GlobalInfo.m_WeaponType)
		{
		case EWeaponType::HITSCAN: {
			g_AimbotHitscan.Run(pLocal, pWeapon, pCmd);
			break;
		}

		case EWeaponType::PROJECTILE: {
			g_AimbotProjectile.Run(pLocal, pWeapon, pCmd);
			break;
		}

		case EWeaponType::MELEE: {
			g_AimbotMelee.Run(pLocal, pWeapon, pCmd);
			break;
		}

		default: break;
		}
	}
}