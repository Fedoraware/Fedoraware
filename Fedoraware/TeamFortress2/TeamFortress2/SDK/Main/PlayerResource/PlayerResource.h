#pragma once
#include "../../NetVars/NetVars.h"

class CPlayerResource
{
public:
	NETVAR(m_iPing, void*, "CPlayerResource", "m_iPing");
	NETVAR(m_iScore, void*, "CPlayerResource", "m_iScore");
	NETVAR(m_iDeaths, void*, "CPlayerResource", "m_iDeaths");
	NETVAR(m_bConnected, void*, "CPlayerResource", "m_bConnected");
	NETVAR(m_iTeam, void*, "CPlayerResource", "m_iTeam");
	NETVAR(m_bAlive, void*, "CPlayerResource", "m_bAlive");
	NETVAR(m_iHealth, void*, "CPlayerResource", "m_iHealth");
	NETVAR(m_iAccountID, void*, "CPlayerResource", "m_iAccountID");
	NETVAR(m_bValid, void*, "CPlayerResource", "m_bValid");
	NETVAR(m_iUserID, void*, "CPlayerResource", "m_iUserID");
};

class CTFPlayerResource : public CPlayerResource
{
public:
	NETVAR(m_iTotalScore, void*, "CTFPlayerResource", "m_iTotalScore");
	NETVAR(m_iMaxHealth, void*, "CTFPlayerResource", "m_iMaxHealth");
	NETVAR(m_iMaxBuffedHealth, void*, "CTFPlayerResource", "m_iMaxBuffedHealth");
	NETVAR(m_iPlayerClass, void*, "CTFPlayerResource", "m_iPlayerClass");
	NETVAR(m_bArenaSpectator, void*, "CTFPlayerResource", "m_bArenaSpectator");
	NETVAR(m_iActiveDominations, void*, "CTFPlayerResource", "m_iActiveDominations");
	NETVAR(m_flNextRespawnTime, void*, "CTFPlayerResource", "m_flNextRespawnTime");
	NETVAR(m_iChargeLevel, void*, "CTFPlayerResource", "m_iChargeLevel");
	NETVAR(m_iDamage, void*, "CTFPlayerResource", "m_iDamage");
	NETVAR(m_iDamageAssist, void*, "CTFPlayerResource", "m_iDamageAssist");
	NETVAR(m_iDamageBoss, void*, "CTFPlayerResource", "m_iDamageBoss");
	NETVAR(m_iHealing, void*, "CTFPlayerResource", "m_iHealing");
	NETVAR(m_iHealingAssist, void*, "CTFPlayerResource", "m_iHealingAssist");
	NETVAR(m_iDamageBlocked, void*, "CTFPlayerResource", "m_iDamageBlocked");
	NETVAR(m_iCurrencyCollected, void*, "CTFPlayerResource", "m_iCurrencyCollected");
	NETVAR(m_iBonusPoints, void*, "CTFPlayerResource", "m_iBonusPoints");
	NETVAR(m_iPlayerLevel, void*, "CTFPlayerResource", "m_iPlayerLevel");
	NETVAR(m_iStreaks, void*, "CTFPlayerResource", "m_iStreaks");
	NETVAR(m_iUpgradeRefundCredits, void*, "CTFPlayerResource", "m_iUpgradeRefundCredits");
	NETVAR(m_iBuybackCredits, void*, "CTFPlayerResource", "m_iBuybackCredits");
	NETVAR(m_iPartyLeaderRedTeamIndex, int, "CTFPlayerResource", "m_iPartyLeaderRedTeamIndex");
	NETVAR(m_iPartyLeaderBlueTeamIndex, int, "CTFPlayerResource", "m_iPartyLeaderBlueTeamIndex");
	NETVAR(m_iEventTeamStatus, int, "CTFPlayerResource", "m_iEventTeamStatus");
	NETVAR(m_iPlayerClassWhenKilled, void*, "CTFPlayerResource", "m_iPlayerClassWhenKilled");
	NETVAR(m_iConnectionState, void*, "CTFPlayerResource", "m_iConnectionState");
	NETVAR(m_flConnectTime, void*, "CTFPlayerResource", "m_flConnectTime");

	/* CPlayerResource */

	int GetPing(int idx)
	{
		if (!this) { return 0; }
		static auto offset = g_NetVars.get_offset("DT_TFPlayerResource", "baseclass", "m_iPing");
		return *reinterpret_cast<int*>(this + offset + 4 * idx);
	}

