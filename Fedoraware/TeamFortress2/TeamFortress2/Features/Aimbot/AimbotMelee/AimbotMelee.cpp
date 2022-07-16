#include "AimbotMelee.h"
#include "../../Vars.h"

#include "../../Backtrack/Backtrack.h"

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
		const bool HasWrench = (pWeapon->GetWeaponID() == TF_WEAPON_WRENCH);

		auto AimFriendly = [](CBaseObject* Building) -> bool
		{
			int MaxAmmo = 0;
			int MaxRocket = 0;

			if (Building->GetLevel() != 3 || Building->GetSapped() || Building->GetHealth() < Building->GetMaxHealth())
				return true;

			if (Building->IsSentrygun())
			{
				switch (Building->GetLevel())
				{
				case 1:
				{
					MaxAmmo = 150;
					break;
				}
				case 2:
				{
					MaxAmmo = 200;
					break;
				}
				case 3:
				{
					MaxAmmo = 200;
					MaxRocket = 20;//Yeah?
					break;
				}
				}
			}

			if (Building->GetAmmo() < MaxAmmo || Building->GetRockets() < MaxRocket)
				return true;

			return false;
		};

		for (const auto& pBuilding : g_EntityCache.GetGroup(HasWrench ? EGroupType::BUILDINGS_ALL : EGroupType::BUILDINGS_ENEMIES))
		{
			const auto& Building = reinterpret_cast<CBaseObject*>(pBuilding);

			if (HasWrench && (Building->GetTeamNum() == pLocal->GetTeamNum()))
			{
				if (!AimFriendly(Building))
					continue;
			}

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

bool CAimbotMelee::VerifyTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, Target_t& target)
{
	Vec3 hitboxpos;

	// Backtrack the target if required
	if (Vars::Backtrack::Enabled.Value && Vars::Backtrack::Aim.Value)
	{
		if (const auto& pRecord = F::Backtrack.GetPlayerRecords(target.m_pEntity))
		{
			const auto& pLastTick = pRecord->back();
			if (const auto& pHDR = pLastTick.HDR)
			{
				if (const auto& pSet = pHDR->GetHitboxSet(pLastTick.HitboxSet))
				{
					if (const auto& pBox = pSet->hitbox(HITBOX_PELVIS))
					{
						const Vec3 vPos = (pBox->bbmin + pBox->bbmax) * 0.5f;
						Vec3 vOut;
						const matrix3x4& bone = pLastTick.BoneMatrix.BoneMatrix[pBox->bone];
						Math::VectorTransform(vPos, bone, vOut);
						hitboxpos = vOut;
						target.SimTime = pLastTick.SimulationTime;
					}
				}
			}
		}

		if (Utils::VisPos(pLocal, target.m_pEntity, pLocal->GetShootPos(), hitboxpos))
		{
			target.m_vAngleTo = Math::CalcAngle(pLocal->GetShootPos(), hitboxpos);
			target.m_vPos = hitboxpos;
			target.ShouldBacktrack = true;
			return true;
		}

		target.ShouldBacktrack = false;
		if (Vars::Backtrack::Latency.Value > 200)
		{
			return false;
		}
	}
	else
	{
		target.ShouldBacktrack = false;
	}
	
	if (Vars::Aimbot::Melee::RangeCheck.Value && !(Vars::Backtrack::Enabled.Value && Vars::Backtrack::Aim.Value))
	{
		if (!CanMeleeHit(pLocal, pWeapon, target.m_vAngleTo, target.m_pEntity->GetIndex()))
		{
			return false;
		}
	}
	else
	{
		const float flRange = (pWeapon->GetSwingRange(pLocal));
		if (hitboxpos.DistTo(target.m_vPos) < flRange)
		{
			if (!Utils::VisPos(pLocal, target.m_pEntity, pLocal->GetShootPos(), target.m_vPos))
			{
				return false;
			}
		}
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

	if (Vars::Backtrack::Enabled.Value && Vars::Backtrack::Aim.Value)
	{
		const float flRange = pWeapon->GetSwingRange(pLocal);

		if (Target.m_vPos.DistTo(pLocal->GetShootPos()) > flRange * 1.9f) // It just works?
		{
			//I::DebugOverlay->AddLineOverlay(Target.m_vPos, pLocal->GetShootPos(), 255, 0, 0, false, 1.f);
			return false;
		}
		else
		{
			/*I::DebugOverlay->AddLineOverlay(Target.m_vPos, pLocal->GetShootPos(), 0, 255, 0, false, 1.f);*/
			return true;
		}
	}

	//There's a reason for running this even if range check is enabled (it calls this too), trust me :)
	if (!CanMeleeHit(pLocal, pWeapon, Vars::Aimbot::Melee::AimMethod.Value == 2 ? Target.m_vAngleTo : I::Engine->GetViewAngles(), Target.m_pEntity->GetIndex()))
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
	return fabs(pWeapon->GetSmackTime() - I::GlobalVars->curtime) < I::GlobalVars->interval_per_tick * 2.0f;
}

void CAimbotMelee::Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	if (!Vars::Aimbot::Global::Active.Value || G::AutoBackstabRunning || pWeapon->GetWeaponID() == TF_WEAPON_KNIFE)
	{
		return;
	}

	Target_t target = {};

	const bool bShouldAim = (Vars::Aimbot::Global::AimKey.Value == VK_LBUTTON ? (pCmd->buttons & IN_ATTACK) : F::AimbotGlobal.IsKeyDown());

	if (GetTarget(pLocal, pWeapon, target) && bShouldAim)
	{
		G::CurrentTargetIdx = target.m_pEntity->GetIndex();

		if (Vars::Aimbot::Melee::AimMethod.Value == 2)
		{
			G::AimPos = target.m_vPos;
		}

		// Early swing prediction
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

		// Set the target tickcount (Backtrack)
		if (bIsAttacking)
		{
			const float simTime = target.ShouldBacktrack ? target.SimTime : target.m_pEntity->GetSimulationTime();
			pCmd->tick_count = TIME_TO_TICKS(simTime + G::LerpTime);
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
