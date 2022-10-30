#include "Resolver.h"
#include "../AntiHack/AntiAim.h"

static std::vector<float> vYawRotations{ 0.0f, 180.0f, 90.0f, -90.0f};

void PResolver::UpdateSniperDots(){
	mSniperDots.clear();
	for (int i = I::EngineClient->GetMaxClients() + 1; i <= I::ClientEntityList->GetHighestEntityIndex(); i++)
	{
		if (CBaseEntity* eTarget = I::ClientEntityList->GetClientEntity(i))
		{
			if (eTarget->GetClassID() != ETFClassID::CSniperDot)
			{
				continue;
			}

			if (CBaseEntity* pOwner = I::ClientEntityList->GetClientEntityFromHandle(eTarget->m_hOwnerEntity()))
			{
				mSniperDots[pOwner] = eTarget;
			}
		}
	}
}

std::optional<float> PResolver::GetPitchForSniperDot(CBaseEntity* pEntity){
	if (CBaseEntity* SniperDot = mSniperDots[pEntity])
	{
		const Vec3 vOrigin = SniperDot->m_vecOrigin();
		const Vec3 vEyeOrigin = pEntity->GetEyePosition();
		const Vec3 vDelta = vOrigin - vEyeOrigin;
		Vec3 vAngles;
		Math::VectorAngles(vDelta, vAngles);
		return vAngles.x;
	}
	return std::nullopt;
}

std::optional<float> PResolver::PredictBaseYaw(CBaseEntity* pEntity){
	if (I::GlobalVars->tickcount - mResolverData[pEntity].pLastFireAngles.first > 66 || !mResolverData[pEntity].pLastFireAngles.first) { //staleness & validity check
		CBaseEntity* pLocal = g_EntityCache.GetLocal();
		if (!pLocal) { return std::nullopt; }
		if (!pLocal->IsAlive() || pLocal->IsAGhost()) { return std::nullopt; }
		return Math::CalcAngle(pEntity->GetAbsOrigin(), pLocal->GetAbsOrigin()).y;
	}

	bool bFound = false;
	float flSmallestAngleTo = 0.f; float flSmallestFovTo = 360.f;
	for (CBaseEntity* pTarget : g_EntityCache.GetGroup(EGroupType::PLAYERS_ALL))
	{
		if (!pTarget || pTarget->IsAGhost() || !pTarget->IsAlive() || pTarget->GetTeamNum() == pEntity->GetTeamNum()) { continue; }
		const Vec3 vAngleTo = Math::CalcAngle(pEntity->GetAbsOrigin(), pTarget->GetAbsOrigin());
		const float flFOVTo = Math::CalcFov(mResolverData[pEntity].pLastFireAngles.second, vAngleTo);
	
		if (flFOVTo < flSmallestFovTo) { bFound = true; flSmallestAngleTo = vAngleTo.y; flSmallestFovTo = flFOVTo; }
	}

	if (!bFound) { return std::nullopt; }

	return flSmallestAngleTo;
}

bool PResolver::ShouldRun(){
	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	if (!pLocal) { return false; }
	if (!(pLocal->IsAlive() && !pLocal->IsAGhost() && Vars::AntiHack::Resolver::Resolver.Value)) { return false; }
	if (G::CurWeaponType != EWeaponType::HITSCAN) { return false; }
	return true;
}

bool PResolver::ShouldRunEntity(CBaseEntity* pEntity){
	if (!pEntity->OnSolid() && Vars::AntiHack::Resolver::IgnoreAirborne.Value) { return false; }
	if (!pEntity->IsAlive() || pEntity->IsAGhost() || pEntity->IsTaunting()) { return false; }

	//if (pEntity->GetSimulationTime() == pEntity->GetOldSimulationTime()) { return false; }				//	last networked angles are the same as these, no need to change them
	return true;
}

bool PResolver::KeepOnShot(CBaseEntity* pEntity){
	return abs(I::GlobalVars->tickcount - mResolverData[pEntity].pLastFireAngles.first) < 2;
}

bool PResolver::IsOnShotPitchReliable(const float flPitch){
	if (flPitch > 180) {
		return flPitch > 273.f;
	}
	else {
		return flPitch < 87.f;
	}
}

float PResolver::GetRealPitch(const float flPitch){
	if (flPitch < 157.5f) { return 89.f; }
	else { return -89.f; }
}

