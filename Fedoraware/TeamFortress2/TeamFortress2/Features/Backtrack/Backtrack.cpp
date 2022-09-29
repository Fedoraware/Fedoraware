#include "Backtrack.h"

bool CBacktrackNew::IsTracked(TickRecordNew Record){
	return I::GlobalVars->curtime - Record.flCreateTime < 1.f;
}


//	this shit and doesn't make sense.
//bool CBacktrackNew::IsBackLagComped(CBaseEntity* pEntity){
//	if (mRecords[pEntity].size() < 2) { return true; }	//	we dont have enough to compare, this will never happen in normal gameplay
//	const Vec3 vBackRecordOrigin = mRecords[pEntity].back().vOrigin;
//	const Vec3 vNextRecordOrigin = mRecords[pEntity].at(mRecords[pEntity].size() - 1).vOrigin;
//	const Vec3 vDelta = vBackRecordOrigin - vNextRecordOrigin;
//	return vDelta.Length2DSqr() < 4096.f;
//}

bool CBacktrackNew::WithinRewind(TickRecordNew Record){	//	check if we can go to this tick, ie, within 200ms of us
	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	INetChannel* iNetChan = I::EngineClient->GetNetChannelInfo();
	
	if (!pLocal || !iNetChan) { return false; }

	const float flDelay = std::clamp(GetLatency(), 0.f, 1.f);	//	TODO:: sv_maxunlag
	const float flDelta = flDelay - TICKS_TO_TIME(pLocal->m_nTickBase() - Record.iTickCount);

	return fabsf(flDelta) < .2f;	//	in short, check if the record is +- 200ms from us
}

void CBacktrackNew::CleanRecords(){
	for (int n = 1; n < I::ClientEntityList->GetHighestEntityIndex(); n++)
	{
		CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(n);
		if (!pEntity) { continue; }
		
		if (pEntity->GetDormant() || !pEntity->IsAlive() || !pEntity->IsPlayer()) { mRecords[pEntity].clear(); continue; }
		else if (mRecords[pEntity].size() < 1) { continue; }
		if (!IsTracked(mRecords[pEntity].back())){ mRecords[pEntity].pop_back(); }
		if (mRecords[pEntity].size() > 66) { mRecords[pEntity].pop_back(); }
	}
}

void CBacktrackNew::MakeRecords(){
	const float flCurTime = I::GlobalVars->curtime;
	const int iTickcount = I::GlobalVars->tickcount;
	for (int n = 1; n < I::ClientEntityList->GetHighestEntityIndex(); n++)
	{
		CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(n);
		if (!pEntity) { continue; }
		if (!pEntity->IsPlayer()) { return; }
		const float flSimTime = pEntity->GetSimulationTime(), flOldSimTime = pEntity->GetOldSimulationTime();
		
		//Utils::ConLog("LagCompensation", tfm::format("SimTime = %.1f\nOldSimTime = %.1f", flSimTime, flOldSimTime).c_str(), {255, 0, 0, 255});

		if (flSimTime != flOldSimTime){	//	create record on simulated players
			//Utils::ConLog("LagCompensation", "Setting Up Bones", {255, 0, 0, 255});
			matrix3x4 bones[128];
			if (!pEntity->SetupBones(bones, 128, BONE_USED_BY_ANYTHING, flSimTime)) { continue; }
			//Utils::ConLog("LagCompensation", "Creating Record", {255, 0, 0, 255});
			const Vec3 vOrigin = pEntity->GetAbsOrigin();
			if (mRecords[pEntity].size()){	// as long as we have 1 record we can check for lagcomp breaking here
				const Vec3 vPrevOrigin = mRecords[pEntity].front().vOrigin;
				const Vec3 vDelta = vOrigin - vPrevOrigin;
				if (vDelta.Length2DSqr() > 4096.f) { Utils::ConLog("LagCompensation", "Cleared borked records", {255, 0, 0, 255}); mRecords[pEntity].clear(); }
			}

			mRecords[pEntity].push_front({
				flSimTime,
				flCurTime,
				iTickcount,
				mDidShoot[pEntity->GetIndex()],
				*reinterpret_cast<BoneMatrixes*>(&bones),
				vOrigin,
				pEntity->GetAbsAngles(),
			});
		}

		//cleanup
		mDidShoot[pEntity->GetIndex()] = false;
		if (mRecords[pEntity].size() > 66){ /*Utils::ConLog("LagCompensation", "Manually removed tick record", {255, 0, 0, 255});*/ mRecords[pEntity].pop_back(); }	//	schizoid check
	}
}

