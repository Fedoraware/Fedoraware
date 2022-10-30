#include "CheaterDetection.h"

bool CCheaterDetection::ShouldScan()
{
	const int iProtFlags = Vars::Misc::CheaterDetection::Protections.Value;
	const int iDetectFlags = Vars::Misc::CheaterDetection::Methods.Value;

	if (I::EngineClient->IsPlayingTimeDemo()) { return false; }

	if (!Vars::Misc::CheaterDetection::Enabled.Value && !iDetectFlags) { return false; }

	if (iLastScanTick == I::GlobalVars->tickcount && iProtFlags & (1 << 2)) { return false; }

	if (flLastFrameTime)
	{
		const float flRealFrameTime = I::GlobalVars->realtime - flLastFrameTime;
		flLastFrameTime = I::GlobalVars->realtime;
		const int iRealFPS = static_cast<int>(1.0f / flRealFrameTime);

		if (iRealFPS < server.iTickRate && iProtFlags & (1 << 1))
		{
			return false;
		}
	}

	if (const INetChannel* NetChannel = I::EngineClient->GetNetChannelInfo())
	{
		const float flLastReceive = NetChannel->GetTimeSinceLastReceived();
		const float flMaxReceive = I::GlobalVars->interval_per_tick * 2;
		const bool bIsTimeOut = NetChannel->IsTimingOut();
		const bool bShouldSkip = (flLastReceive > flMaxReceive) || bIsTimeOut;
		if (bShouldSkip && iProtFlags & (1 << 0)) { return false; }
	}

	return true;
}

bool CCheaterDetection::ShouldScanEntity(CBaseEntity* pEntity)
{
	const int iIndex = pEntity->GetIndex();

	if (fabsf(I::GlobalVars->curtime - mData[pEntity].flJoinTime) < 10.f) { return false; }	//	if they haven't been in the server for 10 seconds dont scan them

	// dont scan invalid players
	if (pEntity->IsAlive() || pEntity->IsAGhost() || pEntity->IsTaunting()) { return false; }

	// dont scan players that arent simulated this tick
	if (pEntity->GetSimulationTime() == pEntity->GetOldSimulationTime()) { return false; }

	// dont scan if we can't get playerinfo
	PlayerInfo_t pInfo{};
	if (!I::EngineClient->GetPlayerInfo(iIndex, &pInfo)) { return false; }

	// dont scan ignored players, friends, or players already marked as cheaters
	switch (G::PlayerPriority[pInfo.friendsID].Mode)
	{
		case 0: case 1: case 4:
		{ return false; }
	}

	if (!mData[pEntity].flJoinTime) { mData[pEntity].flJoinTime = I::GlobalVars->curtime; return false; }

	return true;
}

bool CCheaterDetection::IsPitchLegal(CBaseEntity* pSuspect)
{
	const Vec3 vAngles = pSuspect->GetEyeAngles();
	return fabsf(vAngles.x) < 90.f;
}

void CCheaterDetection::ReportTickCount(std::pair<CBaseEntity*, int> pReport)
{
	if (Vars::Misc::CheaterDetection::Methods.Value & (1 << 3))
	{
		if (mData[pReport.first].pChoke.first = 0) { mData[pReport.first].pChoke.second = pReport.second; return; }
		mData[pReport.first].pChoke.first++;
		mData[pReport.first].pChoke.second += pReport.second;
	}
}

bool CCheaterDetection::CheckBHop(CBaseEntity* pEntity)
{
	const bool bOnGround = pEntity->OnSolid();	//	NOTE: groundentity isn't networked properly sometimes i think
	if (bOnGround) { mData[pEntity].pBhop.first++; }
	else if (mData[pEntity].pBhop.first <= Vars::Misc::CheaterDetection::BHopMaxDelay.Value && mData[pEntity].pBhop.first > 0) { mData[pEntity].pBhop.second++; mData[pEntity].pBhop.first = 0; }
	else { mData[pEntity].pBhop = { 0, 0 }; }

	if (mData[pEntity].pBhop.second >= Vars::Misc::CheaterDetection::BHopDetectionsRequired.Value) { mData[pEntity].iPlayerSuspicion++; mData[pEntity].pBhop = { 0, 0 }; return true; }
	return false;
}

