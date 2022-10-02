#include "Resolver.h"
#include "../AntiHack/AntiAim.h"

static std::vector YawResolves{ 0.0f, 180.0f, 65.0f, -65.0f, -180.0f };

bool CResolver::ShouldAutoResolve()
{
	if (G::CurWeaponType == EWeaponType::PROJECTILE) { return false; }

	if (const auto& pWeapon = g_EntityCache.GetWeapon())
	{
		if (pWeapon->GetClassID() == ETFClassID::CTFMinigun) { return false; }
	}

	return true;
}

void CResolver::ReportShot(int iIndex){
	CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(iIndex);
	if (!pEntity) { return; }
	CBaseCombatWeapon* pWeapon = pEntity->GetActiveWeapon();
	if (!pWeapon) { return; }
	mDidShoot[pEntity->GetIndex()] = Utils::GetWeaponType(pWeapon) == EWeaponType::HITSCAN;
}

/* Run the resolver and apply the resolved angles */
void CResolver::Run()
{
	if (!Vars::AntiHack::Resolver::Resolver.Value) { return; }

	Vec3 localHead;
	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	if (!pLocal) { return; }
	localHead = pLocal->GetEyePosition();

	UpdateSniperDots();

	for (auto i = 1; i <= I::EngineClient->GetMaxClients(); i++)
	{
		CBaseEntity* entity;
		PlayerInfo_t temp{};

		if (!(entity = I::ClientEntityList->GetClientEntity(i))) {
			continue;
		}

		if (entity->GetDormant()) {
			continue;
		}

		if (!I::EngineClient->GetPlayerInfo(i, &temp)) {
			continue;
		}

		if (!entity->GetLifeState() == LIFE_ALIVE) {
			continue;
		}

		if (entity->IsTaunting()) {
			continue;
		}

		if (mDidShoot[entity->GetIndex()]) {
			continue;
		}

		const Vector vX = entity->GetEyeAngles();
		auto* m_angEyeAnglesX = reinterpret_cast<float*>(reinterpret_cast<DWORD>(entity) + g_NetVars.
			get_offset("DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[0]"));
		auto* m_angEyeAnglesY = reinterpret_cast<float*>(reinterpret_cast<DWORD>(entity) + g_NetVars.
			get_offset("DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[1]"));

		auto findResolve = F::Resolver.ResolvePlayers.find(temp.friendsID);
		ResolveMode resolveMode;
		if (findResolve != F::Resolver.ResolvePlayers.end())
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
			// if we can resolve using the sniper dot, do that
			if (const float SniperDotYaw = ResolveSniperDot(entity)) {
				*m_angEyeAnglesX = SniperDotYaw;
				break;
			}

			// Auto (Will resolve fake up/down) (poorly)
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
		const Vec3 vAngleTo = Math::CalcAngle(entity->GetEyePosition(), localHead);
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
		case 6:	//	find edge
		{
			const Vec3 vAngleTo = Math::CalcAngle(entity->GetAbsOrigin(), pLocal->GetAbsOrigin());	//	baseyaw
			const bool bEdge = G::RealViewAngles.x == 89.f ? F::AntiAim.FindEdge(vAngleTo.y) : F::AntiAim.FindEdge(vAngleTo.y);	//	this is terrible but should work fine
			*m_angEyeAnglesY = vAngleTo.y + (bEdge ? 90 : -90);
			break;
		}
		case 7:
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
	mDidShoot.clear();
}

/* Update resolver data (for Bruteforce) */
void CResolver::Update(CUserCmd* pCmd)
{
	if (!Vars::AntiHack::Resolver::Resolver.Value) { return; }

	// Log shots
	if (const auto& pLocal = g_EntityCache.GetLocal())
	{
		if (G::CurrentTargetIdx != 0 &&
			(pCmd->buttons & IN_ATTACK || G::IsAttacking) &&
			pLocal->GetActiveWeapon()->CanShoot(pLocal))
		{
			PlayerInfo_t temp{};
			const int aimTarget = G::CurrentTargetIdx;
			
			if (const auto& pTarget = I::ClientEntityList->GetClientEntity(aimTarget))
			{
				if (I::EngineClient->GetPlayerInfo(aimTarget, &temp))
				{
					const auto findResolve = F::Resolver.ResolvePlayers.find(temp.friendsID);
					ResolveMode resolveMode;
					if (findResolve != F::Resolver.ResolvePlayers.end())
					{
						resolveMode = findResolve->second;
					}

					if (resolveMode.m_Yaw == 6)
					{
						ResolveData[temp.friendsID].LastShot = I::EngineClient->Time();
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
		if (const auto nc = I::EngineClient->GetNetChannelInfo())
		{
			delay = (nc->GetLatency(FLOW_OUTGOING) + nc->GetLatency(FLOW_INCOMING)) + 0.3f;
		}
		const float time = I::EngineClient->Time();
		const bool shouldCheck = (time - data.second.LastShot) > delay;
		const float timeDiff = data.second.LastHit - data.second.LastShot;

		if (data.second.RequiresUpdate && shouldCheck && timeDiff <= 0.f)
		{
			// Miss
			data.second.Mode += 1;
			while (data.second.Mode >= static_cast<int>(YawResolves.size()))
			{
				data.second.Mode -= YawResolves.size();
			}

			data.second.RequiresUpdate = false;
		}
	}
}

void CResolver::UpdateSniperDots() {
	SniperDotMap.clear();

	// Find sniper dots
	for (int i = I::EngineClient->GetMaxClients() + 1; i <= I::ClientEntityList->GetHighestEntityIndex(); i++)
	{
		if (CBaseEntity* eTarget = I::ClientEntityList->GetClientEntity(i)) {
			if (eTarget->GetClassID() != ETFClassID::CSniperDot)
				continue;

			if (CBaseEntity* pOwner = I::ClientEntityList->GetClientEntityFromHandle(eTarget->m_hOwnerEntity())) {
				SniperDotMap[pOwner] = eTarget;
			}
		}
	}
}

float CResolver::ResolveSniperDot(CBaseEntity* pOwner) {
	if (CBaseEntity* SniperDot = SniperDotMap[pOwner]) {
		const Vec3 DotOrigin = SniperDot->m_vecOrigin();
		const Vec3 EyePosition = pOwner->GetEyePosition();
		const Vec3 delta = DotOrigin - EyePosition;
		Vec3 angles;
		Math::VectorAngles(delta, angles);
		return angles.x;
	}
	return false;
}

/* Called when the someone was damaged. Did we hit? */
void CResolver::OnPlayerHurt(CGameEvent* pEvent)
{
	if (!Vars::AntiHack::Resolver::Resolver.Value) { return; }

	const int victim = I::EngineClient->GetPlayerForUserID(pEvent->GetInt("userid"));
	const int attacker = I::EngineClient->GetPlayerForUserID(pEvent->GetInt("attacker"));
	const bool bCrit = pEvent->GetBool("crit");

	if (attacker == I::EngineClient->GetLocalPlayer()) {
		PlayerInfo_t temp{};

		if (!I::EngineClient->GetPlayerInfo(victim, &temp)) { return; }
		if (ResolveData.find(temp.friendsID) == ResolveData.end()) { return; }
		if (Vars::Aimbot::Hitscan::AimHitbox.Value == 0 && !bCrit) { return; }

		ResolveData[temp.friendsID].LastHit = I::EngineClient->Time();
		// ResolveData[temp.friendsID].RequiresUpdate = false;
	}
}