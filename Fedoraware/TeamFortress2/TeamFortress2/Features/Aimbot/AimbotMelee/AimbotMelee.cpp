#include "AimbotMelee.h"
#include "../../Vars.h"

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
		if (!Vars::Aimbot::Melee::PredictSwing.Value || pWeapon->GetWeaponID() == TF_WEAPON_KNIFE || pLocal->IsCharging())
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
	switch (Vars::Aimbot::Melee::SortMethod.Value)
	{
	case 0: return ESortMethod::FOV;
	case 1: return ESortMethod::DISTANCE;
	default: return ESortMethod::UNKNOWN;
	}
}

bool CAimbotMelee::GetTargets(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon)
{
	const ESortMethod sortMethod = GetSortMethod();

	F::AimbotGlobal.m_vecTargets.clear();

	const Vec3 vLocalPos = pLocal->GetShootPos();
	const Vec3 vLocalAngles = I::Engine->GetViewAngles();

	// Players
	if (Vars::Aimbot::Global::AimPlayers.Value)
	{
		const bool bWhipTeam = (pWeapon->GetItemDefIndex() == Soldier_t_TheDisciplinaryAction &&
			Vars::Aimbot::Melee::WhipTeam.Value);

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

			if (F::AimbotGlobal.ShouldIgnore(pTarget, true)) { continue; }

			Vec3 vPos = pTarget->GetHitboxPos(HITBOX_PELVIS);
			Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
			const float flFOVTo = Math::CalcFov(vLocalAngles, vAngleTo);
			
			if ((sortMethod == ESortMethod::FOV || Vars::Aimbot::Melee::RespectFOV.Value) && flFOVTo > Vars::Aimbot::Global::AimFOV.Value)
			{
				continue;
			}
			const float flDistTo = sortMethod == ESortMethod::DISTANCE ? vLocalPos.DistTo(vPos) : 0.0f;

			const uint32_t priorityID = g_EntityCache.GetPR()->GetValid(pTarget->GetIndex()) ? g_EntityCache.GetPR()->GetAccountID(pTarget->GetIndex()) : 0;
			const auto& priority = G::PlayerPriority[priorityID];

			F::AimbotGlobal.m_vecTargets.push_back({ pTarget, ETargetType::PLAYER, vPos, vAngleTo, flFOVTo, flDistTo, -1, false, priority });
		}
	}
	
	// Buildings
	if (Vars::Aimbot::Global::AimBuildings.Value)
	{
		for (const auto& pBuilding : g_EntityCache.GetGroup(EGroupType::BUILDINGS_ENEMIES))
		{
			if (!pBuilding->IsAlive())
			{
				continue;
			}

			Vec3 vPos = pBuilding->GetWorldSpaceCenter();
			Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
			const float flFOVTo = Math::CalcFov(vLocalAngles, vAngleTo);

			if ((sortMethod == ESortMethod::FOV || Vars::Aimbot::Melee::RespectFOV.Value) && flFOVTo > Vars::Aimbot::Global::AimFOV.Value)
			{
				continue;
			}
			const float flDistTo = sortMethod == ESortMethod::DISTANCE ? vLocalPos.DistTo(vPos) : 0.0f;

			F::AimbotGlobal.m_vecTargets.push_back({pBuilding, ETargetType::BUILDING, vPos, vAngleTo, flFOVTo, flDistTo});
		}
	}

	return !F::AimbotGlobal.m_vecTargets.empty();
}

bool CAimbotMelee::VerifyTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, const Target_t& target)
{
	if (Vars::Aimbot::Melee::RangeCheck.Value
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

	F::AimbotGlobal.SortTargets(GetSortMethod());

	for (auto& target : F::AimbotGlobal.m_vecTargets)
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
	vAngle -= G::PunchAngles;
	Math::ClampAngles(vAngle);

	switch (Vars::Aimbot::Melee::AimMethod.Value)
	{
	case 0:
		{
			pCmd->viewangles = vAngle;
			I::Engine->SetViewAngles(pCmd->viewangles);
			break;
		}

	case 1:
		{
			if (Vars::Aimbot::Melee::SmoothingAmount.Value == 0)
			{
				// plain aim at 0 smoothing factor
				pCmd->viewangles = vAngle;
				I::Engine->SetViewAngles(pCmd->viewangles);
				break;
			}

			Vec3 vecDelta = vAngle - pCmd->viewangles;
			Math::ClampAngles(vecDelta);
			pCmd->viewangles += vecDelta / Vars::Aimbot::Melee::SmoothingAmount.Value;
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
	if (!Vars::Aimbot::Global::AutoShoot.Value)
	{
		return false;
	}

	//There's a reason for running this even if range check is enabled (it calls this too), trust me :)
	if (!CanMeleeHit(pLocal, pWeapon,
	                 Vars::Aimbot::Melee::AimMethod.Value == 2
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
		return ((pCmd->buttons & IN_ATTACK) && G::WeaponCanAttack);
	}
	return fabs(pWeapon->GetSmackTime() - I::GlobalVars->curtime) < I::GlobalVars->interval_per_tick
		* 2.0f;
}

void CAimbotMelee::Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	if (!Vars::Aimbot::Global::Active.Value || G::AutoBackstabRunning || pWeapon->GetWeaponID() ==
		TF_WEAPON_KNIFE)
	{
		return;
	}

	Target_t target = {};

	const bool bShouldAim = (Vars::Aimbot::Global::AimKey.Value == VK_LBUTTON
		                         ? (pCmd->buttons & IN_ATTACK)
		                         : F::AimbotGlobal.IsKeyDown());

	if (GetTarget(pLocal, pWeapon, target) && bShouldAim)
	{
		G::CurrentTargetIdx = target.m_pEntity->GetIndex();

		if (Vars::Aimbot::Melee::AimMethod.Value == 2)
		{
			G::AimPos = target.m_vPos;
		}

		if (ShouldSwing(pLocal, pWeapon, pCmd, target))
		{
			pCmd->buttons |= IN_ATTACK;
		}
			
		if (Vars::Misc::CL_Move::Enabled.Value && Vars::Misc::CL_Move::Doubletap.Value && (pCmd->buttons & IN_ATTACK) && G::ShiftedTicks && !G::WaitForShift)
		{
			if ((Vars::Misc::CL_Move::DTMode.Value == 0 && GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.Value)) ||
				(Vars::Misc::CL_Move::DTMode.Value == 1) ||
				(Vars::Misc::CL_Move::DTMode.Value == 2 && !GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.Value)))
			{
				if ((Vars::Misc::CL_Move::NotInAir.Value && !pLocal->IsOnGround() && G::ShiftedTicks))
				{
					G::ShouldShift = false;
				}
				else
				{
					G::ShouldShift = true;
				}
			}
		}		

		const bool bIsAttacking = IsAttacking(pCmd, pWeapon);

		if (bIsAttacking)
		{
			G::IsAttacking = true;
		}

		if (Vars::Aimbot::Melee::AimMethod.Value == 2)
		{
			if (bIsAttacking)
			{
				Aim(pCmd, target.m_vAngleTo);
				G::SilentTime = true;
			}
		}

		else
		{
			Aim(pCmd, target.m_vAngleTo);
		}
	}
}
