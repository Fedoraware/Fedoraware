#pragma once
#include "../BaseEntity/BaseEntity.h"
#include "../BaseCombatWeapon/BaseCombatWeapon.h"
#include "../PlayerResource/PlayerResource.h"
#include "../BaseObject/BaseObject.h"

enum struct EGroupType
{
	PLAYERS_ALL, PLAYERS_ENEMIES, PLAYERS_TEAMMATES,
	BUILDINGS_ALL, BUILDINGS_ENEMIES, BUILDINGS_TEAMMATES,
	WORLD_HEALTH, WORLD_AMMO, WORLD_PROJECTILES, WORLD_NPC, WORLD_BOMBS,
	LOCAL_STICKIES, LOCAL_FLARES
};

class CEntityCache
{
	CBaseEntity* m_pLocal = nullptr;
	CBaseCombatWeapon* m_pLocalWeapon = nullptr;
	CBaseEntity* m_pObservedTarget = nullptr;
	CTFPlayerResource* m_pPlayerResource = nullptr;

	std::unordered_map<EGroupType, std::vector<CBaseEntity*>> m_vecGroups;
	void UpdateFriends();

public:

	void Fill();
	void Clear();
    uint_fast64_t friends;
    bool IsFriend(int entIdx) { return (friends >> entIdx) & 1; }

	CBaseEntity* GetLocal() { return m_pLocal; }
	CBaseCombatWeapon* GetWeapon() { return m_pLocalWeapon; }
	CBaseEntity* GetObservedTarget() { return m_pObservedTarget; }
	CTFPlayerResource* GetPR() { return m_pPlayerResource; }

	const std::vector<CBaseEntity*>& GetGroup(const EGroupType& Group);
};

inline CEntityCache g_EntityCache;