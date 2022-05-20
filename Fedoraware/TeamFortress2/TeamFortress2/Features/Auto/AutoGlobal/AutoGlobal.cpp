#include "AutoGlobal.h"

#include "../../Vars.h"

bool CAutoGlobal::IsKeyDown()
{
	static KeyHelper autoKey{ &Vars::Triggerbot::Global::TriggerKey.m_Var };
	switch (Vars::Triggerbot::Global::TriggerKey.m_Var)
	{
		case 0x0: return true;
		default: return autoKey.Down();
	}
}

bool CAutoGlobal::ShouldIgnore(CBaseEntity* pTarget)
{
	PlayerInfo_t pInfo{};
	if (!pTarget) { return true; }
	if (!I::Engine->GetPlayerInfo(pTarget->GetIndex(), &pInfo)) { return true; }
	if (g_GlobalInfo.IsIgnored(pInfo.friendsID)) { return true; }
	if (Vars::Triggerbot::Global::IgnoreFriends.m_Var && g_EntityCache.IsFriend(pTarget->GetIndex())) { return true; }
	if (Vars::Triggerbot::Global::IgnoreCloaked.m_Var && pTarget->IsCloaked()) { return true; }
	if (Vars::Triggerbot::Global::IgnoreInvlunerable.m_Var && !pTarget->IsVulnerable()) { return true; }

	return false;
}
