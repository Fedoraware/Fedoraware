#pragma once

class CBaseEntity;

class C_PlayerResource {
public:
	void Update();
	int GetMaxHealth(int idx);
	int GetHealth(int idx);
	int GetMaxBuffedHealth(int idx);
	int GetClass(int idx);
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
	int GetUserID(int idx);
	bool isValid(int idx);
	const char* GetPlayerName(int idx);

	int Entity;
};

inline C_PlayerResource ___playerResourceInst;

inline C_PlayerResource* g_PR = &___playerResourceInst;
