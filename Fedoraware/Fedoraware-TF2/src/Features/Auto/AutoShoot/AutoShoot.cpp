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

				if (pEntity->GetDormant())
					return false;

				if (!Vars::Triggerbot::Shoot::TriggerPlayers.Value)
					return false;

				if (pEntity->GetTeamNum() == pLocal->GetTeamNum())
					return false;

				if (F::AutoGlobal.ShouldIgnore(pEntity)) { return false; }

				if (Vars::Triggerbot::Shoot::HeadOnly.Value && G::WeaponCanHeadShot && Trace.hitbox !=
					HITBOX_HEAD)
					return false;

				if (Trace.hitbox == HITBOX_HEAD && Vars::Triggerbot::Shoot::HeadScale.Value < 1.0f)
				{
					Vec3 vMins = {}, vMaxs = {}, vCenter = {};
					matrix3x4 Matrix = {};

					if (!pEntity->GetHitboxMinsAndMaxsAndMatrix(HITBOX_HEAD, vMins, vMaxs, Matrix, &vCenter))
						return false;

					vMins *= Vars::Triggerbot::Shoot::HeadScale.Value;
					vMaxs *= Vars::Triggerbot::Shoot::HeadScale.Value;

					if (!Math::RayToOBB(vFrom, vForward, vCenter, vMins, vMaxs, Matrix))
						return false;
				}

				if (Vars::Misc::DisableInterpolation.Value)
					*pSimTime = pEntity->GetSimulationTime();

				break;
			}

			case ETFClassID::CObjectSentrygun:
			case ETFClassID::CObjectDispenser:
			case ETFClassID::CObjectTeleporter:
			{
				if (!pEntity->IsAlive())
					return false;

				if (!Vars::Triggerbot::Shoot::TriggerBuildings.Value)
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
	switch (G::CurItemDefIndex)
	{
		case Sniper_m_TheMachina:
		case Sniper_m_ShootingStar:
		{
			if (!pLocal->IsScoped())
				return false;

			break;
		}
		default: break;
	}

	switch (pLocal->GetClassNum())
	{
		case CLASS_SNIPER:
		{
			if (Vars::Triggerbot::Shoot::WaitForHeadshot.Value)
			{
				if (!G::WeaponCanHeadShot && pLocal->IsScoped())
					return false;
			}

			if (!pLocal->IsScoped() && Vars::Triggerbot::Shoot::ScopeOnly.Value)
				return false;

			break;
		}
		case CLASS_SPY:
		{
			if (!G::WeaponCanHeadShot && Vars::Triggerbot::Shoot::WaitForHeadshot.Value)
			{
				if (G::CurItemDefIndex == Spy_m_TheAmbassador || G::CurItemDefIndex ==
					Spy_m_FestiveAmbassador)
					return false;
			}

			break;
		}
		default: break;
	}

	return true;
}

void CAutoShoot::Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	if (!Vars::Triggerbot::Shoot::Active.Value)
		return;

	if (Vars::Aimbot::Global::AutoShoot.Value && G::HitscanRunning)
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

		if (fSimTime && Vars::Misc::DisableInterpolation.Value && G::WeaponCanAttack)
		{
			pCmd->tick_count = TIME_TO_TICKS(fSimTime + G::LerpTime);
		}
	}
}