// Store the last 2048 sequences
void CBacktrackNew::UpdateDatagram()
{
	if (INetChannel* INetChan = I::EngineClient->GetNetChannelInfo())
	{
		if (INetChan->m_nInSequenceNr > iLastInSequence)
		{
			iLastInSequence = INetChan->m_nInSequenceNr;
			dSequences.push_front(CIncomingSequence(INetChan->m_nInReliableState, INetChan->m_nInSequenceNr, I::GlobalVars->realtime));
		}

		if (dSequences.size() > 2048)
		{
			dSequences.pop_back();
		}
	}
}

// Returns the current (custom) backtrack latency
float CBacktrackNew::GetLatency()
{
	if (INetChannel* iNetChan = I::EngineClient->GetNetChannelInfo()){
		const float flRealLatency = iNetChan->GetLatency(FLOW_OUTGOING) + G::LerpTime;
		const float flFakeLatency = (float)std::clamp(Vars::Backtrack::Latency.Value, 0, 800) / 1000.f;
		const float flAdjustedLatency = std::clamp((flRealLatency + (bFakeLatency ? flFakeLatency : 0.f)), 0.f, 1.f);
		return flAdjustedLatency;
	}

	return 0.f;	//	we failed to get net channel and therefor have no latency.
}

void CBacktrackNew::PlayerHurt(CGameEvent* pEvent){
	const int iIndex = I::EngineClient->GetPlayerForUserID(pEvent->GetInt("userid"));
	if (CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(iIndex)){
		mRecords[pEntity].clear();	//	bone cache has gone to poop for this entity, they must be cleansed in holy fire :smiling_imp:
	}
}

void CBacktrackNew::Restart(){
	mRecords.clear(); 
	dSequences.clear();
	flLatencyRampup = 0.f;
	iLastInSequence = 0;
}

void CBacktrackNew::FrameStageNotify(){
	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	INetChannel* iNetChan = I::EngineClient->GetNetChannelInfo();
	if (!pLocal || !iNetChan) { return Restart(); }

	flLatencyRampup = std::min(1.f, flLatencyRampup += I::GlobalVars->interval_per_tick);
	UpdateDatagram();
	MakeRecords();
	CleanRecords();
}

void CBacktrackNew::ReportShot(int iIndex, void* pWpn){
	if (CBaseCombatWeapon* pWeapon = reinterpret_cast<CBaseCombatWeapon*>(pWpn)){
		mDidShoot[iIndex] = Utils::GetWeaponType(pWeapon) == EWeaponType::HITSCAN;
	}
}

std::optional<TickRecordNew> CBacktrackNew::Run(CUserCmd* pCmd, bool bAimbot = false, CBaseEntity* pEntity = nullptr){
	static int iOldTickCount = I::GlobalVars->tickcount;
	const int iCurTickCount = I::GlobalVars->tickcount;
	static int iLastIndex = 0;
	static CBaseEntity* oldEnt = pEntity; 

	if (pEntity){ if (pEntity != oldEnt) { iLastIndex = 0; oldEnt = pEntity; } }
	if (iOldTickCount != iCurTickCount) { iLastIndex = 0; iOldTickCount = iCurTickCount; }

	if (bAimbot && pEntity){	//	if we're aimbotting we don't care what record we get
		if (mRecords[pEntity].empty()) { return std::nullopt; }
		std::optional<TickRecordNew> rReturnRecord = std::nullopt;
		for (const auto& rCurQuery : mRecords[pEntity]){
			if (!IsTracked(rCurQuery) || !WithinRewind(rCurQuery)) { continue; }	//	this record is borked :D
			if (rCurQuery.bOnShot) { rReturnRecord = rCurQuery; break; }	//	this record is an onshot record
			rReturnRecord = rCurQuery;	//	nothing special, continue
		}
		return rReturnRecord;
	}
	else{
		const Vec3 vAngles = pCmd->viewangles;
	}
	return std::nullopt;
}

std::deque<TickRecordNew>* CBacktrackNew::GetRecords(CBaseEntity* pEntity){
	if (mRecords[pEntity].empty())
	{
		return nullptr;
	}

	return &mRecords[pEntity];
}

std::optional<TickRecordNew> CBacktrackNew::GetLastRecord(CBaseEntity* pEntity){
	if (mRecords[pEntity].empty()) { return std::nullopt; }
	std::optional<TickRecordNew> rReturnRecord = std::nullopt;
	for (const auto& rCurQuery : mRecords[pEntity]){
		if (!IsTracked(rCurQuery) || !WithinRewind(rCurQuery)) { continue; }
		rReturnRecord = rCurQuery;
	}
	return rReturnRecord;
}

// Adjusts the fake latency ping
void CBacktrackNew::AdjustPing(INetChannel* netChannel)
{
	for (const auto& cSequence : dSequences)
	{
		if (I::GlobalVars->realtime - cSequence.CurTime >= GetLatency())
		{
			netChannel->m_nInReliableState = cSequence.InReliableState;
			netChannel->m_nInSequenceNr = cSequence.SequenceNr;
			break;
		}
	}
}