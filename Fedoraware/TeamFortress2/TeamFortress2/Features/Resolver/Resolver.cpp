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
	if (pEntity->GetDormant() || !pEntity->IsAlive() || pEntity->IsAGhost() || pEntity->IsTaunting()) { return false; }

	if (I::GlobalVars->tickcount - mResolverData[pEntity].pLastFireAngles.first < 2) { return false; }	//	the networked angles are accurate
	if (pEntity->GetSimulationTime() == pEntity->GetOldSimulationTime()) { return false; }				//	last networked angles are the same as these, no need to change them
	return true;
}

bool PResolver::IsOnShotPitchReliable(const float flPitch){
	return fabsf(flPitch) <= 90.f;
}

float PResolver::GetRealPitch(const float flPitch){
	const float flRemainder = fmodf(flPitch, 360.f);	//	will return ((pitch/360)r * 360)

	if (flRemainder > 180.f) { return flRemainder - 360.f; }
	else if (flRemainder < -180.f){ return flRemainder + 360.f; }

	return flRemainder;
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

void PResolver::Aimbot(CBaseEntity* pEntity){
	if (abs(I::GlobalVars->tickcount - pWaiting.first) < 66) { return; }

	INetChannel* iNetChan = I::EngineClient->GetNetChannelInfo();
	if (!iNetChan) { return; }

	const int iDelay = 6 + TIME_TO_TICKS(G::LerpTime + iNetChan->GetLatency(FLOW_INCOMING) + iNetChan->GetLatency(FLOW_OUTGOING));
	pWaiting = {I::GlobalVars->tickcount + iDelay, pEntity};
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

		mResolverData[pEntity].vOriginalAngles = {pEntity->GetEyeAngles().x, pEntity->GetEyeAngles().y};

		if (!ShouldRunEntity(pEntity)) { continue; }

		Vec3 vAdjustedAngle = pEntity->GetEyeAngles();

		if (std::optional<float> flPitch = GetPitchForSniperDot(pEntity))
		{
			vAdjustedAngle.x = flPitch.value();
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
				const bool bEdge = vAdjustedAngle.x == 89 ? !F::AntiAim.FindEdge(flBaseYaw) : F::AntiAim.FindEdge(flBaseYaw);
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
	if (I::GlobalVars->tickcount > pWaiting.first && pWaiting.second) { 
		mResolverData[pWaiting.second].iYawIndex++;
		if (mResolverData[pWaiting.second].iYawIndex > 3) { mResolverData[pWaiting.second].iYawIndex = 0; }
		pWaiting = {0, nullptr};
	}
}

void PResolver::FXFireBullet(int iIndex, const Vec3 vAngles){
	CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(iIndex);
	if (!pEntity) { return; }

	Vec3 vAngAdjusted = vAngles;

	if (!IsOnShotPitchReliable(vAngles.x)){
		vAngAdjusted.x = GetRealPitch(vAngles.x);
	}

	mResolverData[pEntity].pLastFireAngles = { I::GlobalVars->tickcount, vAngAdjusted};	//	doesnt account for fakeyaw players in fov calculations BEWARE
	SetAngles(vAngAdjusted, pEntity);
}

void PResolver::OnPlayerHurt(CGameEvent* pEvent){
	const bool bLocal = I::EngineClient->GetPlayerForUserID(pEvent->GetInt("attacker")) == I::EngineClient->GetLocalPlayer();
	if (!bLocal) { return; }

	const CBaseEntity* pVictim = I::ClientEntityList->GetClientEntity(I::EngineClient->GetPlayerForUserID(pEvent->GetInt("userid")));

	if (pVictim == pWaiting.second) { pWaiting = {0, nullptr}; }
	return;
}