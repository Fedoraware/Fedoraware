#include "Killstreak.h"
#include "../PlayerResource/PlayerResource.h"

void CKillstreaker::ResetKillstreak()
{
	Killstreak = 0;
	KillstreakMap.clear();
}

int CKillstreaker::GetCurrentStreak()
{
	return Killstreak;
}

int CKillstreaker::GetCurrentWeaponStreak()
{
	return KillstreakMap[g_EntityCache.m_pLocalWeapon->GetIndex()];
}

void CKillstreaker::ApplyKillstreak()
{
	if (Vars::Misc::KillstreakWeapon.m_Var) {
		const auto& pLocal = g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetLocalPlayer());
		const auto& resource = g_Interfaces.EntityList->GetClientEntity(g_PR->Entity);
		if (!pLocal || !resource || resource->GetClientClass()->m_ClassID != (int)ETFClassID::CTFPlayerResource) {
			return;
		}

		int* streaks_resource = reinterpret_cast<int*>((unsigned)resource + g_NetVars.get_offset("DT_TFPlayerResource", "m_iStreaks") + 4 * g_Interfaces.Engine->GetLocalPlayer());

		if (*streaks_resource != GetCurrentStreak())
		{
			streaks_resource[0] = GetCurrentStreak();
			streaks_resource[1] = GetCurrentStreak();
			streaks_resource[2] = GetCurrentStreak();
			streaks_resource[3] = GetCurrentStreak();
		}
		static auto streaks_offset = g_NetVars.get_offset("DT_TFPlayer", "m_Shared", "m_nStreaks");
		int* streaks_player = reinterpret_cast<int*>(pLocal + streaks_offset);
		streaks_player[0] = GetCurrentStreak();
		streaks_player[1] = GetCurrentStreak();
		streaks_player[2] = GetCurrentStreak();
		streaks_player[3] = GetCurrentStreak();
	}
}

int _GetPlayerForUserID(int userID)
{
	for (int i = 1; i <= g_Interfaces.Engine->GetMaxClients(); i++)
	{
		PlayerInfo_t player_info;
		if (!g_Interfaces.Engine->GetPlayerInfo(i, &player_info))
			continue;
		// Found player
		if (player_info.userID == userID)
			return i;
	}
	return 0;
};

void CKillstreaker::PlayerDeath(CGameEvent* pEvent)
{
	int attacker = _GetPlayerForUserID(pEvent->GetInt("attacker"));
	int userid = _GetPlayerForUserID(pEvent->GetInt("userid"));

	if (userid == g_Interfaces.Engine->GetLocalPlayer())
	{
		ResetKillstreak();
		return;
	}

	if (attacker != g_Interfaces.Engine->GetLocalPlayer() ||
		attacker == userid ||
		!g_EntityCache.m_pLocal || !g_EntityCache.m_pLocalWeapon || !g_EntityCache.m_pLocal->IsAlive()) {
		return;
	}

	Killstreak++;
	KillstreakMap[g_EntityCache.m_pLocalWeapon->GetIndex()]++;

	pEvent->SetInt("kill_streak_total", GetCurrentStreak());
	pEvent->SetInt("kill_streak_wep", GetCurrentWeaponStreak());

	ApplyKillstreak();
}

void CKillstreaker::PlayerSpawn(CGameEvent* pEvent)
{
	int userid = _GetPlayerForUserID(pEvent->GetInt("userid"));

	if (userid == g_Interfaces.Engine->GetLocalPlayer())
	{
		ResetKillstreak();
	}
	ApplyKillstreak();
}

void CKillstreaker::FireEvents(CGameEvent* pEvent, const FNV1A_t uNameHash)
{
	if (Vars::Misc::KillstreakWeapon.m_Var) {
		if (uNameHash == FNV1A::HashConst("player_death")) {
			PlayerDeath(pEvent);
		}
		else if (uNameHash == FNV1A::HashConst("player_spawn")) {
			PlayerSpawn(pEvent);
		}
	}
}
