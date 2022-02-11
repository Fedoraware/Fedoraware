#include "PlayerResource.h"
#include "../../SDK/SDK.h"

#define HIGHEST_ENTITY g_Interfaces.EntityList->GetHighestEntityIndex()
#define RCC_PLAYERRESOURCE (int)ETFClassID::CTFPlayerResource
#define MAX_PLAYERS 32

void C_PlayerResource::Update() {
	CBaseEntity* ent;

	Entity = 0;
	for (int i = 0; i <= HIGHEST_ENTITY; i++) {
		ent = g_Interfaces.EntityList->GetClientEntity(i);
		if (ent && ent->GetClientClass()->iClassID == RCC_PLAYERRESOURCE) {
			Entity = i;
			return;
		}
	}
}

int C_PlayerResource::GetHealth(CBaseEntity* player) {
	CBaseEntity* ent;
	int idx;
	/* :thinking */
	////IF_GAME(!IsTF()) return 100;
	ent = g_Interfaces.EntityList->GetClientEntity(Entity);
	if (!ent || ent->GetClientClass()->iClassID != RCC_PLAYERRESOURCE) { return 0; }
	if (!player) { return 0; }
	idx = player->GetIndex();
	if (idx >= 64 || idx < 0) { return 0; }
	return *(int*)((unsigned)ent + g_NetVars.get_offset("DT_TFPlayerResource", "baseclass", "m_iHealth") + 4 * idx);
}


int C_PlayerResource::GetMaxHealth(CBaseEntity* player) {
	CBaseEntity* ent;
	int idx;
	/* :thinking */
	////IF_GAME(!IsTF()) return 100;
	ent = g_Interfaces.EntityList->GetClientEntity(Entity);
	if (!ent || ent->GetClientClass()->iClassID != RCC_PLAYERRESOURCE) { return 0; }
	idx = player->GetIndex();
	if (idx >= 64 || idx < 0) { return 0; }
	return *(int*)((unsigned)ent + g_NetVars.get_offset("DT_TFPlayerResource", "baseclass", "m_iMaxHealth") + 4 * idx);
}

int C_PlayerResource::GetMaxBuffedHealth(CBaseEntity* player) {
	CBaseEntity* ent;
	int idx;

	////IF_GAME(!IsTF()) return GetMaxHealth(player);
	ent = g_Interfaces.EntityList->GetClientEntity(Entity);
	if (!ent || ent->GetClientClass()->iClassID != RCC_PLAYERRESOURCE) { return 0; }
	idx = player->GetIndex();
	if (idx >= 64 || idx < 0) { return 0; }
	return *(int*)((unsigned)ent + g_NetVars.get_offset("DT_TFPlayerResource", "m_iMaxBuffedHealth") + 4 * idx);
}

int C_PlayerResource::GetTeamByIndex(int idx) {
	CBaseEntity* ent;

	if (idx >= MAX_PLAYERS || idx < 0) { return 0; }
	ent = g_Interfaces.EntityList->GetClientEntity(Entity);
	if (!ent || ent->GetClientClass()->iClassID != RCC_PLAYERRESOURCE) { return 0; }
	return *(int*)((unsigned)ent + g_NetVars.get_offset("DT_TFPlayerResource", "baseclass", "m_iTeam") + 4 * idx);
}

int C_PlayerResource::GetScoreByIndex(int idx) {
	CBaseEntity* ent;

	if (idx >= MAX_PLAYERS || idx < 1) { return 0; }
	ent = g_Interfaces.EntityList->GetClientEntity(Entity);
	if (!ent || ent->GetClientClass()->iClassID != RCC_PLAYERRESOURCE) { return 0; }
	return *(int*)((unsigned)ent + g_NetVars.get_offset("DT_TFPlayerResource", "m_iTotalScore") + 4 * idx);
}

int C_PlayerResource::GetKillsByIndex(int idx) {
	CBaseEntity* ent;

	if (idx >= MAX_PLAYERS || idx < 1) { return 0; }
	ent = g_Interfaces.EntityList->GetClientEntity(Entity);
	if (!ent || ent->GetClientClass()->iClassID != RCC_PLAYERRESOURCE) { return 0; }
	return *(int*)((unsigned)ent + g_NetVars.get_offset("DT_TFPlayerResource", "baseclass", "m_iScore") + 4 * idx);
}