void PResolver::SetAngles(const Vec3 vAngles, CBaseEntity* pEntity){
	if (CTFPlayerAnimState* pAnimState = pEntity->GetAnimState()){
		float* flPitch = reinterpret_cast<float*>(reinterpret_cast<DWORD>(pEntity) + g_NetVars.get_offset("DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[0]"));

		*flPitch = vAngles.x;
		pAnimState->m_flCurrentFeetYaw = vAngles.y;
		pAnimState->m_flGoalFeetYaw = vAngles.y;
		pAnimState->Update(vAngles.y, vAngles.x);
	}
}

int PResolver::GetPitchMode(CBaseEntity* pEntity){
	PlayerInfo_t pInfo{};
	if (!I::EngineClient->GetPlayerInfo(pEntity->GetIndex(), &pInfo)) { return 0; }
	return mResolverMode[pInfo.friendsID].first;
}

int PResolver::GetYawMode(CBaseEntity* pEntity){
	PlayerInfo_t pInfo{};
	if (!I::EngineClient->GetPlayerInfo(pEntity->GetIndex(), &pInfo)) { return 0; }
	return mResolverMode[pInfo.friendsID].second;
}

void PResolver::OnDormancy(CBaseEntity* pEntity){
	mResolverData[pEntity].pLastSniperPitch = {0, 0.f};
	mResolverData[pEntity].flPitchNoise = 0.f;
	mResolverData[pEntity].iPitchNoiseSteps = 0;
	mResolverData[pEntity].pLastFireAngles = {0, {}};
	mResolverData[pEntity].vOriginalAngles = {};
}

void PResolver::Aimbot(CBaseEntity* pEntity, const bool bHeadshot){
	if (abs(I::GlobalVars->tickcount - pWaiting.first) < 66) { return; }

	INetChannel* iNetChan = I::EngineClient->GetNetChannelInfo();
	if (!iNetChan) { return; }

	const int iDelay = 6 + TIME_TO_TICKS(G::LerpTime + iNetChan->GetLatency(FLOW_INCOMING) + iNetChan->GetLatency(FLOW_OUTGOING));
	pWaiting = {I::GlobalVars->tickcount + iDelay, {pEntity, bHeadshot}};
}

void PResolver::FrameStageNotify(){
	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	if (!pLocal) { return; }
	if (!ShouldRun()) { return; }

	UpdateSniperDots();

	for (auto i = 1; i <= I::EngineClient->GetMaxClients(); i++)
	{
		CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(i);
		if (!pEntity) { continue; }

		if (pEntity->GetDormant()) { OnDormancy(pEntity); continue; }

		mResolverData[pEntity].vOriginalAngles = {pEntity->GetEyeAngles().x, pEntity->GetEyeAngles().y};

		if (!ShouldRunEntity(pEntity)) { continue; }
		if (KeepOnShot(pEntity)) { SetAngles(mResolverData[pEntity].pLastFireAngles.second, pEntity); continue; }

		Vec3 vAdjustedAngle = pEntity->GetEyeAngles();

		if (std::optional<float> flPitch = GetPitchForSniperDot(pEntity))
		{
			vAdjustedAngle.x = flPitch.value();

			//	get noise
			if (mResolverData[pEntity].pLastSniperPitch.first){
				const float flNoise = mResolverData[pEntity].pLastSniperPitch.second - flPitch.value();
				mResolverData[pEntity].flPitchNoise *= mResolverData[pEntity].iPitchNoiseSteps;
				mResolverData[pEntity].flPitchNoise += flNoise;
				mResolverData[pEntity].iPitchNoiseSteps++;
				mResolverData[pEntity].flPitchNoise /= mResolverData[pEntity].iPitchNoiseSteps;
			}

			mResolverData[pEntity].pLastSniperPitch = {I::GlobalVars->tickcount, flPitch.value()};
		}
		else if (I::GlobalVars->tickcount - mResolverData[pEntity].pLastSniperPitch.first < 66 && mResolverData[pEntity].flPitchNoise < 5.f) {
			vAdjustedAngle.x = mResolverData[pEntity].pLastSniperPitch.second;
		}
		else {
			switch (GetPitchMode(pEntity)){
			case 0: break;
			case 1: vAdjustedAngle.x = -89.f; break;				//up
			case 2: vAdjustedAngle.x = 89.f; break;					//down
			case 3: vAdjustedAngle.x = 0.f; break;					//zero
			case 4: {												//auto
				if (mResolverData[pEntity].vOriginalAngles.x >= 90.f) { vAdjustedAngle.x = -89.f; }
				else if (mResolverData[pEntity].vOriginalAngles.x <= -90.f) { vAdjustedAngle.x = 89.f; }
				break;
			}
			}
		}

		const int iYawMode = GetYawMode(pEntity);
		if (iYawMode){
			std::optional<float> flTempYaw = PredictBaseYaw(pEntity);
			if (!flTempYaw) { flTempYaw = Math::CalcAngle(pEntity->GetAbsOrigin(), pLocal->GetAbsOrigin()).y; }

			const float flBaseYaw = flTempYaw.value();

			switch (iYawMode){
			case 0: break;
			case 1: vAdjustedAngle.y = flBaseYaw; break;			//forward
			case 2: vAdjustedAngle.y = flBaseYaw + 180.f; break;	//backwards
			case 3: vAdjustedAngle.y = flBaseYaw - 90.f; break;		//side1
			case 4: vAdjustedAngle.y = flBaseYaw + 90.f; break;		//side2
			case 5: vAdjustedAngle.y += 180.f; break;				//invert
			case 6:{												//edge
				const bool bEdge = vAdjustedAngle.x == 89 ? !F::AntiAim.FindEdge(flBaseYaw, pEntity) : F::AntiAim.FindEdge(flBaseYaw, pEntity);
				vAdjustedAngle.y = flBaseYaw + (bEdge ? 90.f : -90.f);
				break;
			}
			case 7:{												//auto
				vAdjustedAngle.y = vYawRotations[mResolverData[pEntity].iYawIndex];
				break;
			}
			}
		}

		SetAngles(vAdjustedAngle, pEntity);
	}
}

