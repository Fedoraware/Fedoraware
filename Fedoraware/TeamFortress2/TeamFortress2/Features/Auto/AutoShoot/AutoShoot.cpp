#include "AutoShoot.h"
#include "../../Vars.h"

bool CAutoShoot::IsAimingAtValidTarget(CBaseEntity* pLocal, CUserCmd* pCmd, float* pSimTime)
{
	CGameTrace Trace = {};
	Vec3 vForward = {};
	Math::AngleVectors(pCmd->viewangles, &vForward);
	Vec3 vFrom = pLocal->GetShootPos();
	Vec3 vTo = (vFrom + (vForward * 8192.0f));
	CTraceFilterHitscan Filter = {};
	Filter.pSkip = pLocal;
	Utils::Trace(vFrom, vTo, (MASK_SHOT | CONTENTS_GRATE), &Filter, &Trace);

	if (const auto& pEntity = Trace.entity)
	{
		switch (pEntity->GetClassID())
		{
		case ETFClassID::CTFPlayer:
		{
			if (!pEntity->IsAlive())
				return false;

			if (!Vars::Triggerbot::Shoot::TriggerPlayers.m_Var)
				return false;

			if (pEntity->GetTeamNum() == pLocal->GetTeamNum())
				return false;

			if (Vars::Triggerbot::Global::IgnoreInvlunerable.m_Var && !pEntity->IsVulnerable())
				return false;

			if (Vars::Triggerbot::Global::IgnoreCloaked.m_Var && pEntity->IsCloaked())
				return false;

			if (Vars::Triggerbot::Global::IgnoreFriends.m_Var && g_EntityCache.Friends[pEntity->GetIndex()])
				return false;

			if (Vars::Triggerbot::Shoot::HeadOnly.m_Var && g_GlobalInfo.m_bWeaponCanHeadShot && Trace.hitbox != HITBOX_HEAD)
				return false;

			if (Trace.hitbox == HITBOX_HEAD && Vars::Triggerbot::Shoot::HeadScale.m_Var < 1.0f)
			{
				Vec3 vMins = {}, vMaxs = {}, vCenter = {};
				matrix3x4 Matrix = {};

				if (!pEntity->GetHitboxMinsAndMaxsAndMatrix(HITBOX_HEAD, vMins, vMaxs, Matrix, &vCenter))
					return false;

				vMins *= Vars::Triggerbot::Shoot::HeadScale.m_Var;
				vMaxs *= Vars::Triggerbot::Shoot::HeadScale.m_Var;

				if (!Math::RayToOBB(vFrom, vForward, vCenter, vMins, vMaxs, Matrix))
					return false;
			}

			if (Vars::Misc::DisableInterpolation.m_Var)
				*pSimTime = pEntity->GetSimulationTime();

			break;
		}

		case ETFClassID::CObjectSentrygun:
		case ETFClassID::CObjectDispenser:
		case ETFClassID::CObjectTeleporter:
		{
			if (!pEntity->IsAlive())
				return false;

			if (!Vars::Triggerbot::Shoot::TriggerBuildings.m_Var)
				return false;

			if (pEntity->GetTeamNum() == pLocal->GetTeamNum())
				return false;

			break;
		}

		default: return false;
		}

		return true;
	}

	return false;
}

bool CAutoShoot::ShouldFire(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon)
{
	if (Vars::Triggerbot::Shoot::WaitForCharge.m_Var)
	{
		switch (pLocal->GetClassNum()) {
		case CLASS_SNIPER:
		{
			if (!g_GlobalInfo.m_bWeaponCanHeadShot && pLocal->IsScoped())
				return false;

			break;
		}

		case CLASS_SPY:
		{
			if (!g_GlobalInfo.m_bWeaponCanHeadShot)
			{
				if (g_GlobalInfo.m_nCurItemDefIndex == Spy_m_TheAmbassador || g_GlobalInfo.m_nCurItemDefIndex == Spy_m_FestiveAmbassador)
					return false;
			}

			break;
		}

		default: break;
		}
	}

	return true;
}

void CAutoShoot::Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	if (!Vars::Triggerbot::Shoot::Active.m_Var)
		return;

	if (Vars::Aimbot::Global::AutoShoot.m_Var && g_GlobalInfo.m_bHitscanRunning)
		return;

	if (!pLocal
		|| !pLocal->IsAlive()
		|| pLocal->IsTaunting()
		|| pLocal->IsBonked()
		|| pLocal->IsAGhost()
		|| pLocal->IsInBumperKart())
		return;

	if (g_GlobalInfo.m_WeaponType != EWeaponType::HITSCAN)
		return;

	float fSimTime = 0.0f;

	if (IsAimingAtValidTarget(pLocal, pCmd, &fSimTime) && ShouldFire(pLocal, pWeapon))
	{
		pCmd->buttons |= IN_ATTACK;

		if (g_GlobalInfo.m_bWeaponCanAttack)
			g_GlobalInfo.m_bAttacking = true;

		if (fSimTime && Vars::Misc::DisableInterpolation.m_Var && g_GlobalInfo.m_bWeaponCanAttack)
		{
			pCmd->tick_count = TIME_TO_TICKS(fSimTime
				+ std::max(g_ConVars.cl_interp->GetFloat(), g_ConVars.cl_interp_ratio->GetFloat() / g_ConVars.cl_updaterate->GetFloat()));
		}
	}
}