#include "EntityCache.h"

#include "../GlobalInfo/GlobalInfo.h"
#include "../../../Features/ESP/ESP.h"

void CEntityCache::Fill()
{
	CBaseEntity* _pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer());

	if (_pLocal && _pLocal->IsInValidTeam())
	{
		m_pLocal = _pLocal;
		m_pLocalWeapon = m_pLocal->GetActiveWeapon();

		switch (m_pLocal->GetObserverMode())
		{
			case OBS_MODE_FIRSTPERSON:
			case OBS_MODE_THIRDPERSON:
			{
				m_pObservedTarget = I::ClientEntityList->GetClientEntityFromHandle(m_pLocal->GetObserverTarget());
				break;
			}
			default: break;
		}

		for (int n = 1; n < I::ClientEntityList->GetHighestEntityIndex(); n++)
		{
			CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(n);

			if (!pEntity)
			{
				continue;
			}

			const auto nClassID = pEntity->GetClassID();
			const int entIdx = pEntity->GetIndex();

			if (pEntity->GetDormant())
			{
				if (nClassID != ETFClassID::CTFPlayer)
				{
					continue;
				}

				// Is any dormant data available?
				if (!G::DormantPlayerESP.count(entIdx))
				{
					continue;
				}

				const auto& dormantData = G::DormantPlayerESP[entIdx];
				const float lastUpdate = dormantData.LastUpdate;

				if (I::EngineClient->Time() - lastUpdate > 6.0f)
				{
					continue;
				}

				pEntity->SetAbsOrigin(dormantData.Location);
				pEntity->SetVecOrigin(dormantData.Location);

				pEntity->m_lifeState() = LIFE_ALIVE;
				const auto& playerResource = GetPR();
				if (playerResource && playerResource->GetValid(entIdx))
				{
					pEntity->m_iHealth() = playerResource->GetHealth(entIdx);
				}
			}

			if (pEntity == m_pLocal && !I::Input->CAM_IsThirdPerson())
			{
				continue;
			}
			
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
				case ETFClassID::CTFProjectile_GrapplingHook:
				case ETFClassID::CTFProjectile_Cleaver:
				case ETFClassID::CTFProjectile_EnergyBall:
				case ETFClassID::CTFProjectile_EnergyRing:
				case ETFClassID::CTFProjectile_HealingBolt:
				case ETFClassID::CTFProjectile_ThrowableBreadMonster:
				{
					m_vecGroups[EGroupType::WORLD_PROJECTILES].push_back(pEntity);

					if (nClassID == ETFClassID::CTFGrenadePipebombProjectile && pEntity->GetPipebombType() == TYPE_STICKY)
					{
						if (I::ClientEntityList->GetClientEntityFromHandle(reinterpret_cast<int>(pEntity->GetThrower())) == m_pLocal)
						{
							m_vecGroups[EGroupType::LOCAL_STICKIES].push_back(pEntity);
						}

						break;
					}

					if (nClassID == ETFClassID::CTFProjectile_Flare)
					{
						if (const auto& pSecondary = m_pLocal->GetWeaponFromSlot(EWeaponSlots::SLOT_SECONDARY))
						{
							if (pSecondary->GetItemDefIndex() == ETFWeapons::Pyro_s_TheDetonator)
							{
								if (I::ClientEntityList->GetClientEntityFromHandle(pEntity->GethOwner()) == m_pLocal)
								{
									m_vecGroups[EGroupType::LOCAL_FLARES].push_back(pEntity);
								}
							}
						}

						break;
					}

					break;
				}

				case ETFClassID::CTFPlayerResource:
				{
					m_pPlayerResource = reinterpret_cast<CTFPlayerResource*>(pEntity);
					break;
				}
				case ETFClassID::CHeadlessHatman:
				case ETFClassID::CTFTankBoss:
				case ETFClassID::CMerasmus:
				case ETFClassID::CZombie:
				case ETFClassID::CEyeballBoss:
				{
					m_vecGroups[EGroupType::WORLD_NPC].push_back(pEntity);

					break;
				}
				case ETFClassID::CTFPumpkinBomb:
				case ETFClassID::CTFGenericBomb:
				{
					m_vecGroups[EGroupType::WORLD_BOMBS].push_back(pEntity);

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

	if (I::EngineClient->GetPlayerInfo(pPlayer->GetIndex(), &pi) && pi.friendsID)
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
		{
			Friends[Player->GetIndex()] = IsPlayerOnSteamFriendList(Player);
		}
	}

	OldSize = CurSize;
}

void CEntityCache::Clear()
{
	m_pLocal = nullptr;
	m_pLocalWeapon = nullptr;
	m_pObservedTarget = nullptr;
	m_pPlayerResource = nullptr;

	for (auto& Group : m_vecGroups)
	{
		Group.second.clear();
	}
}

bool CEntityCache::IsFriend(int entIdx)
{
	if (entIdx < 0 || entIdx >= 129)
	{
		return false;
	}
	return Friends[entIdx];
}

const std::vector<CBaseEntity*>& CEntityCache::GetGroup(const EGroupType& Group)
{
	return m_vecGroups[Group];
}
