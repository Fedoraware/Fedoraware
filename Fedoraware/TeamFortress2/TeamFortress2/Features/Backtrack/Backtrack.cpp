#include "Backtrack.h"

bool CBacktrackNew::IsTracked(TickRecordNew Record){
	return I::GlobalVars->curtime - Record.flCreateTime < 1.f;
}

bool CBacktrackNew::WithinRewind(TickRecordNew Record){	//	check if we can go to this tick, ie, within 200ms of us
	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	INetChannel* iNetChan = I::EngineClient->GetNetChannelInfo();
	
	if (!pLocal || !iNetChan) { return false; }

	const float flDelay = std::clamp(GetLatency(), 0.f, 1.f);	//	TODO:: sv_maxunlag
	const float flDelta = flDelay - TICKS_TO_TIME(I::GlobalVars->tickcount - Record.iTickCount);

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
				if (vDelta.Length2DSqr() > 4096.f) { /*Utils::ConLog("LagCompensation", "Cleared borked records", {255, 0, 0, 255});*/ mRecords[pEntity].clear(); }
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

void CBacktrackNew::ReportShot(int iIndex){
	CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(iIndex);
	if (!pEntity) { return; }
	CBaseCombatWeapon* pWeapon = pEntity->GetActiveWeapon();
	if (!pWeapon) { return; }
	mDidShoot[pEntity->GetIndex()] = Utils::GetWeaponType(pWeapon) == EWeaponType::HITSCAN;
}

std::optional<TickRecordNew> CBacktrackNew::GetHitRecord(CUserCmd* pCmd, CBaseEntity* pEntity, const Vec3 vAngles, const Vec3 vPos){
	Vec3 vForward = {};

	std::optional<TickRecordNew> cReturnRecord{};
	float flLastAngle = 45.f;

	for (const auto& rCurQuery : mRecords[pEntity]){
		if (!WithinRewind(rCurQuery)) { continue; }
		for (int iCurHitbox = 0; iCurHitbox < 18; iCurHitbox++){
			//	it's possible to set entity positions and bones back to this record and then see what hitbox we will hit and rewind to that record, bt i dont wanna
			const Vec3 vHitboxPos = pEntity->GetHitboxPosMatrix(iCurHitbox, (matrix3x4*)(&rCurQuery.BoneMatrix.BoneMatrix));
			const Vec3 vAngleTo = Math::CalcAngle(vPos, vHitboxPos);
			const float flFOVTo = Math::CalcFov(vAngles, vAngleTo);
			if (flFOVTo < flLastAngle) { cReturnRecord = rCurQuery; flLastAngle = flFOVTo; }
		}
	}
	return cReturnRecord;
}

std::optional<TickRecordNew> CBacktrackNew::Run(CUserCmd* pCmd){
	if (G::IsAttacking && Vars::Backtrack::Enabled.Value){
		CBaseEntity* pLocal = g_EntityCache.GetLocal();
		if (!pLocal) { return std::nullopt; }
		
		Vec3 vPos = pLocal->GetShootPos();
		const Vec3 vAngles = pCmd->viewangles;

		std::optional<TickRecordNew> cReturnTick;
		for (const auto& pEnemy : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
		{
			if (!pEnemy || !pEnemy->IsAlive()) { continue; }	//	dont scan
			PlayerInfo_t pInfo{};	//	dont care about ignored players
			if (!I::EngineClient->GetPlayerInfo(pEnemy->GetIndex(), &pInfo)){
				if (G::IsIgnored(pInfo.friendsID)) { continue; }
			}
			if (std::optional<TickRecordNew> checkRec = GetHitRecord(pCmd, pEnemy, vAngles, vPos)){
				cReturnTick = checkRec;
				break;
			}
		}
		if (cReturnTick)
		{ pCmd->tick_count = TIME_TO_TICKS(cReturnTick->flSimTime + G::LerpTime); return std::nullopt; }
	}
	return std::nullopt;
}

std::optional<TickRecordNew> CBacktrackNew::Aimbot(CBaseEntity* pEntity, BacktrackMode iMode, int nHitbox){
	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	if (!pLocal) { return std::nullopt; }
	if (mRecords[pEntity].empty()) { return std::nullopt; }
	switch (iMode){
	case BacktrackMode::ALL: {
		for (const auto& rCurQuery : mRecords[pEntity]){ 
			if (!WithinRewind(rCurQuery) || !IsTracked(rCurQuery)) { continue; }
			const Vec3 vHitboxPos = pEntity->GetHitboxPosMatrix(nHitbox, (matrix3x4*)(&rCurQuery.BoneMatrix.BoneMatrix));
			if (Utils::VisPos(pLocal,pEntity, pLocal->GetShootPos(), vHitboxPos)) { return rCurQuery; }
		}
		return std::nullopt;
	}
	case BacktrackMode::FIRST: {
		if (std::optional<TickRecordNew> FirstRecord = GetFirstRecord(pEntity)){
			const Vec3 vHitboxPos = pEntity->GetHitboxPosMatrix(nHitbox, (matrix3x4*)(&FirstRecord->BoneMatrix.BoneMatrix));
			if (Utils::VisPos(pLocal,pEntity, pLocal->GetShootPos(), vHitboxPos)) { return FirstRecord; }
		}
		return std::nullopt;
	}
	case BacktrackMode::LAST: {
		if (std::optional<TickRecordNew> LastRecord = GetLastRecord(pEntity)){
			const Vec3 vHitboxPos = pEntity->GetHitboxPosMatrix(nHitbox, (matrix3x4*)(&LastRecord->BoneMatrix.BoneMatrix));
			if (Utils::VisPos(pLocal,pEntity, pLocal->GetShootPos(), vHitboxPos)) { return LastRecord; }
		}
		return std::nullopt;
	}
	case BacktrackMode::ADAPTIVE: {
		std::optional<TickRecordNew> ReturnTick{};
		for (const auto& rCurQuery : mRecords[pEntity]){ 
			if (!WithinRewind(rCurQuery) || !IsTracked(rCurQuery)) { continue; }
			const Vec3 vHitboxPos = pEntity->GetHitboxPosMatrix(nHitbox, (matrix3x4*)(&rCurQuery.BoneMatrix.BoneMatrix));
			if (Utils::VisPos(pLocal,pEntity, pLocal->GetShootPos(), vHitboxPos)) { ReturnTick = rCurQuery; }
			if (ReturnTick->bOnShot) { break; }
		}
		return ReturnTick;
	}
	case BacktrackMode::ONSHOT: {
		for (const auto& rCurQuery : mRecords[pEntity]){ 
			if (!WithinRewind(rCurQuery) || !IsTracked(rCurQuery)) { continue; }
			const Vec3 vHitboxPos = pEntity->GetHitboxPosMatrix(nHitbox, (matrix3x4*)(&rCurQuery.BoneMatrix.BoneMatrix));
			if (Utils::VisPos(pLocal,pEntity, pLocal->GetShootPos(), vHitboxPos)) { 
				if (rCurQuery.bOnShot) { return rCurQuery; }
			}
		}
		return std::nullopt;
	}
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

std::optional<TickRecordNew> CBacktrackNew::GetFirstRecord(CBaseEntity* pEntity){
	if (mRecords[pEntity].empty()) { return std::nullopt; }
	std::optional<TickRecordNew> rReturnRecord = std::nullopt;
	for (const auto& rCurQuery : mRecords[pEntity]){
		if (!IsTracked(rCurQuery) || !WithinRewind(rCurQuery)) { continue; }
		return rCurQuery;
	}
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