#include "EntityCache.h"

#include "../GlobalInfo/GlobalInfo.h"

void CEntityCache::Fill()
{
	CBaseEntity* _pLocal = g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetLocalPlayer());

	if (_pLocal && _pLocal->IsInValidTeam())
	{
		m_pLocal = _pLocal;
		m_pLocalWeapon = m_pLocal->GetActiveWeapon();

		switch (m_pLocal->GetObserverMode())
		{
		case OBS_MODE_FIRSTPERSON:
		case OBS_MODE_THIRDPERSON:
		{
			m_pObservedTarget = g_Interfaces.EntityList->GetClientEntityFromHandle(m_pLocal->GetObserverTarget());
			break;
		}
		default: break;
		}

		CBaseEntity* pEntity;
		for (int n = 1; n < g_Interfaces.EntityList->GetHighestEntityIndex(); n++)
		{
			pEntity = g_Interfaces.EntityList->GetClientEntity(n);

			if (!pEntity)
				continue;

			if (pEntity->GetDormant() && !g_GlobalInfo.partyPlayerESP.count(pEntity->GetIndex())) {
				continue;
			}

			if (pEntity->GetDormant()) {
				const float lastUpdate = g_GlobalInfo.partyPlayerESP[pEntity->GetIndex()].LastUpdate;
				if (g_Interfaces.Engine->Time() - lastUpdate <= 5.0f) {
					pEntity->SetAbsOrigin(g_GlobalInfo.partyPlayerESP[pEntity->GetIndex()].Location);
					pEntity->SetVecOrigin(g_GlobalInfo.partyPlayerESP[pEntity->GetIndex()].Location);
				} else {
					continue;
				}
			}

			if (pEntity == m_pLocal)
			{
				if (!g_Interfaces.Input->CAM_IsThirdPerson())
					continue;
			}

			auto nClassID = pEntity->GetClassID();
			switch (nClassID)
			{
			case ETFClassID::CTFPlayer:
			{
				m_vecGroups[EGroupType::PLAYERS_ALL].push_back(pEntity);
				m_vecGroups[pEntity->GetTeamNum() != m_pLocal->GetTeamNum() ? EGroupType::PLAYERS_ENEMIES : EGroupType::PLAYERS_TEAMMATES].push_back(pEntity);
				break;
			}

			case ETFClassID::CObjectSentrygun:
			case ETFClassID::CObjectDispenser:
			case ETFClassID::CObjectTeleporter:
			{
				m_vecGroups[EGroupType::BUILDINGS_ALL].push_back(pEntity);
				m_vecGroups[pEntity->GetTeamNum() != m_pLocal->GetTeamNum() ? EGroupType::BUILDINGS_ENEMIES : EGroupType::BUILDINGS_TEAMMATES].push_back(pEntity);
				break;
			}

			case ETFClassID::CBaseAnimating:
			{
				const auto szName = pEntity->GetModelName();

				if (Hash::IsAmmo(szName))
				{
					m_vecGroups[EGroupType::WORLD_AMMO].push_back(pEntity);
					break;
				}

				if (Hash::IsHealth(szName))
				{
					m_vecGroups[EGroupType::WORLD_HEALTH].push_back(pEntity);
					break;
				}

				break;
			}

			case ETFClassID::CTFAmmoPack:
			{
				m_vecGroups[EGroupType::WORLD_AMMO].push_back(pEntity);
				break;
			}

			case ETFClassID::CTFProjectile_Rocket:
			case ETFClassID::CTFGrenadePipebombProjectile:
			case ETFClassID::CTFProjectile_Jar:
			case ETFClassID::CTFProjectile_JarGas:
			case ETFClassID::CTFProjectile_JarMilk:
			case ETFClassID::CTFProjectile_Arrow:
			case ETFClassID::CTFProjectile_SentryRocket:
			case ETFClassID::CTFProjectile_Flare:
			case ETFClassID::CTFProjectile_Cleaver:
			case ETFClassID::CTFProjectile_EnergyBall:
			case ETFClassID::CTFProjectile_HealingBolt:
			case ETFClassID::CTFProjectile_ThrowableBreadMonster:
			{
				m_vecGroups[EGroupType::WORLD_PROJECTILES].push_back(pEntity);

				if (nClassID == ETFClassID::CTFGrenadePipebombProjectile && pEntity->GetPipebombType() == TYPE_STICKY)
				{
					if (g_Interfaces.EntityList->GetClientEntityFromHandle(reinterpret_cast<int>(pEntity->GetThrower())) == m_pLocal)
						m_vecGroups[EGroupType::LOCAL_STICKIES].push_back(pEntity);

					break;
				}

				if (nClassID == ETFClassID::CTFProjectile_Flare)
				{
					if (const auto& pSecondary = m_pLocal->GetWeaponFromSlot(EWeaponSlots::SLOT_SECONDARY))
					{
						if (pSecondary->GetItemDefIndex() == ETFWeapons::Pyro_s_TheDetonator)
						{
							if (g_Interfaces.EntityList->GetClientEntityFromHandle(pEntity->GethOwner()) == m_pLocal)
								m_vecGroups[EGroupType::LOCAL_FLARES].push_back(pEntity);
						}
					}

					break;
				}

				break;
			}

			default: break;
			}
		}

		UpdateFriends();
	}
}

bool IsPlayerOnSteamFriendList(CBaseEntity* pPlayer)
{
	PlayerInfo_t pi = { };

	if (g_Interfaces.Engine->GetPlayerInfo(pPlayer->GetIndex(), &pi) && pi.friendsID)
	{
		CSteamID steamID{ pi.friendsID, 1, k_EUniversePublic, k_EAccountTypeIndividual };
		return g_SteamInterfaces.Friends002->HasFriend(steamID, k_EFriendFlagImmediate);
	}

	return false;
}

void CEntityCache::UpdateFriends()
{
	static size_t CurSize, OldSize;
	const auto Players = GetGroup(EGroupType::PLAYERS_ALL);
	CurSize = Players.size();

	if (CurSize != OldSize)
	{
		for (const auto& Player : Players)
			Friends[Player->GetIndex()] = IsPlayerOnSteamFriendList(Player);
	}

	OldSize = CurSize;
}

void CEntityCache::Clear()
{
	m_pLocal = nullptr;
	m_pLocalWeapon = nullptr;
	m_pObservedTarget = nullptr;

	for (auto& Group : m_vecGroups)
		Group.second.clear();
}

const std::vector<CBaseEntity*>& CEntityCache::GetGroup(const EGroupType& Group)
{
	return m_vecGroups[Group];
}