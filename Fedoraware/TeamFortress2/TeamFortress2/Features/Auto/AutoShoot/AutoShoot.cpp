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

				if (g_AutoGlobal.ShouldIgnore(pEntity)) { return false; }

				if (Vars::Triggerbot::Shoot::HeadOnly.m_Var && G::WeaponCanHeadShot && Trace.hitbox !=
					HITBOX_HEAD)
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
		switch (pLocal->GetClassNum())
		{
		case CLASS_SNIPER:
			{
				if (!G::WeaponCanHeadShot && pLocal->IsScoped())
					return false;

				break;
			}

		case CLASS_SPY:
			{
				if (!G::WeaponCanHeadShot)
				{
					if (G::CurItemDefIndex == Spy_m_TheAmbassador || G::CurItemDefIndex ==
						Spy_m_FestiveAmbassador)
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

	if (Vars::Aimbot::Global::AutoShoot.m_Var && G::HitscanRunning)
		return;

	if (!pLocal
		|| !pLocal->IsAlive()
		|| pLocal->IsTaunting()
		|| pLocal->IsBonked()
		|| pLocal->IsAGhost()
		|| pLocal->IsInBumperKart())
		return;

	if (G::CurWeaponType != EWeaponType::HITSCAN)
		return;

	float fSimTime = 0.0f;

	if (IsAimingAtValidTarget(pLocal, pCmd, &fSimTime) && ShouldFire(pLocal, pWeapon))
	{
		pCmd->buttons |= IN_ATTACK;

		if (G::WeaponCanAttack)
			G::IsAttacking = true;

		if (fSimTime && Vars::Misc::DisableInterpolation.m_Var && G::WeaponCanAttack)
		{
			pCmd->tick_count = TIME_TO_TICKS(fSimTime
				+ std::max(g_ConVars.cl_interp->GetFloat(), g_ConVars.cl_interp_ratio->GetFloat() / g_ConVars.
					cl_updaterate->GetFloat()));
		}
	}
}
