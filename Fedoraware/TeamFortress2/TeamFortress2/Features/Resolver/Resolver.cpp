#include "Resolver.h"

static std::vector YawResolves{ 0.0f, 180.0f, 65.0f, -65.0f, -180.0f };

bool CResolver::ShouldAutoResolve()
{
	if (g_GlobalInfo.m_WeaponType == EWeaponType::PROJECTILE) { return false; }

	if (const auto& pWeapon = g_EntityCache.m_pLocalWeapon)
	{
		if (pWeapon->GetClassID() == ETFClassID::CTFMinigun) { return false; }
	}

	return true;
}

/* Run the resolver and apply the resolved angles */
void CResolver::Run()
{
	if (Vars::AntiHack::Resolver::Resolver.m_Var)
	{
		Vec3 localHead;
		if (const auto& pLocal = g_EntityCache.m_pLocal)
		{
			localHead = pLocal->GetHitboxPos(HITBOX_HEAD);
		}

		for (auto i = 1; i <= I::Engine->GetMaxClients(); i++)
		{
			CBaseEntity* entity;
			PlayerInfo_t temp{};

			if (!(entity = I::EntityList->GetClientEntity(i))) {
				continue;
			}

			if (entity->GetDormant()) {
				continue;
			}

			if (!I::Engine->GetPlayerInfo(i, &temp)) {
				continue;
			}

			if (!entity->GetLifeState() == LIFE_ALIVE) {
				continue;
			}

			if (entity->IsTaunting()) {
				continue;
			}

			const Vector vX = entity->GetEyeAngles();
			auto* m_angEyeAnglesX = reinterpret_cast<float*>(reinterpret_cast<DWORD>(entity) + g_NetVars.
				get_offset("DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[0]"));
			auto* m_angEyeAnglesY = reinterpret_cast<float*>(reinterpret_cast<DWORD>(entity) + g_NetVars.
				get_offset("DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[1]"));

			auto findResolve = g_Resolver.ResolvePlayers.find(temp.friendsID);
			ResolveMode resolveMode;
			if (findResolve != g_Resolver.ResolvePlayers.end())
			{
				resolveMode = findResolve->second;
			}

			// Pitch resolver 
			switch (resolveMode.m_Pitch)
			{
			case 1:
			{
				*m_angEyeAnglesX = -89; // Up
				break;
			}
			case 2:
			{
				*m_angEyeAnglesX = 89; // Down
				break;
			}
			case 3:
			{
				*m_angEyeAnglesX = 0; // Zero
				break;
			}
			case 4:
			{
				// Auto (Will resolve fake up/down)
				if (vX.x >= 90)
				{
					*m_angEyeAnglesX = -89;
				}

				if (vX.x <= -90)
				{
					*m_angEyeAnglesX = 89;
				}
				break;
			}
			default:
				break;
			}

			// Yaw resolver
			const Vec3 vAngleTo = Math::CalcAngle(entity->GetHitboxPos(HITBOX_HEAD), localHead);
			switch (resolveMode.m_Yaw)
			{
			case 1:
			{
				*m_angEyeAnglesY = vAngleTo.y; // Forward
				break;
			}
			case 2:
			{
				*m_angEyeAnglesY = vAngleTo.y + 180.f; // Backward
				break;
			}
			case 3:
			{
				*m_angEyeAnglesY = vAngleTo.y - 90.f; // Left
				break;
			}
			case 4:
			{
				*m_angEyeAnglesY = vAngleTo.y + 90.f; // Right
				break;
			}
			case 5:
			{
				*m_angEyeAnglesY += 180; // Invert (this doesn't work properly)
				break;
			}
			case 6:
			{
				// Auto resolver
				if (ShouldAutoResolve())
				{
					*m_angEyeAnglesY = YawResolves[ResolveData[temp.friendsID].Mode];
				}
				break;
			}
			default:
				break;
			}
		}
	}
}

/* Update resolver data (for Bruteforce) */
void CResolver::Update(CUserCmd* pCmd)
{
	if (!Vars::AntiHack::Resolver::Resolver.m_Var) { return; }

	// Log shots
	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (g_GlobalInfo.m_nCurrentTargetIdx != 0 &&
			(pCmd->buttons & IN_ATTACK || g_GlobalInfo.m_bAttacking) &&
			pLocal->GetActiveWeapon()->CanShoot(pLocal))
		{
			PlayerInfo_t temp{};
			const int aimTarget = g_GlobalInfo.m_nCurrentTargetIdx;
			
			if (const auto& pTarget = I::EntityList->GetClientEntity(aimTarget))
			{
				if (I::Engine->GetPlayerInfo(aimTarget, &temp))
				{
					const auto findResolve = g_Resolver.ResolvePlayers.find(temp.friendsID);
					ResolveMode resolveMode;
					if (findResolve != g_Resolver.ResolvePlayers.end())
					{
						resolveMode = findResolve->second;
					}

					if (resolveMode.m_Yaw == 6)
					{
						ResolveData[temp.friendsID].LastShot = I::Engine->Time();
						ResolveData[temp.friendsID].RequiresUpdate = true;
					}
				}
			}
		}
	}

	// Check for misses
	for (auto& data : ResolveData)
	{
		float delay = 1.f;
		if (const auto nc = I::Engine->GetNetChannelInfo())
		{
			delay = (nc->GetLatency(FLOW_OUTGOING) + nc->GetLatency(FLOW_INCOMING)) + 0.3f;
		}
		const float time = I::Engine->Time();
		const bool shouldCheck = (time - data.second.LastShot) > delay;
		const float timeDiff = data.second.LastHit - data.second.LastShot;

		if (data.second.RequiresUpdate && shouldCheck && timeDiff <= 0.f)
		{
			// Miss
			data.second.Mode += 1;
			while (data.second.Mode >= YawResolves.size())
			{
				data.second.Mode -= YawResolves.size();
			}

			data.second.RequiresUpdate = false;
		}
	}
}

/* Called when the someone was damaged. Did we hit? */
void CResolver::OnPlayerHurt(CGameEvent* pEvent)
{
	if (!Vars::AntiHack::Resolver::Resolver.m_Var) { return; }

	const int victim = I::Engine->GetPlayerForUserID(pEvent->GetInt("userid"));
	const int attacker = I::Engine->GetPlayerForUserID(pEvent->GetInt("attacker"));
	const bool bCrit = pEvent->GetBool("crit");

	if (attacker == I::Engine->GetLocalPlayer()) {
		PlayerInfo_t temp{};

		if (!I::Engine->GetPlayerInfo(victim, &temp)) { return; }
		if (ResolveData.find(temp.friendsID) == ResolveData.end()) { return; }
		if (Vars::Aimbot::Hitscan::AimHitbox.m_Var == 0 && !bCrit) { return; }

		ResolveData[temp.friendsID].LastHit = I::Engine->Time();
		// ResolveData[temp.friendsID].RequiresUpdate = false;
	}
}