bool CCheaterDetection::AreAnglesSuspicious(CBaseEntity* pEntity)
{
// first check if we should scan this player at all
	if (G::ChokeMap[pEntity->GetIndex()] > 0 || pEntity->GetVelocity().Length() < 10.f || mData[pEntity].vLastAngle.IsZero()) { mData[pEntity].pTrustAngles = { false, {0, 0} }; return false; } //	angles don't update the way I WANT them to if the player is not moving.
	if (!mData[pEntity].pTrustAngles.first)
	{	//	we are not suspicious of this player yet
		const Vec3 vCurAngles = pEntity->GetEyeAngles();
		const Vec2 vDelta = Vec2{ vCurAngles.x, vCurAngles.y } - mData[pEntity].vLastAngle;
		const float flDelta = vDelta.Length();

		if (flDelta > (Vars::Misc::CheaterDetection::MinimumFlickDistance.Value)) { mData[pEntity].pTrustAngles = { true, {vCurAngles.x, vCurAngles.y} }; }
	}
	else
	{
  //	check for noise on this player (how much their mouse moves after the initial flick)
		const Vec3 vCurAngles = pEntity->GetEyeAngles();
		const Vec2 vDelta = Vec2{ vCurAngles.x, vCurAngles.y } - mData[pEntity].pTrustAngles.second;
		const float flDelta = vDelta.Length();

		if (flDelta < (Vars::Misc::CheaterDetection::MaximumNoise.Value * server.flMultiplier)) { mData[pEntity].pTrustAngles = { false, {0, 0} }; return true; }
		else { mData[pEntity].pTrustAngles = { false, {0, 0} }; }
	}
	return false;
}

void CCheaterDetection::AimbotCheck(CBaseEntity* pEntity)
{
	const Vec3 vCurAngle = pEntity->GetEyeAngles();
	const Vec2 vCurAngle2 = { vCurAngle.x, vCurAngle.y };
	const float flDelta = vCurAngle.DistTo(mData[pEntity].vLastAngle);	//	aimbot flick (unscaled)
	const float flScaled = std::clamp(flDelta * G::ChokeMap[pEntity->GetIndex()], 0.f, Vars::Misc::CheaterDetection::MaxScaledAimbotFoV.Value);	//	aimbot flick scaled

	if (flScaled > Vars::Misc::CheaterDetection::MinimumAimbotFoV.Value)
	{	//	this person is aimbotting in our eyes.
		PlayerInfo_t pInfo{};
		if (!I::EngineClient->GetPlayerInfo(pEntity->GetIndex(), &pInfo)) { return; }

		mData[pEntity].iPlayerSuspicion++;
		Utils::ConLog("CheaterDetection", tfm::format("%s infracted for aimbot.", pInfo.name).c_str(), { 224, 255, 131, 255 });
	}
}

bool CCheaterDetection::IsDuckSpeed(CBaseEntity* pEntity)
{
	const bool bIsDucking = pEntity->IsDucking() && pEntity->OnSolid();	//	this may break on movement sim?? (ty spook >:C)
	if (!bIsDucking) { mData[pEntity].pDuckInfo = { 0, 0 }; return false; }
	if (mData[pEntity].pDuckInfo.first == -1) { return false; }	//	dont double infract

	if (TICKS_TO_TIME(mData[pEntity].pDuckInfo.first) > 1.f)
	{	//	only check once we have a full second of data.
		const float flAvg = mData[pEntity].pDuckInfo.second / mData[pEntity].pDuckInfo.first;
		if (flAvg > pEntity->GetMaxSpeed() * .5)
		{	//	this is meant to be exactly 1/3rd but since we're averaging we should be nicer...
			mData[pEntity].pDuckInfo.first = -1;
			return true;
		}
	}
	else
	{
		mData[pEntity].pDuckInfo.first++;
		mData[pEntity].pDuckInfo.second += pEntity->GetVelocity().Length2D();
	}
	return false;
}

void CCheaterDetection::SimTime(CBaseEntity* pEntity)
{
	if (Vars::Misc::CheaterDetection::Methods.Value & ~(1 << 2))
	{
		if (mData[pEntity].iNonDormantCleanQueries < 6) { return; }
		const float flSimDelta = pEntity->GetSimulationTime() - pEntity->GetOldSimulationTime();
		const int iTickDelta = TIME_TO_TICKS(flSimDelta);
		if (mData[pEntity].pChoke.first = 0) { mData[pEntity].pChoke.second = iTickDelta; return; }
		mData[pEntity].pChoke.first++;
		mData[pEntity].pChoke.second += iTickDelta;
	}
	return;
}

void CCheaterDetection::OnDormancy(CBaseEntity* pEntity)
{
	mData[pEntity].pTrustAngles = { false, {0, 0} };
	mData[pEntity].iNonDormantCleanQueries = 0;
	mData[pEntity].vLastAngle = {};
	mData[pEntity].pBhop = { false, 0 };
	mFired[pEntity] = false;
}

