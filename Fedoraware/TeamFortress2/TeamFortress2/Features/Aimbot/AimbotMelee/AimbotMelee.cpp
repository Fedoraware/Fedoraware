#include "AimbotMelee.h"
#include "../../Vars.h"
#include "../MovementSimulation/MovementSimulation.h"
#include "../../TickHandler/TickHandler.h"
#include "../../Backtrack/Backtrack.h"

bool CAimbotMelee::CanMeleeHit(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, const Vec3& vecViewAngles,
							   int nTargetIndex)
{
	static Vec3 vecSwingMins = { -18.0f, -18.0f, -18.0f };
	static Vec3 vecSwingMaxs = { 18.0f, 18.0f, 18.0f };

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

		const float FL_DELAY = std::max(pWeapon->GetWeaponData().m_flSmackDelay - ((F::Ticks.MeleeDoubletapCheck(pLocal) && Vars::Misc::CL_Move::AntiWarp.Value) ? TICKS_TO_TIME(G::ShiftedTicks) : 0.f), 0.f);
		
		if (FL_DELAY == 0.f) { return false; }
		
		const int iTicks = TIME_TO_TICKS(FL_DELAY);

		if (!bCached){
			if (F::MoveSim.Initialize(g_EntityCache.GetLocal()))
			{
				CMoveData moveData = {};
				Vec3 absOrigin = {};
				for (int i = 0; i < iTicks; i++){
					F::MoveSim.RunTick(moveData, absOrigin);
				}
				vecTraceStart = absOrigin;
				vecTraceStart.z += 64;	//	viewheight bcs lazy
				bCached = true;
				vCached = vecTraceStart;
				F::MoveSim.Restore();
			}
			else { return false; }
		}
		else{
			vecTraceStart = vCached;
		}

		CBaseEntity* pTarget = I::ClientEntityList->GetClientEntity(nTargetIndex);
		if (!pTarget) { return false; }

		if (pTarget->GetVelocity().Length() < 10.f || !pTarget->IsPlayer()){
			Vec3 vecTraceEnd = vecTraceStart + (vecForward * flRange);
			Utils::TraceHull(vecTraceStart, vecTraceEnd, vecSwingMins, vecSwingMaxs, MASK_SHOT, &filter, &trace);
			return (trace.entity && trace.entity->GetIndex() == nTargetIndex);
		}

		if (F::MoveSim.Initialize(pTarget)){
			CMoveData moveData = {};
			Vec3 absOrigin = {};
			for (int i = 0; i < iTicks; i++){
				F::MoveSim.RunTick(moveData, absOrigin);
			}

			const Vec3 vRestore = pTarget->GetAbsOrigin();
			pTarget->SetAbsOrigin(absOrigin);

			Vec3 vecTraceEnd = vecTraceStart + (vecForward * flRange);
			Utils::TraceHull(vecTraceStart, vecTraceEnd, vecSwingMins, vecSwingMaxs, MASK_SHOT, &filter, &trace);
			const bool bReturn = (trace.entity && trace.entity->GetIndex() == nTargetIndex);

			pTarget->SetAbsOrigin(vRestore);
			F::MoveSim.Restore();
			return bReturn;
		}

		return false;
	}

	return true;
}

EGroupType CAimbotMelee::GetGroupType(CBaseCombatWeapon* pWeapon)
{
	if (Vars::Aimbot::Melee::WhipTeam.Value && pWeapon->GetItemDefIndex() == Soldier_t_TheDisciplinaryAction)
	{
		return EGroupType::PLAYERS_ALL;
	}

	return EGroupType::PLAYERS_ENEMIES;
}

/* Aim at friendly building with the wrench */
bool CAimbotMelee::AimFriendlyBuilding(CBaseObject* pBuilding)
{
	int maxAmmo = 0;
	int maxRocket = 0;

	if (pBuilding->GetLevel() != 3 || pBuilding->GetSapped() || pBuilding->GetHealth() < pBuilding->GetMaxHealth())
	{
		return true;
	}

	if (pBuilding->IsSentrygun())
	{
		switch (pBuilding->GetLevel())
		{
			case 1:
			{
				maxAmmo = 150;
				break;
			}
			case 2:
			{
				maxAmmo = 200;
				break;
			}
			case 3:
			{
				maxAmmo = 200;
				maxRocket = 20; //Yeah?
				break;
			}
		}
	}

	if (pBuilding->GetAmmo() < maxAmmo || pBuilding->GetRockets() < maxRocket)
	{
		return true;
	}

	return false;
}

