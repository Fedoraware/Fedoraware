#include "AutoStab.h"

#include "../../Vars.h"
#include "../../Backtrack/Backtrack.h"
#include "../../Aimbot/MovementSimulation/MovementSimulation.h"
#include "../../TickHandler/TickHandler.h"

bool CAutoStab::CanBackstabEx(const Vec3 vFrom, const Vec3 vTo, const Vec3 vTargetAng) {
	const Vec3 vAngleTo = Math::CalcAngle(vTo, vFrom);
	const float flFOVTo = Math::CalcFov(vTargetAng, vAngleTo);
	return flFOVTo > 90.f;
}

bool CAutoStab::CanBackstab(const Vec3& vSrc, const Vec3& vDst, Vec3 vWSCDelta)
{
	vWSCDelta.z = 0;
	vWSCDelta.NormalizeInPlace();

	auto vecEyeSpy = Vec3();
	Math::AngleVectors(vSrc, &vecEyeSpy);
	vecEyeSpy.z = 0;
	vecEyeSpy.NormalizeInPlace();

	auto vecEyeVictim = Vec3();
	Math::AngleVectors(vDst, &vecEyeVictim);
	vecEyeVictim.z = 0;
	vecEyeVictim.NormalizeInPlace();

	if (vWSCDelta.Dot(vecEyeVictim) <= 0.01f ||
		vWSCDelta.Dot(vecEyeSpy) <= 0.5f ||
		vecEyeSpy.Dot(vecEyeVictim) <= -0.3f)
	{
		return false;
	}

	return true;
}

CBaseEntity* CAutoStab::TraceMelee(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, const Vec3& vViewAngles, const Vec3 vStart)
{
	float flRange = (48.0f * Vars::Triggerbot::Stab::Range.Value);

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
	Utils::TraceHull(vTraceStart, vTraceEnd, { -18.0f, -18.0f, -18.0f }, { 18.0f, 18.0f, 18.0f }, MASK_SOLID, &Filter, &Trace);
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
	CBaseEntity* pEnemy = TraceMelee(pLocal, pWeapon, pCmd->viewangles, pLocal->GetShootPos());
	if (!pEnemy) { return; }

	if (!CanBackstab(pCmd->viewangles, pEnemy->GetEyeAngles(),
		(pEnemy->GetWorldSpaceCenter() - pLocal->GetWorldSpaceCenter())))
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
	// validate stab range
	const float flRange = (48.0f * Vars::Triggerbot::Stab::Range.Value);
	if (flRange <= 0.0f) { return; }

	Vec3 vShootPos{};
	Vec3 vOrigin{};
	if (Vars::Debug::DebugInfo.Value && F::MoveSim.Initialize(pLocal))
	{
		CMoveData moveData = {};
		Vec3 absOrigin = {};
		F::MoveSim.RunTick(moveData, absOrigin);
		vOrigin = absOrigin;
		vShootPos = vOrigin + pLocal->GetViewOffset();
		F::MoveSim.Restore();
	}
	else { 
		vOrigin = pLocal->m_vecOrigin();
		vShootPos = vOrigin + pLocal->GetViewOffset();
	}

	for (const auto& pEnemy : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
	{
		if (!IsEntityValid(pLocal, pEnemy)) { continue; }
		
		//	NON BACKTRACK
		if (F::Backtrack.CanHitOriginal(pEnemy)) {
			Vec3 vTargetPoint{};
			pEnemy->GetCollision()->CalcNearestPoint(vShootPos, &vTargetPoint);	//	we can backstab any part of the bounding box.
			Vec3 vAngleTo = Math::CalcAngle(vShootPos, vTargetPoint);
			if (vTargetPoint.DistTo(vShootPos) <= flRange)	//	range check
			{
				if (CanBackstab(vAngleTo, pEnemy->GetEyeAngles(), (vTargetPoint - vShootPos)))	//	angle check
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
		const Vec3 vOriginalEyeAngles = pEnemy->GetEyeAngles();
		const auto& pRecords = Vars::Backtrack::Enabled.Value ? F::Backtrack.GetRecords(pEnemy) : nullptr;
		const bool bDoBT = pRecords;
		if (!bDoBT) { return; }

		for (const auto& pTick : *pRecords)
		{
			if (!F::Backtrack.WithinRewind(pTick)) { continue; }

			pEnemy->SetAbsOrigin(pTick.vOrigin);	//	set these 4 CalcNearestPoint
			pEnemy->SetEyeAngles(pTick.vAngles);

			Vec3 vTargetPoint{};
			pEnemy->GetCollision()->CalcNearestPoint(vShootPos, &vTargetPoint);	//	we can backstab any part of the bounding box.
			const Vec3 vAngleTo = Math::CalcAngle(vShootPos, vTargetPoint);

			if (vTargetPoint.DistTo(vShootPos) > flRange)	//	range check
			{ continue; }
			if (!CanBackstabEx(vOrigin, pTick.vOrigin, pTick.vAngles))	//	ang check
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
			pEnemy->SetEyeAngles(vOriginalEyeAngles);
			return;
		}

		pEnemy->SetAbsOrigin(vOriginalPos);
		pEnemy->SetEyeAngles(vOriginalEyeAngles);
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
