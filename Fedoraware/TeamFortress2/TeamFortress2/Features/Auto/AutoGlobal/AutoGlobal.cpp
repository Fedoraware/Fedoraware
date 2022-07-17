#include "AutoGlobal.h"

#include "../../Vars.h"

bool CAutoGlobal::IsKeyDown()
{
	static KeyHelper autoKey{ &Vars::Triggerbot::Global::TriggerKey.Value };
	switch (Vars::Triggerbot::Global::TriggerKey.Value)
	{
		case 0x0: return true;
		default: return autoKey.Down();
	}
}

bool CAutoGlobal::ShouldIgnore(CBaseEntity* pTarget)
{
	PlayerInfo_t pInfo{};
	if (!pTarget) { return true; }
	if (!I::EngineClient->GetPlayerInfo(pTarget->GetIndex(), &pInfo)) { return true; }
	if (G::IsIgnored(pInfo.friendsID)) { return true; }
	if (Vars::Triggerbot::Global::IgnoreFriends.Value && g_EntityCache.IsFriend(pTarget->GetIndex())) { return true; }
	if (Vars::Triggerbot::Global::IgnoreCloaked.Value && pTarget->IsCloaked()) { return true; }
	if (Vars::Triggerbot::Global::IgnoreInvlunerable.Value && !pTarget->IsVulnerable()) { return true; }

	return false;
}