int C_PlayerResource::GetDeathsByIndex(int idx) {
	CBaseEntity* ent;

	if (idx >= MAX_PLAYERS || idx < 1) { return 0; }
	ent = g_Interfaces.EntityList->GetClientEntity(Entity);
	if (!ent || ent->GetClientClass()->iClassID != RCC_PLAYERRESOURCE) { return 0; }
	return *(int*)((unsigned)ent + g_NetVars.get_offset("DT_TFPlayerResource", "baseclass", "m_iDeaths") + 4 * idx);
}

int C_PlayerResource::GetLevelByIndex(int idx) {
	CBaseEntity* ent;

	if (idx >= MAX_PLAYERS || idx < 1) { return 0; }
	ent = g_Interfaces.EntityList->GetClientEntity(Entity);
	if (!ent || ent->GetClientClass()->iClassID != RCC_PLAYERRESOURCE) { return 0; }
	return *(int*)((unsigned)ent + g_NetVars.get_offset("DT_TFPlayerResource", "m_iPlayerLevel") + 4 * idx);
}

int C_PlayerResource::GetDamageByIndex(int idx) {
	CBaseEntity* ent;

	if (idx >= MAX_PLAYERS || idx < 1) { return 0; }
	ent = g_Interfaces.EntityList->GetClientEntity(Entity);
	if (!ent || ent->GetClientClass()->iClassID != RCC_PLAYERRESOURCE) { return 0; }
	return *(int*)((unsigned)ent + g_NetVars.get_offset("DT_TFPlayerResource", "m_iDamage") + 4 * idx);
}

unsigned C_PlayerResource::GetAccountID(int idx) {
	CBaseEntity* ent;

	if (idx >= MAX_PLAYERS || idx < 1) { return 0; }
	ent = g_Interfaces.EntityList->GetClientEntity(Entity);
	if (!ent || ent->GetClientClass()->iClassID != RCC_PLAYERRESOURCE) { return 0; }
	return *(unsigned*)((unsigned)ent + g_NetVars.get_offset("DT_TFPlayerResource", "baseclass", "m_iAccountID") + 4 *
		idx);
}

int C_PlayerResource::GetPing(int idx) {
	CBaseEntity* ent;

	if (idx >= MAX_PLAYERS || idx < 1) { return 0; }
	ent = g_Interfaces.EntityList->GetClientEntity(Entity);
	if (!ent || ent->GetClientClass()->iClassID != RCC_PLAYERRESOURCE) { return 0; }
	return *(int*)((unsigned)ent + g_NetVars.get_offset("DT_TFPlayerResource", "baseclass", "m_iPing") + 4 * idx);
}

int C_PlayerResource::GetClass(CBaseEntity* player) {
	CBaseEntity* ent;
	int idx;

	ent = g_Interfaces.EntityList->GetClientEntity(Entity);
	if (!ent || ent->GetClientClass()->iClassID != RCC_PLAYERRESOURCE) { return 0; }
	idx = player->GetIndex();
	if (idx >= MAX_PLAYERS || idx < 0) { return 0; }
	return *(int*)((unsigned)ent + g_NetVars.get_offset("DT_TFPlayerResource", "m_iPlayerClass") + 4 * idx);
}

bool C_PlayerResource::IsAlive(int idx) {
	CBaseEntity* ent = g_Interfaces.EntityList->GetClientEntity(Entity);
	if (!ent || ent->GetClientClass()->iClassID != RCC_PLAYERRESOURCE) { return false; }
	if (idx >= MAX_PLAYERS || idx < 0) { return false; }
	return *(bool*)((unsigned)ent + g_NetVars.get_offset("DT_TFPlayerResource", "baseclass", "m_bAlive") + idx);
}

int C_PlayerResource::GetClassByIndex(int idx) {
	CBaseEntity* ent = g_Interfaces.EntityList->GetClientEntity(Entity);
	if (!ent || ent->GetClientClass()->iClassID != RCC_PLAYERRESOURCE) { return 0; }
	if (idx >= MAX_PLAYERS || idx < 0) { return 0; }
	return *(int*)((unsigned)ent + g_NetVars.get_offset("DT_TFPlayerResource", "m_iPlayerClass") + 4 * idx);
}

int C_PlayerResource::GetTeam(int idx) {
	CBaseEntity* ent = g_Interfaces.EntityList->GetClientEntity(Entity);
	if (!ent || ent->GetClientClass()->iClassID != RCC_PLAYERRESOURCE) { return 0; }
	if (idx >= MAX_PLAYERS || idx < 0) { return 0; }
	return *(int*)((unsigned)ent + g_NetVars.get_offset("DT_TFPlayerResource", "baseclass", "m_iTeam") + 4 * idx);
}
