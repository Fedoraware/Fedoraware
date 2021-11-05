#include "AutoUber.h"

#include "../../Vars.h"
#include "../AutoGlobal/AutoGlobal.h"

void CAutoUber::Run(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	if (!Vars::Triggerbot::Uber::Active.m_Var || //Not enabled, return
		pWeapon->GetWeaponID() != TF_WEAPON_MEDIGUN || //Not medigun, return
		g_GlobalInfo.m_nCurItemDefIndex == Medic_s_TheKritzkrieg || //Kritzkrieg,  return
		pWeapon->GetUberCharge() < 1.0f)			                //Not charged, return
		return;

	//Check local status, if enabled. Don't pop if local already is not vulnerable
	if (Vars::Triggerbot::Uber::PopLocal.m_Var && pLocal->IsVulnerable())
	{
		m_flHealth = static_cast<float>(pLocal->GetHealth());
		m_flMaxHealth = static_cast<float>(pLocal->GetMaxHealth());

		if (((m_flHealth / m_flMaxHealth) * 100.0f) <= Vars::Triggerbot::Uber::HealthLeft.m_Var)
		{
			pCmd->buttons |= IN_ATTACK2; //We under the wanted health percentage, pop
			return; //Popped, no point checking our target's status
		}
	}

	//Will be null as long as we aren't healing anyone
	if (const auto& pTarget = pWeapon->GetHealingTarget())
	{
		//Ignore if target is somehow dead, or already not vulnerable
		if (!pTarget->IsAlive() || !pTarget->IsVulnerable())
			return;

		//Dont waste if not a friend, fuck off scrub
		if (Vars::Triggerbot::Uber::OnlyFriends.m_Var && !g_EntityCache.Friends[pTarget->GetIndex()])
			return;

		//Check target's status
		m_flHealth = static_cast<float>(pTarget->GetHealth());
		m_flMaxHealth = static_cast<float>(pTarget->GetMaxHealth());

		if (((m_flHealth / m_flMaxHealth) * 100.0f) <= Vars::Triggerbot::Uber::HealthLeft.m_Var)
		{
			pCmd->buttons |= IN_ATTACK2; //Target under wanted health percentage, pop
		}
	}
}