#include "EntityCache.h"

#include <ranges>

#include "../GlobalInfo/GlobalInfo.h"
#include "../../../Features/ESP/ESP.h"

void CEntityCache::Fill()
{
	CBaseEntity* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer());
	if (!pLocal || !pLocal->IsInValidTeam()) { return; }

	m_pLocal = pLocal;
	m_pLocalWeapon = m_pLocal->GetActiveWeapon();

	// Cache observer target
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

	// Cache entities
	for (int n = 1; n < I::ClientEntityList->GetHighestEntityIndex(); n++)
	{
		CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(n);

		if (!pEntity)
		{
			continue;
		}

		const auto nClassID = pEntity->GetClassID();
		const int entIdx = pEntity->GetIndex();

		// Dormant ESP
		if (pEntity->GetDormant())
		{
			if (nClassID != ETFClassID::CTFPlayer)
			{
				continue;
			}

			// Is any dormant data available?
			if (!G::DormantPlayerESP.contains(entIdx))
			{
				continue;
			}

			const auto& dormantData = G::DormantPlayerESP[entIdx];
			const float lastUpdate = dormantData.LastUpdate;

			if (I::EngineClient->Time() - lastUpdate > Vars::ESP::Main::DormantTime.Value)
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

		// Ignore our third person player
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
			case ETFClassID::CObjectSapper:
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

				if (Hash::IsSpell(szName))
				{
					m_vecGroups[EGroupType::WORLD_SPELLBOOK].push_back(pEntity);
				}

				break;
			}

			case ETFClassID::CTFAmmoPack:
			{
				m_vecGroups[EGroupType::WORLD_AMMO].push_back(pEntity);
				break;
			}

		case ETFClassID::CTFProjectile_Rocket:
		case ETFClassID::CTFProjectile_SentryRocket:
		case ETFClassID::CTFGrenadePipebombProjectile:
		case ETFClassID::CTFProjectile_Jar:
		case ETFClassID::CTFProjectile_JarGas:
		case ETFClassID::CTFProjectile_JarMilk:
		case ETFClassID::CTFProjectile_Throwable:
		case ETFClassID::CTFProjectile_ThrowableBreadMonster:
		case ETFClassID::CTFStunBall:
		case ETFClassID::CTFBall_Ornament:
		case ETFClassID::CTFProjectile_Cleaver:
		case ETFClassID::CTFProjectile_Arrow:
		case ETFClassID::CTFProjectile_HealingBolt:
		case ETFClassID::CTFProjectile_Flare:
		case ETFClassID::CTFProjectile_BallOfFire:
		case ETFClassID::CTFProjectile_GrapplingHook:
		case ETFClassID::CTFProjectile_EnergyBall:
		case ETFClassID::CTFProjectile_EnergyRing:
		case ETFClassID::CTFProjectile_MechanicalArmOrb:
		case ETFClassID::CTFProjectile_SpellKartBats:
		case ETFClassID::CTFProjectile_SpellKartOrb:
		case ETFClassID::CTFProjectile_SpellLightningOrb:
		case ETFClassID::CTFProjectile_SpellTransposeTeleport:
		case ETFClassID::CTFProjectile_SpellMeteorShower:
		case ETFClassID::CTFProjectile_SpellSpawnBoss:
		case ETFClassID::CTFProjectile_SpellMirv:
		case ETFClassID::CTFProjectile_SpellPumpkin:
		case ETFClassID::CTFProjectile_SpellSpawnHorde:
		case ETFClassID::CTFProjectile_SpellSpawnZombie:
		case ETFClassID::CTFProjectile_SpellBats:
		case ETFClassID::CTFProjectile_SpellFireball:
			{
				m_vecGroups[EGroupType::WORLD_PROJECTILES].push_back(pEntity);
				if (I::ClientEntityList->GetClientEntityFromHandle((int)pEntity->GetThrower()) == m_pLocal ||
					I::ClientEntityList->GetClientEntityFromHandle((int)pEntity->GethOwner()) == m_pLocal) {
					m_vecGroups[EGroupType::LOCAL_PROJECTILES].push_back(pEntity);

					switch (nClassID) {
					case ETFClassID::CTFGrenadePipebombProjectile: {
						if (pEntity->GetPipebombType() == TYPE_STICKY || pEntity->GetPipebombPulsed()) {
							m_vecGroups[EGroupType::LOCAL_STICKIES].push_back(pEntity);
						}
						break;
					}
					case ETFClassID::CTFProjectile_Flare: {
						if (G::CurItemDefIndex == ETFWeapons::Pyro_s_TheDetonator) {
							m_vecGroups[EGroupType::LOCAL_FLARES].push_back(pEntity);
						}
						break;
					}

					}
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
			case ETFClassID::CHalloweenGiftPickup:
			{
				if (I::ClientEntityList->GetClientEntityFromHandle(pEntity->GetTargetPlayer()) == m_pLocal)
					m_vecGroups[EGroupType::WORLD_GARGOYLE].push_back(pEntity);

				break;
			}
			case ETFClassID::CCurrencyPack:
			{
				m_vecGroups[EGroupType::WORLD_CREDITS].push_back(pEntity);

				break;
			}

			default: break;
		}
	}

	UpdateFriends();
}

void CEntityCache::UpdateFriends()
{
	m_Friends.reset();
	// Check friendship for every player
	for (CBaseEntity* pEntity : GetGroup(EGroupType::PLAYERS_ALL))
	{
		PlayerInfo_t pInfo{};
		if (!I::EngineClient->GetPlayerInfo(pEntity->GetIndex(), &pInfo)) { continue; }
		m_Friends[pEntity->GetIndex()] = Utils::IsSteamFriend(pEntity) || G::PlayerPriority[pInfo.friendsID].Mode == 0;
	}
}

void CEntityCache::Clear()
{
	m_pLocal = nullptr;
	m_pLocalWeapon = nullptr;
	m_pObservedTarget = nullptr;
	m_pPlayerResource = nullptr;

	for (auto& group : m_vecGroups | std::views::values)
	{
		group.clear();
	}
}


const std::vector<CBaseEntity*>& CEntityCache::GetGroup(const EGroupType& Group)
{
	return m_vecGroups[Group];
}
