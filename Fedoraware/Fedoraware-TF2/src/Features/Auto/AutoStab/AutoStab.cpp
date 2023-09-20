#include "AutoStab.h"

#include "../../Vars.h"
#include "../../Backtrack/Backtrack.h"
#include "../../Aimbot/MovementSimulation/MovementSimulation.h"
#include "../../TickHandler/TickHandler.h"

bool CAutoStab::CanBackstab(CBaseCombatWeapon* pWeapon, CBaseEntity* pTarget) {
	static auto CTFKnife_IsBehindAndFacingTarget = reinterpret_cast<bool(__fastcall*)(void*, void*, CBaseEntity*)>(S::CTFKnife_IsBehindAndFacingTarget());
	return CTFKnife_IsBehindAndFacingTarget(pWeapon, nullptr, pTarget);
}

bool CAutoStab::CanBackstab(CBaseEntity* pLocal, CBaseEntity* pTarget, CBaseCombatWeapon* pWeapon, Vec3 vLocalAngles) {
	Vec3 vLocalAngB = pLocal->GetEyeAngles();
	I::Prediction->SetLocalViewAngles(vLocalAngles);
	const bool bReturnVal = CanBackstab(pWeapon, pTarget);
	I::Prediction->SetLocalViewAngles(vLocalAngB);

	return bReturnVal;
}

CBaseEntity* CAutoStab::TraceMelee(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, const Vec3& vViewAngles, const Vec3 vStart)
{
	const float flScale = std::min(1.0f, pLocal->m_flModelScale());
	const float flRange = Utils::ATTRIB_HOOK_FLOAT((66.f * Vars::Triggerbot::Stab::Range.Value) * flScale, "melee_range_multiplier", pWeapon, 0, true);
	const float flHull = 18.f * Utils::ATTRIB_HOOK_FLOAT(1.0f, "melee_bounds_multiplier", pWeapon, 0, true) * flScale;

	if (flRange <= 0.0f)
	{
		return nullptr;
	}

	auto vForward = Vec3();
	Math::AngleVectors(vViewAngles, &vForward);
	Vec3 vTraceStart = vStart;
	Vec3 vTraceEnd = (vTraceStart + (vForward * flRange));

	CGameTrace Trace = {};
	CTraceFilterHitscan Filter = {};
	Filter.pSkip = pLocal;
	Utils::TraceHull(vTraceStart, vTraceEnd, { -flHull, -flHull, -flHull }, { flHull, flHull, flHull }, MASK_SOLID, &Filter, &Trace);
	if (IsEntityValid(pLocal, Trace.entity))
	{ return Trace.entity; }

	return nullptr;
}

bool CAutoStab::IsEntityValid(CBaseEntity* pLocal, CBaseEntity* pEntity)
{
	if (!pEntity || !pEntity->IsAlive() || pEntity->GetTeamNum() == pLocal->GetTeamNum() || !pEntity->IsPlayer())
		return false;

	if (Vars::Triggerbot::Stab::IgnRazor.Value) {
		CBaseEntity* pAttachment = pEntity->FirstMoveChild();

		for (int n = 0; n < 32; n++) {
			if (!pAttachment) { continue; }
			if (pAttachment->GetClassID() == ETFClassID::CTFWearableRazorback) { return false; }	//	Credits to mfed
		}
	}

	if (F::AutoGlobal.ShouldIgnore(pEntity)) { return false; }

	return true;
}

void CAutoStab::RunLegit(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	CBaseEntity* pEnemy = TraceMelee(pLocal, pWeapon, pCmd->viewangles, pLocal->GetEyePosition());
	if (!pEnemy) { return; }

	if (!CanBackstab(pWeapon, pEnemy))
	{
		return;
	}

	pCmd->buttons |= IN_ATTACK;
	m_bShouldDisguise = true;

	if (Vars::Misc::DisableInterpolation.Value)
	{
		pCmd->tick_count = TIME_TO_TICKS(pEnemy->GetSimulationTime() + TICKS_TO_TIME(TIME_TO_TICKS(G::LerpTime)));
	}
}