void PResolver::CreateMove(){
	if (I::GlobalVars->tickcount > pWaiting.first && pWaiting.second.first) { 
		mResolverData[pWaiting.second.first].iYawIndex++;
		if (mResolverData[pWaiting.second.first].iYawIndex > 3) { mResolverData[pWaiting.second.first].iYawIndex = 0; }
		pWaiting = {0, {nullptr, false}};
	}
}

void PResolver::FXFireBullet(int iIndex, const Vec3 vAngles){
	CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(iIndex);
	if (!pEntity) { return; }

	Vec3 vAngAdjusted = vAngles;
	Vec3 vAngStore = vAngles;

	if (!IsOnShotPitchReliable(vAngles.x)){
		float flAdjustedPitch = vAngles.x;

		vAngStore.x += 360.f;
		vAngStore.x *= -1;

		vAngAdjusted.x = GetRealPitch(flAdjustedPitch);

		if (fabsf(flAdjustedPitch) > 89.f) { vAngStore.y += 180; }	//	account for likely yaw faking
		while (vAngStore.y > 360) { vAngStore.y -= 360.f; }	//	hacky fix for previous line
		vAngStore.x += 540;	//	(360+180)
		Utils::ConLog("Resolver", tfm::format("sent %.1f %.1f", vAngles.x, vAngles.y).c_str(), {0, 222, 255, 255});
		Utils::ConLog("Resolver", tfm::format("adjusted %.1f %.1f", vAngAdjusted.x, vAngAdjusted.y).c_str(), {0, 222, 255, 255});
		Utils::ConLog("Resolver", tfm::format("adjusted 2 %.1f %.1f", vAngStore.x, vAngStore.y).c_str(), {0, 222, 255, 255});
	}

	mResolverData[pEntity].pLastFireAngles = { I::GlobalVars->tickcount, vAngStore};
	SetAngles(vAngAdjusted, pEntity);
}

void PResolver::OnPlayerHurt(CGameEvent* pEvent){
	const bool bLocal = I::EngineClient->GetPlayerForUserID(pEvent->GetInt("attacker")) == I::EngineClient->GetLocalPlayer();
	if (!bLocal) { return; }

	const CBaseEntity* pVictim = I::ClientEntityList->GetClientEntity(I::EngineClient->GetPlayerForUserID(pEvent->GetInt("userid")));

	if (pVictim == pWaiting.second.first) { 
		if (pWaiting.second.second && G::WeaponCanHeadShot){	//	should be headshot
			const bool bCrit = pEvent->GetBool("crit");
			if (!bCrit) { return; }
		}
		pWaiting = {0, {nullptr, false}}; 
	}
	return;
}