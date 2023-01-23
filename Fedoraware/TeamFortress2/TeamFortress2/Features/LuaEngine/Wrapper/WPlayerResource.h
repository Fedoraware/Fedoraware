#pragma once

class WPlayerResource {
	CTFPlayerResource* PlayerResource;

public:
	explicit  WPlayerResource(CTFPlayerResource* playerResource) : PlayerResource(playerResource) { }

	bool IsValid(int idx)
	{
		return PlayerResource != nullptr && PlayerResource->GetValid(idx);
	}

	int GetPing(int idx)
	{
		if (!IsValid(idx)) { return 0; }
		return PlayerResource->GetPing(idx);
	}

	int GetKills(int idx)
	{
		if (!IsValid(idx)) { return 0; }
		return PlayerResource->GetKills(idx);
	}

	int GetDeaths(int idx)
	{
		if (!IsValid(idx)) { return 0; }
		return PlayerResource->GetDeaths(idx);
	}

	bool GetConnected(int idx)
	{
		if (!IsValid(idx)) { return false; }
		return PlayerResource->GetConnected(idx);
	}

	int GetTeam(int idx)
	{
		if (!IsValid(idx)) { return 0; }
		return PlayerResource->GetTeam(idx);
	}

	bool IsAlive(int idx)
	{
		if (!IsValid(idx)) { return false; }
		return PlayerResource->IsAlive(idx);
	}

	int GetHealth(int idx)
	{
		if (!IsValid(idx)) { return 0; }
		return PlayerResource->GetHealth(idx);
	}

	unsigned GetAccountID(int idx)
	{
		if (!IsValid(idx)) { return 0; }
		return PlayerResource->GetAccountID(idx);
	}

	bool GetValid(int idx)
	{
		if (!IsValid(idx)) { return false; }
		return PlayerResource->GetValid(idx);
	}

	int GetUserID(int idx)
	{
		if (!IsValid(idx)) { return 0; }
		return PlayerResource->GetUserID(idx);
	}

	const char* GetPlayerName(int idx)
	{
		if (!IsValid(idx)) { return ""; }
		return PlayerResource->GetPlayerName(idx);
	}

	int GetScore(int idx)
	{
		if (!IsValid(idx)) { return 0; }
		return PlayerResource->GetScore(idx);
	}

	int GetLevel(int idx)
	{
		if (!IsValid(idx)) { return 0; }
		return PlayerResource->GetLevel(idx);
	}

	int GetDamage(int idx)
	{
		if (!IsValid(idx)) { return 0; }
		return PlayerResource->GetDamage(idx);
	}

	int GetClass(int idx)
	{
		if (!IsValid(idx)) { return 0; }
		return PlayerResource->GetClass(idx);
	}

	int GetMaxHealth(int idx)
	{
		if (!IsValid(idx)) { return 0; }
		return PlayerResource->GetMaxHealth(idx);
	}
};