void CAutoStab::RunRage(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	const float flScale = std::min(1.0f, pLocal->m_flModelScale());
	// validate stab range
	const float flRange = Utils::ATTRIB_HOOK_FLOAT((66.f * Vars::Triggerbot::Stab::Range.Value) * flScale, "melee_range_multiplier", pWeapon, 0, true);;
	if (flRange <= 0.0f) { return; }
	const float flHull = Utils::ATTRIB_HOOK_FLOAT(18.f, "melee_bounds_multiplier", pWeapon, 0, true) * flScale;
	const float flCheckRange = sqrt(pow(flRange, 2) + pow(flHull, 2));

	const Vec3 vShootPos = pLocal->GetEyePosition();
	const Vec3 vOrigin = pLocal->GetWorldSpaceCenter();

	for (const auto& pEnemy : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
	{
		if (!IsEntityValid(pLocal, pEnemy)) { continue; }
		
		//	NON BACKTRACK
		if (F::Backtrack.CanHitOriginal(pEnemy)) {
			Vec3 vTargetPoint{};
			pEnemy->GetCollision()->CalcNearestPoint(vShootPos, &vTargetPoint);	//	we can backstab any part of the bounding box.
			const Vec3 vAngleTo = Math::CalcAngle(vShootPos, vTargetPoint);
			if (vTargetPoint.DistTo(vShootPos) <= flCheckRange)	//	range check
			{
				if (CanBackstab(pLocal, pEnemy, pWeapon, vAngleTo))	//	angle check
				{
					if (TraceMelee(pLocal, pWeapon, vAngleTo, vShootPos) == pEnemy)	//	vis check
					{
						// Silent backstab
						if (Vars::Triggerbot::Stab::Silent.Value)
						{
							Utils::FixMovement(pCmd, vAngleTo);
							G::SilentTime = true;
						}

						pCmd->viewangles = vAngleTo;
						pCmd->buttons |= IN_ATTACK;
						m_bShouldDisguise = true;

						pCmd->tick_count = TIME_TO_TICKS(pEnemy->GetSimulationTime() + TICKS_TO_TIME(TIME_TO_TICKS(G::LerpTime)));
						return;
					}
				}
			}
		}

		//	BACKTRACK
		const Vec3 vOriginalPos = pEnemy->GetAbsOrigin();
		//const Vec3 vOriginalEyeAngles = pEnemy->GetEyeAngles();
		const auto& pRecords = Vars::Backtrack::Enabled.Value ? F::Backtrack.GetRecords(pEnemy) : nullptr;
		const bool bDoBT = pRecords;
		if (!bDoBT) { continue; }

		for (const auto& pTick : *pRecords)
		{
			if (!F::Backtrack.WithinRewind(pTick)) { continue; }

			pEnemy->SetAbsOrigin(pTick.vOrigin);	//	set these 4 CalcNearestPoint

			Vec3 vTargetPoint{};
			pEnemy->GetCollision()->CalcNearestPoint(vShootPos, &vTargetPoint);	//	we can backstab any part of the bounding box.
			const Vec3 vAngleTo = Math::CalcAngle(vShootPos, vTargetPoint);

			if (vTargetPoint.DistTo(vShootPos) > flRange)	//	range check
			{ continue; }
			if (!CanBackstab(pLocal, pEnemy, pWeapon, vAngleTo))	//	ang check
			{ continue; }

			// Silent backstab
			if (Vars::Triggerbot::Stab::Silent.Value)
			{
				Utils::FixMovement(pCmd, vAngleTo);
				G::SilentTime = true;
			}

			pCmd->viewangles = vAngleTo;
			pCmd->buttons |= IN_ATTACK;
			m_bShouldDisguise = true;

			pCmd->tick_count = TIME_TO_TICKS(pTick.flSimTime + TICKS_TO_TIME(TIME_TO_TICKS(G::LerpTime)));

			pEnemy->SetAbsOrigin(vOriginalPos);
			return;
		}

		pEnemy->SetAbsOrigin(vOriginalPos);
	}
}

void CAutoStab::Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	if (!Vars::Triggerbot::Stab::Active.Value || !G::WeaponCanAttack || pWeapon->GetWeaponID() != TF_WEAPON_KNIFE || G::IsAttacking)
	{
		return;
	}

	if (Vars::Triggerbot::Stab::RageMode.Value)
	{
		RunRage(pLocal, pWeapon, pCmd);
	}
	else
	{
		RunLegit(pLocal, pWeapon, pCmd);
	}

	if (pCmd->buttons & IN_ATTACK)
	{
		G::IsAttacking = true;
	}

	G::AutoBackstabRunning = true;
}
