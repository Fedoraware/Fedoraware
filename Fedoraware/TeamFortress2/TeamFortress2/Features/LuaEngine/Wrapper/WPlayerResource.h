#pragma once

class WPlayerResource {
	CTFPlayerResource* PlayerResource;

public:
	explicit  WPlayerResource(CTFPlayerResource* playerResource) : PlayerResource(playerResource) { }

	int GetPing(int idx)
	{
		return PlayerResource->GetPing(idx);
	}

	int GetKills(int idx)
	{
		return PlayerResource->GetKills(idx);
	}

	int GetDeaths(int idx)
	{
		return PlayerResource->GetDeaths(idx);
	}

	bool GetConnected(int idx)
	{
		return PlayerResource->GetConnected(idx);
	}

	int GetTeam(int idx)
	{
		return PlayerResource->GetTeam(idx);
	}

	bool IsAlive(int idx)
	{
		return PlayerResource->IsAlive(idx);
	}

	int GetHealth(int idx)
	{
		return PlayerResource->GetHealth(idx);
	}

	unsigned GetAccountID(int idx)
	{
		return PlayerResource->GetAccountID(idx);
	}

	bool GetValid(int idx)
	{
		return PlayerResource->GetValid(idx);
	}

	int GetUserID(int idx)
	{
		return PlayerResource->GetUserID(idx);
	}

	const char* GetPlayerName(int idx)
	{
		return PlayerResource->GetPlayerName(idx);
	}

	int GetScore(int idx)
	{
		return PlayerResource->GetScore(idx);
	}

	int GetLevel(int idx)
	{
		return PlayerResource->GetLevel(idx);
	}

	int GetDamage(int idx)
	{
		return PlayerResource->GetDamage(idx);
	}

	int GetClass(int idx)
	{
		return PlayerResource->GetClass(idx);
	}

	int GetMaxHealth(int idx)
	{
		return PlayerResource->GetMaxHealth(idx);
	}
};