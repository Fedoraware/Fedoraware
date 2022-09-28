#include "Backtrack.h"

/*bool CBacktrack::IsGoodTick(const float simTime)
{
	const float latency = std::clamp(GetLatency(), 0.f, 0.9f);
	const float deltaTime = latency - (I::GlobalVars->curtime - simTime);

	return std::abs(deltaTime) <= 0.2f - TICKS_TO_TIME(2);
}*/

bool CBacktrackNew::IsTracked(TickRecordNew Record){
	return I::GlobalVars->curtime - Record.flCreateTime < 1.f;
}

bool CBacktrackNew::WithinRewind(TickRecordNew Record){	//	check if we can go to this tick, ie, within 200ms of us
	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	INetChannel* iNetChan = I::EngineClient->GetNetChannelInfo();
	
	if (!pLocal || !iNetChan) { return false; }

	const float flDelay = std::clamp(iNetChan->GetLatency(MAX_FLOWS) + G::LerpTime, 0.f, 1.f);	//	TODO:: sv_maxunlag
	const float flDelta = flDelay - TICKS_TO_TIME(pLocal->m_nTickBase() - Record.flSimTime);

	return fabsf(flDelta) < .2f;	//	in short, check if the record is +- 200ms from us
}

void CBacktrackNew::CleanRecords(){
	for (int n = 1; n < I::ClientEntityList->GetHighestEntityIndex(); n++)
	{
			CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(n);
			if (!pEntity) { continue; }
			std::deque<TickRecordNew> vRecords = mRecords[pEntity];

			if (pEntity->GetDormant() || !pEntity->IsAlive() || !pEntity->IsPlayer()) { vRecords.clear(); continue; }
			else if (vRecords.size() < 1) { continue; }
			if (!IsTracked(vRecords.front())){ vRecords.pop_front(); }
	}
}

void CBacktrackNew::MakeRecords(){
	for (int n = 1; n < I::ClientEntityList->GetHighestEntityIndex(); n++)
	{
		CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(n);
		if (!pEntity) { continue; }
		if (!pEntity->IsPlayer()) { return; }
		const float flSimTime = pEntity->GetSimulationTime(), flOldSimTime = pEntity->GetOldSimulationTime();
		
		Utils::ConLog("LagCompensation", tfm::format("SimTime = %.1f\nOldSimTime = %.1f", flSimTime, flOldSimTime).c_str(), {255, 0, 0, 255});

		if (flSimTime != flOldSimTime){	//	create record on simulated players
			Utils::ConLog("LagCompensation", "Setting Up Bones", {255, 0, 0, 255});
			matrix3x4 bones[128];
			if (!pEntity->SetupBones(bones, 128, BONE_USED_BY_ANYTHING, flSimTime)) { continue; }
			Utils::ConLog("LagCompensation", "Creating Record", {255, 0, 0, 255});
			mRecords[pEntity].push_back({
				flSimTime,
				I::GlobalVars->curtime,
				I::GlobalVars->tickcount,
				mDidShoot[pEntity->GetIndex()],
				*reinterpret_cast<BoneMatrixes*>(&bones),
			});
		}

		//cleanup
		mDidShoot[pEntity->GetIndex()] = false;
		//if (mRecords[pEntity].size() > 66){ Utils::ConLog("LagCompensation", "Manually removed tick record", {255, 0, 0, 255}); mRecords[pEntity].pop_back(); }	//	schizoid check
	}
}

void CBacktrackNew::Restart(){
	mRecords.clear();
}

void CBacktrackNew::FrameStageNotify(){
	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	INetChannel* iNetChan = I::EngineClient->GetNetChannelInfo();
	if (!pLocal || !iNetChan) { return Restart(); }

	MakeRecords();
	CleanRecords();
}

void CBacktrackNew::ReportShot(int iIndex, void* pWpn){
	if (CBaseCombatWeapon* pWeapon = reinterpret_cast<CBaseCombatWeapon*>(pWpn)){
		mDidShoot[iIndex] = Utils::GetWeaponType(pWeapon) == EWeaponType::HITSCAN;
	}
}

