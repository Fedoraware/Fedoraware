#include "AimbotGlobal.h"

#include "../../Vars.h"

namespace SandvichAimbot
{
	bool bIsSandvich;

	void IsSandvich()
	{
		bIsSandvich = (g_GlobalInfo.m_nCurItemDefIndex == Heavy_s_RoboSandvich ||
			g_GlobalInfo.m_nCurItemDefIndex == Heavy_s_Sandvich ||
			g_GlobalInfo.m_nCurItemDefIndex == Heavy_s_FestiveSandvich ||
			g_GlobalInfo.m_nCurItemDefIndex == Heavy_s_Fishcake ||
			g_GlobalInfo.m_nCurItemDefIndex == Heavy_s_TheDalokohsBar ||
			g_GlobalInfo.m_nCurItemDefIndex == Heavy_s_SecondBanana);
	}

	void RunSandvichAimbot(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, CUserCmd* pCmd, CBaseEntity* pTarget)
	{
		const int nWeaponID = pWeapon->GetWeaponID();
		const bool bShouldAim = (Vars::Aimbot::Global::AimKey.m_Var == VK_LBUTTON
			                         ? (pCmd->buttons & IN_ATTACK)
			                         : g_AimbotGlobal.IsKeyDown());

		if (bShouldAim)
		{
			Vec3 tr = pTarget->GetAbsOrigin() - pLocal->GetEyePosition();
			Vec3 angle;
			Math::VectorAngles(tr, angle);
			Math::ClampAngles(angle);
			pCmd->viewangles = angle;
			pCmd->buttons |= IN_ATTACK2;
			g_GlobalInfo.m_bHitscanSilentActive = true;
		}
	}
}

bool CAimbotGlobal::IsKeyDown()
{
	static KeyHelper aimKey{ &Vars::Aimbot::Global::AimKey.m_Var };
	return !Vars::Aimbot::Global::AimKey.m_Var ? true : aimKey.Down();
}

void CAimbotGlobal::SortTargets(const ESortMethod& Method)
{
	std::sort(m_vecTargets.begin(), m_vecTargets.end(), [&](const Target_t& a, const Target_t& b) -> bool {
		if (a.n_Priority.Mode < b.n_Priority.Mode) { return false; }

		switch (Method)
		{
		case ESortMethod::FOV: return (a.m_flFOVTo < b.m_flFOVTo);
		case ESortMethod::DISTANCE: return (a.m_flDistTo < b.m_flDistTo);
		default: return false;
		}
	});
}

const Target_t& CAimbotGlobal::GetBestTarget(const ESortMethod& Method)
{
	return *std::min_element(m_vecTargets.begin(), m_vecTargets.end(), [&](const Target_t& a, const Target_t& b) -> bool {
		if (a.n_Priority.Mode < b.n_Priority.Mode) { return false; }

		switch (Method)
		{
		case ESortMethod::FOV: return (a.m_flFOVTo < b.m_flFOVTo);
		case ESortMethod::DISTANCE: return (a.m_flDistTo < b.m_flDistTo);
		default: return false;
		}
	});
}

bool CAimbotGlobal::ShouldIgnore(CBaseEntity* pTarget, bool hasMedigun)
{
	PlayerInfo_t pInfo{};
	if (!pTarget) { return true; }
	if (!I::Engine->GetPlayerInfo(pTarget->GetIndex(), &pInfo)) { return true; }
	if (Vars::Aimbot::Global::IgnoreInvlunerable.m_Var && !pTarget->IsVulnerable()) { return true; }
	if (Vars::Aimbot::Global::IgnoreCloaked.m_Var && pTarget->IsCloaked())
	{
		const int nCond = pTarget->GetCond();
		if (!(nCond & TFCond_Milked) && !(nCond & TFCond_Jarated) && !(nCond & TFCond_CloakFlicker))
		{
			return true;
		}
	}
	if (Vars::Aimbot::Global::IgnoreTaunting.m_Var && pTarget->IsTaunting()) { return true; }

	// Special conditions for mediguns
	if (!hasMedigun)
	{
		if (g_GlobalInfo.IsIgnored(pInfo.friendsID)) { return true; }
		if (Vars::Aimbot::Global::IgnoreFriends.m_Var && g_EntityCache.IsFriend(pTarget->GetIndex())) { return true; }
	}

	return false;
}