std::vector<Target_t> CAimbotMelee::GetTargets(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon)
{
	std::vector<Target_t> validTargets;
	const auto sortMethod = static_cast<ESortMethod>(Vars::Aimbot::Melee::SortMethod.Value);

	const Vec3 vLocalPos = pLocal->GetShootPos();
	const Vec3 vLocalAngles = I::EngineClient->GetViewAngles();

	const bool respectFOV = (sortMethod == ESortMethod::FOV || Vars::Aimbot::Melee::RespectFOV.Value);

	// Players
	if (Vars::Aimbot::Global::AimPlayers.Value)
	{
		const auto groupType = GetGroupType(pWeapon);

		for (const auto& pTarget : g_EntityCache.GetGroup(groupType))
		{
			// Is the target valid and alive?
			if (!pTarget->IsAlive() || pTarget->IsAGhost() || pTarget == pLocal)
			{
				continue;
			}

			if (F::AimbotGlobal.ShouldIgnore(pTarget, true)) { continue; }

			Vec3 vPos = pTarget->GetHitboxPos(HITBOX_PELVIS);
			Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
			const float flFOVTo = Math::CalcFov(vLocalAngles, vAngleTo);

			if (respectFOV && flFOVTo > Vars::Aimbot::Global::AimFOV.Value)
			{
				continue;
			}

			const auto& priority = F::AimbotGlobal.GetPriority(pTarget->GetIndex());
			const float flDistTo = vLocalPos.DistTo(vPos);
			validTargets.push_back({ pTarget, ETargetType::PLAYER, vPos, vAngleTo, flFOVTo, flDistTo, -1, false, priority });
		}
	}

	// Buildings
	if (Vars::Aimbot::Global::AimBuildings.Value)
	{
		const bool hasWrench = (pWeapon->GetWeaponID() == TF_WEAPON_WRENCH);
		const bool canDestroySapper = (G::CurItemDefIndex == Pyro_t_Homewrecker || G::CurItemDefIndex == Pyro_t_TheMaul || G::CurItemDefIndex == Pyro_t_NeonAnnihilator || G::CurItemDefIndex ==
									   Pyro_t_NeonAnnihilatorG);

		for (const auto& pObject : g_EntityCache.GetGroup(hasWrench || canDestroySapper ? EGroupType::BUILDINGS_ALL : EGroupType::BUILDINGS_ENEMIES))
		{
			const auto& pBuilding = reinterpret_cast<CBaseObject*>(pObject);

			// Is the building valid and alive?
			if (!pObject || !pBuilding || !pObject->IsAlive())
			{
				continue;
			}

			if (hasWrench && (pBuilding->GetTeamNum() == pLocal->GetTeamNum()))
			{
				if (!AimFriendlyBuilding(pBuilding))
				{
					continue;
				}
			}

			if (canDestroySapper && (pBuilding->GetTeamNum() == pLocal->GetTeamNum()))
			{
				if (!pBuilding->GetSapped())
				{
					continue;
				}
			}

			Vec3 vPos = pObject->GetWorldSpaceCenter();
			Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
			const float flFOVTo = Math::CalcFov(vLocalAngles, vAngleTo);

			if (respectFOV && flFOVTo > Vars::Aimbot::Global::AimFOV.Value)
			{
				continue;
			}

			const float flDistTo = sortMethod == ESortMethod::DISTANCE ? vLocalPos.DistTo(vPos) : 0.0f;
			validTargets.push_back({ pObject, ETargetType::BUILDING, vPos, vAngleTo, flFOVTo, flDistTo });
		}
	}

	// NPCs
	if (Vars::Aimbot::Global::AimNPC.Value)
	{
		for (const auto& pNPC : g_EntityCache.GetGroup(EGroupType::WORLD_NPC))
		{
			Vec3 vPos = pNPC->GetWorldSpaceCenter();
			Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);

			const float flFOVTo = Math::CalcFov(vLocalAngles, vAngleTo);
			const float flDistTo = sortMethod == ESortMethod::DISTANCE ? vLocalPos.DistTo(vPos) : 0.0f;

			if (respectFOV && flFOVTo > Vars::Aimbot::Global::AimFOV.Value)
			{
				continue;
			}

			validTargets.push_back({ pNPC, ETargetType::NPC, vPos, vAngleTo, flFOVTo, flDistTo });
		}
	}

	return validTargets;
}

