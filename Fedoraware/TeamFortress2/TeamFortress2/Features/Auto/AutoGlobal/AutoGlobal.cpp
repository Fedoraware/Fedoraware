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
	if (pTarget->GetDormant()) { return true; }
	if (!I::EngineClient->GetPlayerInfo(pTarget->GetIndex(), &pInfo)) { return true; }
	if (G::IsIgnored(pInfo.friendsID)) { return true; }
	if ((Vars::Triggerbot::Global::IgnoreOptions.Value & 1 << 5) && pTarget->IsPlayer() && pTarget->IsDisguised()) { return true; }
	if ((Vars::Triggerbot::Global::IgnoreOptions.Value & 1 << 4) && pTarget->IsPlayer() && (pTarget->GetSimulationTime() == pTarget->GetOldSimulationTime())) { return true; }
	if ((Vars::Triggerbot::Global::IgnoreOptions.Value & 1 << 3) && pTarget->IsTaunting()) { return true; }
	if ((Vars::Triggerbot::Global::IgnoreOptions.Value & 1 << 2) && g_EntityCache.IsFriend(pTarget->GetIndex())) { return true; }
	if ((Vars::Triggerbot::Global::IgnoreOptions.Value & 1 << 1) && pTarget->IsVisible()) { return true; }
	if ((Vars::Triggerbot::Global::IgnoreOptions.Value & 1 << 0) && !pTarget->IsVulnerable()) { return true; }

	return false;
}
