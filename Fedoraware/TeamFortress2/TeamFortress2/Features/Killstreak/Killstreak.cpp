#include "Killstreak.h"

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
	return KillstreakMap[g_EntityCache.GetWeapon()->GetIndex()];
}

void CKillstreaker::ApplyKillstreak()
{
	if (Vars::Misc::KillstreakWeapon.Value)
	{
		const auto& pLocal = I::EntityList->GetClientEntity(I::Engine->GetLocalPlayer());
		//const auto& resource = I::EntityList->GetClientEntity(g_EntityCache.GetPR()->Entity);
		if (!pLocal) { return; }

		int* streaksResource = g_EntityCache.GetPR()->GetStreaks(I::Engine->GetLocalPlayer());
		if (*streaksResource != GetCurrentStreak())
		{
			streaksResource[0] = GetCurrentStreak();
			streaksResource[1] = GetCurrentStreak();
			streaksResource[2] = GetCurrentStreak();
			streaksResource[3] = GetCurrentStreak();
		}

		static auto streaksOffset = g_NetVars.get_offset("DT_TFPlayer", "m_Shared", "m_nStreaks");
		const auto streaksPlayer = reinterpret_cast<int*>(pLocal + streaksOffset);
		streaksPlayer[0] = GetCurrentStreak();
		streaksPlayer[1] = GetCurrentStreak();
		streaksPlayer[2] = GetCurrentStreak();
		streaksPlayer[3] = GetCurrentStreak();
	}
}

void CKillstreaker::PlayerDeath(CGameEvent* pEvent)
{
	const int attacker = Utils::GetPlayerForUserID(pEvent->GetInt("attacker"));
	const int userid = Utils::GetPlayerForUserID(pEvent->GetInt("userid"));

	if (userid == I::Engine->GetLocalPlayer())
	{
		ResetKillstreak();
		return;
	}

	if (attacker != I::Engine->GetLocalPlayer() ||
		attacker == userid ||
		!g_EntityCache.GetLocal() || !g_EntityCache.GetWeapon() || !g_EntityCache.GetLocal()->IsAlive())
	{
		return;
	}

	Killstreak++;
	KillstreakMap[g_EntityCache.GetWeapon()->GetIndex()]++;

	pEvent->SetInt("kill_streak_total", GetCurrentStreak());
	pEvent->SetInt("kill_streak_wep", GetCurrentWeaponStreak());

	ApplyKillstreak();
}

void CKillstreaker::PlayerSpawn(CGameEvent* pEvent)
{
	const int userid = Utils::GetPlayerForUserID(pEvent->GetInt("userid"));

	if (userid == I::Engine->GetLocalPlayer())
	{
		ResetKillstreak();
	}
	ApplyKillstreak();
}

void CKillstreaker::FireEvents(CGameEvent* pEvent, const FNV1A_t uNameHash)
{
	if (Vars::Misc::KillstreakWeapon.Value)
	{
		if (uNameHash == FNV1A::HashConst("player_death"))
		{
			PlayerDeath(pEvent);
		}
		else if (uNameHash == FNV1A::HashConst("player_spawn"))
		{
			PlayerSpawn(pEvent);
		}
	}
}
