#include "AutoStab.h"

#include "../../Vars.h"
#include "../../Backtrack/Backtrack.h"

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

bool CAutoStab::TraceMelee(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, const Vec3& vViewAngles,
                           CBaseEntity** pEntityOut)
{
	float flRange = (48.0f * Vars::Triggerbot::Stab::Range.Value);

	if (flRange <= 0.0f)
	{
		return false;
	}

	auto vForward = Vec3();
	Math::AngleVectors(vViewAngles, &vForward);
	Vec3 vTraceStart = pLocal->GetShootPos();
	Vec3 vTraceEnd = (vTraceStart + (vForward * flRange));

	CGameTrace Trace = {};
	CTraceFilterHitscan Filter = {};
	Filter.pSkip = pLocal;
	Utils::TraceHull(vTraceStart, vTraceEnd, {-18.0f, -18.0f, -18.0f}, {18.0f, 18.0f, 18.0f}, MASK_SOLID, &Filter, &Trace);
	if (IsEntityValid(pLocal, Trace.entity))
	{
		if (pEntityOut && !*pEntityOut)
		{
			*pEntityOut = Trace.entity;
		}

		return true;
	}

	return false;
}

bool CAutoStab::IsEntityValid(CBaseEntity* pLocal, CBaseEntity* pEntity)
{
	if (!pEntity || !pEntity->IsAlive() || pEntity->GetTeamNum() == pLocal->GetTeamNum() || !pEntity->IsPlayer())
		return false;

	if (F::AutoGlobal.ShouldIgnore(pEntity)) { return false; }

	return true;
}

void CAutoStab::RunLegit(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	CBaseEntity* pEnemy = nullptr;

	if (!TraceMelee(pLocal, pWeapon, pCmd->viewangles, &pEnemy))
		return;

	if (Vars::Triggerbot::Stab::IgnRazor.Value && pEnemy->GetClassNum() == CLASS_SNIPER &&
		pEnemy->GetWeaponFromSlot(SLOT_SECONDARY)->GetItemDefIndex() == Sniper_s_TheRazorback)
		return;

	if (!CanBackstab(pCmd->viewangles, pEnemy->GetEyeAngles(),
	                 (pEnemy->GetWorldSpaceCenter() - pLocal->GetWorldSpaceCenter())))
		return;

	pCmd->buttons |= IN_ATTACK;
	m_bShouldDisguise = true;

	if (Vars::Misc::DisableInterpolation.Value)
	{
		pCmd->tick_count = TIME_TO_TICKS(pEnemy->GetSimulationTime() + G::LerpTime);
	}
}

void CAutoStab::RunRage(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	for (const auto& pEnemy : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
	{
		const auto& pWeapon = pEnemy->GetWeaponFromSlot(SLOT_SECONDARY);
		if (Vars::Triggerbot::Stab::IgnRazor.Value && pEnemy->GetClassNum() == CLASS_SNIPER &&
			pWeapon && pWeapon->GetItemDefIndex() == Sniper_s_TheRazorback)
			continue;

		CBaseEntity* pTraceEnemy = nullptr;

		Vec3 vAngleTo = Math::CalcAngle(pLocal->GetShootPos(), pEnemy->GetHitboxPos(HITBOX_PELVIS));

		const auto& pRecord = F::Backtrack.GetPlayerRecords(pEnemy);
		bool bBacktrackable = pRecord != nullptr && Vars::Backtrack::Enabled.Value && Vars::Backtrack::Aim.Value;
		float flSimTime = 0;

		Vec3 vOriginalPos = pEnemy->GetAbsOrigin();
		Vec3 vOriginalEyeAngles = pEnemy->GetEyeAngles();

		if (bBacktrackable)
		{
			for (size_t t = 0; t < pRecord->size(); t++)
			{
				auto& pTick = pRecord->back();

				pEnemy->SetAbsOrigin(vOriginalPos);
				pEnemy->SetEyeAngles(vOriginalEyeAngles);

				flSimTime = pTick.SimulationTime;

				if (!F::Backtrack.IsGoodTick(flSimTime))
				{
					continue;
				}

				auto pBoneMatrix = reinterpret_cast<matrix3x4*>(&pTick.BoneMatrix);
				Vec3 vPelvisPos = Vec3(pBoneMatrix[HITBOX_PELVIS][0][3],
									   pBoneMatrix[HITBOX_PELVIS][1][3],
									   pBoneMatrix[HITBOX_PELVIS][2][3]);

				vAngleTo = Math::CalcAngle(pLocal->GetShootPos(), vPelvisPos);

				pEnemy->SetAbsOrigin(pTick.AbsOrigin);
				pEnemy->SetEyeAngles(pTick.EyeAngles);

				float flRange = (48.0f * Vars::Triggerbot::Stab::Range.Value);

				if (flRange <= 0.0f)
				{
					continue;
				}

				if (pTick.WorldSpaceCenter.DistTo(pLocal->GetWorldSpaceCenter()) > flRange * 2)
				{
					continue;
				}

				if (!CanBackstab(vAngleTo, pEnemy->GetEyeAngles(), (pTick.WorldSpaceCenter - pLocal->GetWorldSpaceCenter())))
					continue;

				if (Vars::Triggerbot::Stab::Silent.Value)
				{
					Utils::FixMovement(pCmd, vAngleTo);
					G::SilentTime = true;
				}

				pCmd->viewangles = vAngleTo;
				pCmd->buttons |= IN_ATTACK;
				m_bShouldDisguise = true;

				if (Vars::Misc::DisableInterpolation.Value)
				{
					pCmd->tick_count = TIME_TO_TICKS(flSimTime + G::LerpTime);
				}
				else
				{
					pCmd->tick_count = TIME_TO_TICKS(flSimTime);
				}

				pEnemy->SetAbsOrigin(vOriginalPos);
				pEnemy->SetEyeAngles(vOriginalEyeAngles);

				return;
			}
			continue;
		}
		else
		{
			if (!TraceMelee(pLocal, pWeapon, vAngleTo, &pTraceEnemy) || pTraceEnemy != pEnemy)
				continue;

			if (!CanBackstab(vAngleTo, pEnemy->GetEyeAngles(),
				(pEnemy->GetWorldSpaceCenter() - pLocal->GetWorldSpaceCenter())))
				continue;

			if (Vars::Triggerbot::Stab::Silent.Value)
			{
				Utils::FixMovement(pCmd, vAngleTo);
				G::SilentTime = true;
			}

			pCmd->viewangles = vAngleTo;
			pCmd->buttons |= IN_ATTACK;
			m_bShouldDisguise = true;

			if (Vars::Misc::DisableInterpolation.Value)
			{
				pCmd->tick_count = TIME_TO_TICKS(pEnemy->GetSimulationTime() + G::LerpTime);
			}

			return;
		}
	}
}

void CAutoStab::Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	if (!Vars::Triggerbot::Stab::Active.Value || !G::WeaponCanAttack || pWeapon->GetWeaponID() != TF_WEAPON_KNIFE)
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
