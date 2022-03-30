#include "AutoStab.h"

#include "../../Vars.h"

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

	if (vWSCDelta.Dot(vecEyeVictim) <= 0.01f)
		return false;

	if (vWSCDelta.Dot(vecEyeSpy) <= 0.5f)
		return false;

	if (vecEyeSpy.Dot(vecEyeVictim) <= -0.3f)
		return false;

	return true;
}

bool CAutoStab::TraceMelee(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, const Vec3& vViewAngles,
                           CBaseEntity** pEntityOut)
{
	float flRange = (48.0f * Vars::Triggerbot::Stab::Range.m_Var);

	if (flRange <= 0.0f)
		return false;

	auto vForward = Vec3();
	Math::AngleVectors(vViewAngles, &vForward);
	Vec3 vTraceStart = pLocal->GetShootPos();
	Vec3 vTraceEnd = (vTraceStart + (vForward * flRange));

	CGameTrace Trace = {};
	CTraceFilterHitscan Filter = {};
	Filter.pSkip = pLocal;
	Utils::TraceHull(vTraceStart, vTraceEnd, {-18.0f, -18.0f, -18.0f}, {18.0f, 18.0f, 18.0f}, MASK_SOLID, &Filter,
	                 &Trace);

	if (IsEntityValid(pLocal, Trace.entity))
	{
		if (pEntityOut && !*pEntityOut)
			*pEntityOut = Trace.entity;

		return true;
	}

	return false;
}

bool CAutoStab::IsEntityValid(CBaseEntity* pLocal, CBaseEntity* pEntity)
{
	if (!pEntity || !pEntity->IsAlive() || pEntity->GetTeamNum() == pLocal->GetTeamNum() || !pEntity->IsPlayer())
		return false;

	if (Vars::Triggerbot::Global::IgnoreInvlunerable.m_Var && !pEntity->IsVulnerable())
		return false;

	if (Vars::Triggerbot::Global::IgnoreCloaked.m_Var && pEntity->IsCloaked())
		return false;

	if (Vars::Triggerbot::Global::IgnoreFriends.m_Var && g_EntityCache.Friends[pEntity->GetIndex()])
		return false;

	return true;
}

void CAutoStab::RunLegit(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	CBaseEntity* pEnemy = nullptr;

	if (!TraceMelee(pLocal, pWeapon, pCmd->viewangles, &pEnemy))
		return;

	if (Vars::Triggerbot::Stab::IgnRazor.m_Var && pEnemy->GetClassNum() == CLASS_SNIPER &&
		pEnemy->GetWeaponFromSlot(SLOT_SECONDARY)->GetItemDefIndex() == Sniper_s_TheRazorback)
		return;

	if (!CanBackstab(pCmd->viewangles, pEnemy->GetEyeAngles(),
	                 (pEnemy->GetWorldSpaceCenter() - pLocal->GetWorldSpaceCenter())))
		return;

	pCmd->buttons |= IN_ATTACK;
	m_bShouldDisguise = true;

	if (Vars::Misc::DisableInterpolation.m_Var)
	{
		pCmd->tick_count = TIME_TO_TICKS(pEnemy->GetSimulationTime()
			+ std::max(g_ConVars.cl_interp->GetFloat(), g_ConVars.cl_interp_ratio->GetFloat() / g_ConVars.cl_updaterate
				->GetFloat()));
	}
}

void CAutoStab::RunRage(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	for (const auto& pEnemy : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
	{
		if (Vars::Triggerbot::Stab::IgnRazor.m_Var && pEnemy->GetClassNum() == CLASS_SNIPER &&
			pEnemy->GetWeaponFromSlot(SLOT_SECONDARY)->GetItemDefIndex() == Sniper_s_TheRazorback)
			continue;

		CBaseEntity* pTraceEnemy = nullptr;

		Vec3 vAngleTo = Math::CalcAngle(pLocal->GetShootPos(), pEnemy->GetHitboxPos(HITBOX_PELVIS));

		if (!TraceMelee(pLocal, pWeapon, vAngleTo, &pTraceEnemy) || pTraceEnemy != pEnemy)
			continue;

		if (!CanBackstab(vAngleTo, pEnemy->GetEyeAngles(),
		                 (pEnemy->GetWorldSpaceCenter() - pLocal->GetWorldSpaceCenter())))
			continue;

		if (Vars::Triggerbot::Stab::Silent.m_Var)
		{
			Utils::FixMovement(pCmd, vAngleTo);
			g_GlobalInfo.m_bSilentTime = true;
		}

		pCmd->viewangles = vAngleTo;
		pCmd->buttons |= IN_ATTACK;
		m_bShouldDisguise = true;

		if (Vars::Misc::DisableInterpolation.m_Var)
		{
			pCmd->tick_count = TIME_TO_TICKS(pEnemy->GetSimulationTime()
				+ std::max(g_ConVars.cl_interp->GetFloat(), g_ConVars.cl_interp_ratio->GetFloat() / g_ConVars.
					cl_updaterate->GetFloat()));
		}

		break;
	}
}

void CAutoStab::Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	if (!Vars::Triggerbot::Stab::Active.m_Var || !g_GlobalInfo.m_bWeaponCanAttack || pWeapon->GetWeaponID() !=
		TF_WEAPON_KNIFE)
		return;

	if (Vars::Triggerbot::Stab::RageMode.m_Var)
		RunRage(pLocal, pWeapon, pCmd);

	else RunLegit(pLocal, pWeapon, pCmd);

	if (pCmd->buttons & IN_ATTACK)
		g_GlobalInfo.m_bAttacking = true;

	g_GlobalInfo.m_bAutoBackstabRunning = true;
}