	int GetKills(int idx)
	{
		if (!this) { return 0; }
		static auto offset = g_NetVars.get_offset("DT_TFPlayerResource", "baseclass", "m_iScore");
		return *reinterpret_cast<int*>(this + offset + 4 * idx);
	}

	int GetDeaths(int idx)
	{
		if (!this) { return 0; }
		static auto offset = g_NetVars.get_offset("DT_TFPlayerResource", "baseclass", "m_iDeaths");
		return *reinterpret_cast<int*>(this + offset + 4 * idx);
	}

	bool GetConnected(int idx)
	{
		if (!this) { return false; }
		static auto offset = g_NetVars.get_offset("DT_TFPlayerResource", "baseclass", "m_bConnected");
		return *reinterpret_cast<bool*>(this + offset + idx);
	}

	int GetTeam(int idx)
	{
		if (!this) { return 0; }
		static auto offset = g_NetVars.get_offset("DT_TFPlayerResource", "baseclass", "m_iTeam");
		return *reinterpret_cast<int*>(this + offset + 4 * idx);
	}

	bool IsAlive(int idx)
	{
		if (!this) { return false; }
		static auto offset = g_NetVars.get_offset("DT_TFPlayerResource", "baseclass", "m_bAlive");
		return *reinterpret_cast<bool*>(this + offset + idx);
	}

	int GetHealth(int idx)
	{
		if (!this) { return 0; }
		static auto offset = g_NetVars.get_offset("DT_TFPlayerResource", "baseclass", "m_iHealth");
		return *reinterpret_cast<int*>(this + offset + 4 * idx);
	}

	unsigned GetAccountID(int idx)
	{
		if (!this) { return 0; }
		static auto offset = g_NetVars.get_offset("DT_TFPlayerResource", "baseclass", "m_iAccountID");
		return *reinterpret_cast<int*>(this + offset + 4 * idx);
	}

	bool GetValid(int idx)
	{
		if (!this) { return false; }
		static auto offset = g_NetVars.get_offset("DT_TFPlayerResource", "baseclass", "m_bValid");
		return *reinterpret_cast<bool*>(this + offset + idx);
	}

	int GetUserID(int idx)
	{
		if (!this) { return 0; }
		static auto offset = g_NetVars.get_offset("DT_TFPlayerResource", "baseclass", "m_iUserID");
		return *reinterpret_cast<int*>(this + offset + 4 * idx);
	}

	const char* GetPlayerName(int idx)
	{
		if (!this) { return ""; }
		return *reinterpret_cast<const char**>(this + 0x0554 + 4 * idx);
	}

	/* CTFPlayerResource */

	int GetScore(int idx)
	{
		if (!this) { return 0; }
		static auto offset = g_NetVars.get_offset("DT_TFPlayerResource", "m_iTotalScore");
		return *reinterpret_cast<int*>(this + offset + 4 * idx);
	}

	int GetLevel(int idx)
	{
		if (!this) { return 0; }
		static auto offset = g_NetVars.get_offset("DT_TFPlayerResource", "m_iPlayerLevel");
		return *reinterpret_cast<int*>(this + offset + 4 * idx);
	}

	int GetDamage(int idx)
	{
		if (!this) { return 0; }
		static auto offset = g_NetVars.get_offset("DT_TFPlayerResource", "m_iDamage");
		return *reinterpret_cast<int*>(this + offset + 4 * idx);
	}

	int GetClass(int idx)
	{
		if (!this) { return 0; }
		static auto offset = g_NetVars.get_offset("DT_TFPlayerResource", "m_iPlayerClass");
		return *reinterpret_cast<int*>(this + offset + 4 * idx);
	}

	int GetMaxHealth(int idx)
	{
		if (!this) { return 0; }
		static auto offset = g_NetVars.get_offset("DT_TFPlayerResource", "m_iMaxHealth");
		return *reinterpret_cast<int*>(this + offset + 4 * idx);
	}

	int* GetStreaks(int idx)
	{
		if (!this) { return nullptr; }
		static auto offset = g_NetVars.get_offset("DT_TFPlayerResource", "m_iStreaks");
		return reinterpret_cast<int*>(this + offset + 4 * idx);
	}

	float GetConnectionTime(int idx)
	{
		if (!this) { return 0.f; }
		static auto offset = g_NetVars.get_offset("DT_TFPlayerResource", "m_flConnectTime");
		return *reinterpret_cast<float*>(this + offset + 4 * idx);
	}
};
