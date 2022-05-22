#include "AimbotMelee.h"
#include "../../Vars.h"
#include "../../PlayerResource/PlayerResource.h"

bool CAimbotMelee::CanMeleeHit(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, const Vec3& vecViewAngles,
                               int nTargetIndex)
{
	static Vec3 vecSwingMins = {-18.0f, -18.0f, -18.0f};
	static Vec3 vecSwingMaxs = {18.0f, 18.0f, 18.0f};

	const float flRange = (pWeapon->GetSwingRange(pLocal));

	if (flRange <= 0.0f)
	{
		return false;
	}

	auto vecForward = Vec3();
	Math::AngleVectors(vecViewAngles, &vecForward);

	Vec3 vecTraceStart = pLocal->GetShootPos();
	const Vec3 vecTraceEnd = (vecTraceStart + (vecForward * flRange));

	CTraceFilterHitscan filter;
	filter.pSkip = pLocal;
	CGameTrace trace;
	Utils::TraceHull(vecTraceStart, vecTraceEnd, vecSwingMins, vecSwingMaxs, MASK_SHOT, &filter, &trace);

	if (!(trace.entity && trace.entity->GetIndex() == nTargetIndex))
	{
		if (!Vars::Aimbot::Melee::PredictSwing.m_Var || pWeapon->GetWeaponID() == TF_WEAPON_KNIFE || pLocal->
			IsCharging())
		{
			return false;
		}

		static constexpr float FL_DELAY = 0.2f; //it just works

		if (pLocal->IsOnGround())
		{
			vecTraceStart += (pLocal->GetVelocity() * FL_DELAY);
		}

		else
		{
			vecTraceStart += (pLocal->GetVelocity() * FL_DELAY) - (Vec3(0.0f, 0.0f, g_ConVars.sv_gravity->GetFloat()) *
				0.5f * FL_DELAY * FL_DELAY);
		}

		Vec3 vecTraceEnd = vecTraceStart + (vecForward * flRange);
		Utils::TraceHull(vecTraceStart, vecTraceEnd, vecSwingMins, vecSwingMaxs, MASK_SHOT, &filter, &trace);

		return (trace.entity && trace.entity->GetIndex() == nTargetIndex);
	}

	return true;
}

ESortMethod CAimbotMelee::GetSortMethod()
{
	switch (Vars::Aimbot::Melee::SortMethod.m_Var)
	{
	case 0: return ESortMethod::FOV;
	case 1: return ESortMethod::DISTANCE;
	default: return ESortMethod::UNKNOWN;
	}
}

bool CAimbotMelee::GetTargets(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon)
{
	const ESortMethod sortMethod = GetSortMethod();

	if (sortMethod == ESortMethod::FOV)
	{
		g_GlobalInfo.m_flCurAimFOV = Vars::Aimbot::Global::AimFOV.m_Var;
	}

	g_AimbotGlobal.m_vecTargets.clear();

	const Vec3 vLocalPos = pLocal->GetShootPos();
	const Vec3 vLocalAngles = I::Engine->GetViewAngles();

	// Players
	if (Vars::Aimbot::Global::AimPlayers.m_Var)
	{
		const bool bWhipTeam = (pWeapon->GetItemDefIndex() == Soldier_t_TheDisciplinaryAction &&
			Vars::Aimbot::Melee::WhipTeam.m_Var);

		for (const auto& pTarget : g_EntityCache.GetGroup(
			     bWhipTeam ? EGroupType::PLAYERS_ALL : EGroupType::PLAYERS_ENEMIES))
		{
			if (!pTarget->IsAlive() || pTarget->IsAGhost())
			{
				continue;
			}

			if (pTarget == pLocal)
			{
				continue;
			}

			if (g_AimbotGlobal.ShouldIgnore(pTarget)) { continue; }

			Vec3 vPos = pTarget->GetHitboxPos(HITBOX_PELVIS);
			Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
			const float flFOVTo = sortMethod == ESortMethod::FOV ? Math::CalcFov(vLocalAngles, vAngleTo) : 0.0f;
			const float flDistTo = sortMethod == ESortMethod::DISTANCE ? vLocalPos.DistTo(vPos) : 0.0f;

			if (sortMethod == ESortMethod::FOV && flFOVTo > Vars::Aimbot::Global::AimFOV.m_Var)
			{
				continue;
			}

			const uint32_t priorityID = g_PR->isValid(pTarget->GetIndex()) ? g_PR->GetAccountID(pTarget->GetIndex()) : 0;
			const auto& priority = g_GlobalInfo.PlayerPriority[priorityID];

			g_AimbotGlobal.m_vecTargets.push_back({ pTarget, ETargetType::PLAYER, vPos, vAngleTo, flFOVTo, flDistTo, -1, false, priority });
		}
	}
	
	// Buildings
	if (Vars::Aimbot::Global::AimBuildings.m_Var)
	{
		for (const auto& pBuilding : g_EntityCache.GetGroup(EGroupType::BUILDINGS_ENEMIES))
		{
			if (!pBuilding->IsAlive())
			{
				continue;
			}

			Vec3 vPos = pBuilding->GetWorldSpaceCenter();
			Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
			const float flFOVTo = sortMethod == ESortMethod::FOV ? Math::CalcFov(vLocalAngles, vAngleTo) : 0.0f;
			const float flDistTo = sortMethod == ESortMethod::DISTANCE ? vLocalPos.DistTo(vPos) : 0.0f;

			if (sortMethod == ESortMethod::FOV && flFOVTo > Vars::Aimbot::Global::AimFOV.m_Var)
			{
				continue;
			}

			g_AimbotGlobal.m_vecTargets.push_back({pBuilding, ETargetType::BUILDING, vPos, vAngleTo, flFOVTo, flDistTo});
		}
	}

	return !g_AimbotGlobal.m_vecTargets.empty();
}

