#pragma once
#include "../BaseEntity/BaseEntity.h"
#include "../BaseCombatWeapon/BaseCombatWeapon.h"
#include "../BaseObject/BaseObject.h"

enum struct EGroupType
{
	PLAYERS_ALL, PLAYERS_ENEMIES, PLAYERS_TEAMMATES,
	BUILDINGS_ALL, BUILDINGS_ENEMIES, BUILDINGS_TEAMMATES,
	WORLD_HEALTH, WORLD_AMMO, WORLD_PROJECTILES,
	LOCAL_STICKIES, LOCAL_FLARES
};

class CEntityCache
{
private:
	std::map<EGroupType, std::vector<CBaseEntity*>> m_vecGroups;
	void UpdateFriends();

public:
	CBaseEntity* m_pLocal = nullptr;
	CBaseCombatWeapon* m_pLocalWeapon = nullptr;
	CBaseEntity* m_pObservedTarget = nullptr;

	void Fill();
	void Clear();
	const std::vector<CBaseEntity*>& GetGroup(const EGroupType& Group);

	bool Friends[129] = { false };
};

inline CEntityCache g_EntityCache;