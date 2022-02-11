#pragma once

class CBaseEntity;

class C_PlayerResource {
public:
	void Update();
	int GetMaxHealth(CBaseEntity* player);
	int GetHealth(CBaseEntity* player);
	int GetMaxBuffedHealth(CBaseEntity* player);
	int GetClass(CBaseEntity* player);
	int GetTeamByIndex(int idx);
	int GetScoreByIndex(int idx);
	int GetKillsByIndex(int idx);
	int GetDeathsByIndex(int idx);
	int GetLevelByIndex(int idx);
	int GetDamageByIndex(int idx);
	unsigned GetAccountID(int idx);

	int GetPing(int idx);
	int GetClassByIndex(int idx);
	int GetTeam(int idx);
	bool IsAlive(int idx);

	int Entity;
};

inline C_PlayerResource ___playerResourceInst;

inline C_PlayerResource* g_PR = &___playerResourceInst;