bool CAimbotMelee::VerifyTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, Target_t& target)
{
	Vec3 hitboxpos;

	//Backtrack the target if required
	if (Vars::Backtrack::Enabled.Value && target.m_TargetType == ETargetType::PLAYER)
	{
		const auto& pRecords = F::Backtrack.GetRecords(target.m_pEntity);
		for (const auto& pTick : *pRecords)
		{
			if (!F::Backtrack.WithinRewind(pTick)) { continue; }
			hitboxpos = target.m_pEntity->GetHitboxPosMatrix(HITBOX_PELVIS, (matrix3x4*)(&pTick.BoneMatrix));

			if (Utils::VisPos(pLocal, target.m_pEntity, pLocal->GetShootPos(), hitboxpos)){
				target.SimTime = pTick.flSimTime;
				target.m_vAngleTo = Math::CalcAngle(pLocal->GetShootPos(), hitboxpos);
				target.m_vPos = hitboxpos;
				target.ShouldBacktrack = true;
			}	
		}
		if (!F::Backtrack.CanHitOriginal(target.m_pEntity) && !target.ShouldBacktrack) // Check if the player is in range for a non-backtrack hit
		{
			return false;
		}
	}



	if (Vars::Aimbot::Melee::RangeCheck.Value && !(target.ShouldBacktrack))
	{
		if (!CanMeleeHit(pLocal, pWeapon, target.m_vAngleTo, target.m_pEntity->GetIndex()))
		{
			return false;
		}
	}
	else if (target.ShouldBacktrack)
	{
		const float flRange = (pWeapon->GetSwingRange(pLocal)) * 1.9f;
		//Utils::ConLog("AimbotMelee", tfm::format("flRange : %.1f", flRange).c_str(), {133, 255, 159, 255});
		if (hitboxpos.Dist2D(pLocal->GetShootPos()) > flRange)
		{ return false; }
	}
	else {
		if (!Utils::VisPos(pLocal, target.m_pEntity, pLocal->GetShootPos(), target.m_vPos))
		{ return false; }
	}

	return true;
}

bool CAimbotMelee::GetTarget(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, Target_t& outTarget)
{
	auto validTargets = GetTargets(pLocal, pWeapon);
	if (validTargets.empty()) { return false; }

	const auto& sortMethod = static_cast<ESortMethod>(Vars::Aimbot::Melee::SortMethod.Value);
	F::AimbotGlobal.SortTargets(&validTargets, sortMethod);

	for (auto& target : validTargets)
	{
		if (VerifyTarget(pLocal, pWeapon, target))
		{
			outTarget = target;
			return true;
		}
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
			I::EngineClient->SetViewAngles(pCmd->viewangles);
			break;
		}

		case 1:
		{
			if (Vars::Aimbot::Melee::SmoothingAmount.Value == 0)
			{
				// plain aim at 0 smoothing factor
				pCmd->viewangles = vAngle;
				I::EngineClient->SetViewAngles(pCmd->viewangles);
				break;
			}

			Vec3 vecDelta = vAngle - pCmd->viewangles;
			Math::ClampAngles(vecDelta);
			pCmd->viewangles += vecDelta / Vars::Aimbot::Melee::SmoothingAmount.Value;
			I::EngineClient->SetViewAngles(pCmd->viewangles);
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

	if (Target.ShouldBacktrack){
		return true;
	}

	//There's a reason for running this even if range check is enabled (it calls this too), trust me :)
	if (!CanMeleeHit(pLocal, pWeapon, Vars::Aimbot::Melee::AimMethod.Value == 2 ? Target.m_vAngleTo : I::EngineClient->GetViewAngles(), Target.m_pEntity->GetIndex()))
	{
		return false;
	}

	return true;
}

bool CAimbotMelee::IsAttacking(const CUserCmd* pCmd, CBaseCombatWeapon* pWeapon)
{
	if (pWeapon->GetWeaponID() == TF_WEAPON_KNIFE)
	{
		return ((pCmd->buttons & IN_ATTACK) && G::WeaponCanAttack);
	}

	return fabs(pWeapon->GetSmackTime() - I::GlobalVars->curtime) < I::GlobalVars->interval_per_tick * 2.0f;
}

void CAimbotMelee::Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	bCached = false;

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

		const bool bIsAttacking = IsAttacking(pCmd, pWeapon);

		if (bIsAttacking)
		{
			G::IsAttacking = true;
		}

		// Set the target tickcount (Backtrack)
		if (bIsAttacking && target.m_TargetType == ETargetType::PLAYER)
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