void CCheaterDetection::OnTick()
{
	const auto pLocal = g_EntityCache.GetLocal();
	if (!pLocal || !I::EngineClient->IsConnected() || G::ShouldShift || !ShouldScan()) { return; }
	iLastScanTick = I::GlobalVars->tickcount;
	flScanningTime = I::GlobalVars->curtime - flFirstScanTime;
	FindScores();
	FindHitchances();

	const int iDetectionFlags = Vars::Misc::CheaterDetection::Methods.Value;

	for (int n = 1; n < I::EngineClient->GetMaxClients(); n++)
	{
		CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(n);
		if (!pEntity) { continue; }
		if (pEntity->GetDormant()) { OnDormancy(pEntity); continue; }
		if (!ShouldScanEntity(pEntity)) { OnDormancy(pEntity); continue; }
		if (pEntity == pLocal) { continue; }	//	i think for this code to run the local player has to be cheating anyway :thinking:

		PlayerInfo_t pInfo{};
		if (!I::EngineClient->GetPlayerInfo(pEntity->GetIndex(), &pInfo)) { continue; }

		CalculateHitChance(pEntity);
		SimTime(pEntity);

		if (iDetectionFlags & (1 << 6) && !IsPitchLegal(pEntity)) { mData[pEntity].iPlayerSuspicion = Vars::Misc::CheaterDetection::SuspicionGate.Value; Utils::ConLog("CheaterDetection", tfm::format("%s marked for OOB angles.", pInfo.name).c_str(), { 224, 255, 131, 255 }); }
		if (iDetectionFlags & (1 << 5) && AreAnglesSuspicious(pEntity)) { mData[pEntity].iPlayerSuspicion++; Utils::ConLog("CheaterDetection", tfm::format("%s infracted for suspicious angles.", pInfo.name).c_str(), { 224, 255, 131, 255 }); }
		if (iDetectionFlags & (1 << 4) && CheckBHop(pEntity)) { Utils::ConLog("CheaterDetection", tfm::format("%s infracted for bunny-hopping.", pInfo.name).c_str(), { 224, 255, 131, 255 }); }
		if (iDetectionFlags & (1 << 8) && IsDuckSpeed(pEntity)) { Utils::ConLog("CheaterDetection", tfm::format("%s infracted for suspected duck speed.", pInfo.name).c_str(), { 224, 255, 131, 255 }); mData[pEntity].iPlayerSuspicion++; }

		//analytical analysis
		if (mData[pEntity].pChoke.second && mData[pEntity].pChoke.first) { if (((float)mData[pEntity].pChoke.second / (float)mData[pEntity].pChoke.first) > (Vars::Misc::CheaterDetection::PacketManipGate.Value / server.flMultiplier) && mData[pEntity].pChoke.first > 10) { mData[pEntity].iPlayerSuspicion++; Utils::ConLog("CheaterDetection", tfm::format("%s infracted for high avg packet choking {%.1f / %.1f}.", pInfo.name, mData[pEntity].pChoke.second, (14.f / server.flMultiplier)).c_str(), { 224, 255, 131, 255 }); mData[pEntity].pChoke = { 0, 0.f }; } }
		if (flScanningTime > 60.f)
		{
			if (iDetectionFlags & (1 << 0) && mData[pEntity].flHitchance && server.flHighAccuracy && mData[pEntity].pShots.first > 25) { if (mData[pEntity].flHitchance > (server.flHighAccuracy) && !mData[pEntity].pDetections.first) { mData[pEntity].iPlayerSuspicion += 5; Utils::ConLog("CheaterDetection", tfm::format("%s infracted for extremely high accuracy {%.1f / %.1f}.", pInfo.name, mData[pEntity].flHitchance, server.flHighAccuracy).c_str(), { 224, 255, 131, 255 }); } }
			if (iDetectionFlags & (1 << 1) && mData[pEntity].flScorePerSecond && server.flAverageScorePerSecond) { if (mData[pEntity].flScorePerSecond > (std::max(server.flAverageScorePerSecond, server.flFloorScore) * Vars::Misc::CheaterDetection::ScoreMultiplier.Value) && !mData[pEntity].pDetections.second) { mData[pEntity].iPlayerSuspicion += 5; Utils::ConLog("CheaterDetection", tfm::format("%s infracted for extremely high score per second {%.1f / %.1f}.", pInfo.name, mData[pEntity].flScorePerSecond, server.flAverageScorePerSecond).c_str(), { 224, 255, 131, 255 }); } }
		}

		const Vec3 vAngles = pEntity->GetEyeAngles();
		mData[pEntity].vLastAngle = { vAngles.x, vAngles.y };
		if (mData[pEntity].iPlayerSuspicion >= Vars::Misc::CheaterDetection::SuspicionGate.Value) { mData[pEntity].iPlayerSuspicion = 0; G::PlayerPriority[pInfo.friendsID].Mode = 4; }
	}
}