TickRecordNew CBacktrackNew::Run(CUserCmd* pCmd, bool bAimbot = false, CBaseEntity* pEntity = nullptr){
	static int iOldTickCount = I::GlobalVars->tickcount;
	const int iCurTickCount = I::GlobalVars->tickcount;
	static int iLastIndex = 0;
	static CBaseEntity* oldEnt = pEntity; 

	if (pEntity){ if (pEntity != oldEnt) { iLastIndex = 0; oldEnt = pEntity; } }
	if (iOldTickCount != iCurTickCount) { iLastIndex = 0; iOldTickCount = iCurTickCount; }

	TickRecordNew returnTick{};

	if (bAimbot && pEntity){	//	if we're aimbotting we don't care what record we get
		if (mRecords[pEntity].size()){
			return mRecords[pEntity].front();
		}
		//while (true){
		//	const int curIndex = mRecords[pEntity].size() - iLastIndex;
		//	if (curIndex >= mRecords[pEntity].size() - 1 || mRecords[pEntity].empty()) { return returnTick; }	//	return blank data if we failed to find anything
		//	if (IsTracked(mRecords[pEntity].at(curIndex)) && WithinRewind(mRecords[pEntity].at(curIndex))) { returnTick = mRecords[pEntity].at(iLastIndex); return returnTick; } 
		//	else { iLastIndex++; }
		//}
	}
	else{
		const Vec3 vAngles = pCmd->viewangles;
	}
	return returnTick;
}




//void CBacktrack::Run()
//{
//	if (!Vars::Backtrack::Enabled.Value)
//	{
//		LatencyRampup = 0.f;
//		if (Vars::Aimbot::Projectile::StrafePredictionAir.Value || Vars::Aimbot::Projectile::StrafePredictionGround.Value)
//		{
//			if (g_EntityCache.GetLocal())
//			{
//				UpdateRecords();
//			}
//		}
//		return;
//	}
//
//	LatencyRampup = std::min(1.f, LatencyRampup += I::GlobalVars->interval_per_tick);
//
//	if (g_EntityCache.GetLocal())
//	{
//		UpdateDatagram();
//		UpdateRecords();
//		//else
//		//{
//		//	for (auto& record : Records)
//		//	{
//		//		record.clear();
//		//	}
//		//}
//	}
//	else
//	{
//		Sequences.clear();
//	}
//}

// Store the last 2048 sequences
void CBacktrack::UpdateDatagram()
{
	const INetChannel* netChannel = I::EngineClient->GetNetChannelInfo();
	if (netChannel)
	{
		if (netChannel->m_nInSequenceNr > LastInSequence)
		{
			LastInSequence = netChannel->m_nInSequenceNr;
			Sequences.push_front(CIncomingSequence(netChannel->m_nInReliableState, netChannel->m_nInSequenceNr, I::GlobalVars->realtime));
		}

		if (Sequences.size() > 2048)
		{
			Sequences.pop_back();
		}
	}
}

// Returns the current (custom) backtrack latency
float CBacktrack::GetLatency()
{
	float realLatency = 0.f;

	if (const auto netChannel = I::EngineClient->GetNetChannelInfo())
	{
		realLatency = std::clamp(netChannel->GetLatency(FLOW_OUTGOING), 0.f, 0.9f);
	}

	float flLatency = Vars::Backtrack::Latency.Value;

	if (flLatency <= 200 || !AllowLatency)
	{
		flLatency = 200;
	}

	return LatencyRampup * std::clamp(flLatency * 0.001f, 0.f, 0.9f - realLatency);
}

// Adjusts the fake latency ping
void CBacktrack::AdjustPing(INetChannel* netChannel)
{
	for (const auto& sequence : Sequences)
	{
		if (I::GlobalVars->realtime - sequence.CurTime >= GetLatency())
		{
			netChannel->m_nInReliableState = sequence.InReliableState;
			netChannel->m_nInSequenceNr = sequence.SequenceNr;
			break;
		}
	}
}

void CBacktrack::ResetLatency()
{
	LastInSequence = 0;
	LatencyRampup = 0.f;
}

/* Returns all records of a given player */
std::deque<TickRecord>* CBacktrack::GetPlayerRecords(int entIdx)
{
	if (Records[entIdx].empty())
	{
		return nullptr;
	}

	return &Records[entIdx];
}

//// Returns the last valid backtrack tick (further away from the player)
//std::optional<TickRecord> CBacktrack::GetLastRecord(int entIdx)
//{
//	const auto& entRecord = Records[entIdx];
//	if (!entRecord.empty()) {
//		return { entRecord.back() };
//	}
//
//	return std::nullopt;
//}

///* Returns the first valid backtrack tick(close to the player) */
//std::optional<TickRecord> CBacktrack::GetFirstRecord(int entIdx)
//{
//	const auto& entRecord = Records[entIdx];
//	for (const auto& record : entRecord)
//	{
//		if (IsTickInRange(record.TickCount))
//		{
//			return record;
//		}
//	}
//
//	return std::nullopt;
//}

///* Returns the best tick for the chosen mode */
//std::optional<TickRecord> CBacktrack::GetRecord(int entIdx, BacktrackMode mode)
//{
//	switch (mode)
//	{
//	case BacktrackMode::First: { return GetFirstRecord(entIdx); }
//	default: { return GetLastRecord(entIdx); }
//	}
//}