bool CAimbotMelee::VerifyTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, const Target_t& target)
{
	if (Vars::Aimbot::Melee::RangeCheck.m_Var
		    ? !CanMeleeHit(pLocal, pWeapon, target.m_vAngleTo, target.m_pEntity->GetIndex())
		    : !Utils::VisPos(pLocal, target.m_pEntity, pLocal->GetShootPos(), target.m_vPos))
	{
		return false;
	}

	return true;
}

bool CAimbotMelee::GetTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, Target_t& Out)
{
	if (!GetTargets(pLocal, pWeapon))
	{
		return false;
	}

	g_AimbotGlobal.SortTargets(GetSortMethod());

	for (auto& target : g_AimbotGlobal.m_vecTargets)
	{
		if (!VerifyTarget(pLocal, pWeapon, target))
		{
			continue;
		}

		Out = target;
		return true;
	}

	return false;
}

void CAimbotMelee::Aim(CUserCmd* pCmd, Vec3& vAngle)
{
	vAngle -= g_GlobalInfo.m_vPunchAngles;
	Math::ClampAngles(vAngle);

	switch (Vars::Aimbot::Melee::AimMethod.m_Var)
	{
	case 0:
		{
			pCmd->viewangles = vAngle;
			I::Engine->SetViewAngles(pCmd->viewangles);
			break;
		}

	case 1:
		{
			if (Vars::Aimbot::Melee::SmoothingAmount.m_Var == 0)
			{
				// plain aim at 0 smoothing factor
				pCmd->viewangles = vAngle;
				I::Engine->SetViewAngles(pCmd->viewangles);
				break;
			}

			Vec3 vecDelta = vAngle - pCmd->viewangles;
			Math::ClampAngles(vecDelta);
			pCmd->viewangles += vecDelta / Vars::Aimbot::Melee::SmoothingAmount.m_Var;
			I::Engine->SetViewAngles(pCmd->viewangles);
			break;
		}

	case 2:
		{
			Utils::FixMovement(pCmd, vAngle);
			pCmd->viewangles = vAngle;
			break;
		}

	default: break;
	}
}

bool CAimbotMelee::ShouldSwing(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, const Target_t& Target)
{
	if (!Vars::Aimbot::Global::AutoShoot.m_Var)
	{
		return false;
	}

	//There's a reason for running this even if range check is enabled (it calls this too), trust me :)
	if (!CanMeleeHit(pLocal, pWeapon,
	                 Vars::Aimbot::Melee::AimMethod.m_Var == 2
		                 ? Target.m_vAngleTo
		                 : I::Engine->GetViewAngles(), Target.m_pEntity->GetIndex()))
	{
		return false;
	}

	return true;
}

bool CAimbotMelee::IsAttacking(CUserCmd* pCmd, CBaseCombatWeapon* pWeapon)
{
	if (pWeapon->GetWeaponID() == TF_WEAPON_KNIFE)
	{
		return ((pCmd->buttons & IN_ATTACK) && g_GlobalInfo.m_bWeaponCanAttack);
	}
	return fabs(pWeapon->GetSmackTime() - I::GlobalVars->curtime) < I::GlobalVars->interval_per_tick
		* 2.0f;
}

void CAimbotMelee::Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	if (!Vars::Aimbot::Global::Active.m_Var || g_GlobalInfo.m_bAutoBackstabRunning || pWeapon->GetWeaponID() ==
		TF_WEAPON_KNIFE)
	{
		return;
	}

	Target_t target = {};

	const bool bShouldAim = (Vars::Aimbot::Global::AimKey.m_Var == VK_LBUTTON
		                         ? (pCmd->buttons & IN_ATTACK)
		                         : g_AimbotGlobal.IsKeyDown());

	if (GetTarget(pLocal, pWeapon, target) && bShouldAim)
	{
		g_GlobalInfo.m_nCurrentTargetIdx = target.m_pEntity->GetIndex();

		if (Vars::Aimbot::Melee::AimMethod.m_Var == 2)
		{
			g_GlobalInfo.m_vAimPos = target.m_vPos;
		}

		if (ShouldSwing(pLocal, pWeapon, pCmd, target))
		{
			pCmd->buttons |= IN_ATTACK;
		}

		if (Vars::Misc::CL_Move::Enabled.m_Var && Vars::Misc::CL_Move::Doubletap.m_Var && (pCmd->buttons & IN_ATTACK) && g_GlobalInfo.m_nShifted && !g_GlobalInfo.m_nWaitForShift)
		{
			if ((Vars::Misc::CL_Move::DTMode.m_Var == 0 && GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)) ||
				(Vars::Misc::CL_Move::DTMode.m_Var == 1) ||
				(Vars::Misc::CL_Move::DTMode.m_Var == 2 && !GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)))
			{
				if ((Vars::Misc::CL_Move::NotInAir.m_Var && !pLocal->IsOnGround() && g_GlobalInfo.m_nShifted))
				{
					g_GlobalInfo.m_bShouldShift = false;
				}
				else
				{
					g_GlobalInfo.m_bShouldShift = true;
				}
			}
		}

		const bool bIsAttacking = IsAttacking(pCmd, pWeapon);

		if (bIsAttacking)
		{
			g_GlobalInfo.m_bAttacking = true;
		}

		if (Vars::Aimbot::Melee::AimMethod.m_Var == 2)
		{
			if (bIsAttacking)
			{
				Aim(pCmd, target.m_vAngleTo);
				g_GlobalInfo.m_bSilentTime = true;
			}
		}

		else
		{
			Aim(pCmd, target.m_vAngleTo);
		}
	}
}