void CCheaterDetection::FillServerInfo()
{
	server.flAverageScorePerSecond = 0.f;
	server.iTickRate = 1.f / I::GlobalVars->interval_per_tick;
	server.flMultiplier = 66.7 / server.iTickRate;
	Utils::ConLog("CheaterDetection[UTIL]", tfm::format("Calculated server tickrate & created appropriate multiplier {%.1f | %.1f}.", server.iTickRate, server.flMultiplier).c_str(), { 224, 255, 131, 255 });
}

void CCheaterDetection::FindScores()
{	//	runs every 30 seconds
	if (I::GlobalVars->curtime - flLastScoreTime < 30.f) { return; }
	CTFPlayerResource* cResource = g_EntityCache.GetPR();
	if (!cResource) { return; }
	flLastScoreTime = I::GlobalVars->curtime;

	float flTotalAvg = 0;
	int iTotalPlayers = 0;
	for (int n = 1; n < I::EngineClient->GetMaxClients(); n++)
	{
		CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(n);
		if (!pEntity || !cResource->GetValid(pEntity->GetIndex())) { continue; }
		const float iScore = cResource->GetScore(pEntity->GetIndex()) / cResource->GetConnectionTime(pEntity->GetIndex());
		flTotalAvg += iScore; iTotalPlayers++;	//	used for calculating the average score
	}

	if (!flTotalAvg || !iTotalPlayers) { return; }

	// now that we've gone through all players (including local) find the avg
	server.flAverageScorePerSecond = (flTotalAvg / (float)iTotalPlayers);
	Utils::ConLog("CheaterDetection[UTIL]", tfm::format("Calculated avg. server score per second at %.1f.", server.flAverageScorePerSecond).c_str(), { 224, 255, 131, 255 });
}

void CCheaterDetection::FindHitchances()
{	//	runs every 30 seconds
	if (I::GlobalVars->curtime - flLastAccuracyTime < 30.f) { return; }
	flLastAccuracyTime = I::GlobalVars->curtime;

	const float flAvg = (float)server.iHits / (float)server.iMisses;
	server.flHighAccuracy = std::clamp(flAvg * 2, .05f, .95f);

	Utils::ConLog("CheaterDetection[UTIL]", tfm::format("Calculated server hitchance data {%.5f | %.5f}", flAvg, server.flHighAccuracy).c_str(), { 224, 255, 131, 255 });
}

void CCheaterDetection::Reset()
{
	server = ServerData{};
	mData.clear();
	mFired.clear();
	iLastScanTick = 0;
	flLastFrameTime = 0.f;
	flFirstScanTime = 0.f;
	flScanningTime = 0.f;
	flLastScoreTime = 0.f;
	flLastAccuracyTime = 0.f;
}

void CCheaterDetection::OnLoad()
{
	Reset();
	FillServerInfo();
	flLastAccuracyTime = I::GlobalVars->curtime; flLastScoreTime = I::GlobalVars->curtime;
}

void CCheaterDetection::CalculateHitChance(CBaseEntity* pEntity)
{
	mData[pEntity].flHitchance = (float)mData[pEntity].pShots.first / (float)mData[pEntity].pShots.second;
}

void CCheaterDetection::ReportShot(int iIndex)
{
	CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(iIndex);
	if (!pEntity) { return; }
	mData[pEntity].pShots.second++;
	server.iMisses++;
}

void CCheaterDetection::ReportDamage(CGameEvent* pEvent)
{
	const int iIndex = pEvent->GetInt("attacker");
	CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(iIndex);
	if (!pEntity || pEntity->GetDormant()) { return; }
	CBaseCombatWeapon* pWeapon = pEntity->GetActiveWeapon();
	if (!pWeapon || Utils::GetWeaponType(pWeapon) != EWeaponType::HITSCAN) { return; }
	AimbotCheck(pEntity);
	if (I::GlobalVars->tickcount - mData[pEntity].iLastDamageEventTick <= 1) { return; }
	mData[pEntity].iLastDamageEventTick = I::GlobalVars->tickcount;
	mData[pEntity].pShots.first++; mData[pEntity].bDidDamage = true;
	server.iHits++;
}

#include "../../Hooks/Hooks.h"

MAKE_HOOK(asdfaasfsda, g_Pattern.Find(L"client.dll", L"55 8B EC 81 EC ? ? ? ? 56 8B 75 08 8D 8D"), void, __fastcall,
		  void* ecx, void* edx, int* pTrace, int iDamageType, const char* pCustomImpactName)
{
	Hook.Original<FN>()(ecx, edx, pTrace, iDamageType, pCustomImpactName);
}