#include "Backtrack.h"

#include "../Aimbot/AimbotGlobal/AimbotGlobal.h"
#include "../Aimbot/MovementSimulation/MovementSimulation.h"

#define ROUND_TO_TICKS(t) (TICKS_TO_TIME(TIME_TO_TICKS(t)))

inline bool CBacktrack::IsTracked(const TickRecord& record, const float flDelay)
{
	return record.flSimTime - flDelay >= I::GlobalVars->curtime - 1.f;
}

inline bool CBacktrack::IsEarly(CBaseEntity* pEntity) {
	return !Vars::Backtrack::AllowForward.Value && mRecords[pEntity].front().flSimTime > pEntity->GetSimulationTime();
}

//	should return true if the current position on the client has a lag comp record created for it by the server (SHOULD)
//	if the player has updated more than once, only the first update will have a backtrack record (i think)
//	dont use this yet
bool CBacktrack::IsSimulationReliable(CBaseEntity* pEntity)
{
	const float flSimTimeDelta = pEntity->GetSimulationTime() - pEntity->GetOldSimulationTime();
	return flSimTimeDelta > 0 && flSimTimeDelta < I::GlobalVars->interval_per_tick * 2;	//	there is some innacuracy here, as long as it's less than 2 ticks it should be fine.
}

bool CBacktrack::WithinRewindEx(const TickRecord& record, const float flCompTime) {
	const auto iNetChan = I::EngineClient->GetNetChannelInfo();
	if (!iNetChan) { return false; }

	const float flTargetTime = record.flSimTime + G::LerpTime;
	const float flCorrect = ROUND_TO_TICKS(std::clamp(G::LerpTime + ROUND_TO_TICKS(iNetChan->GetLatency(FLOW_INCOMING) + iNetChan->GetLatency(FLOW_OUTGOING)), 0.f, g_ConVars.sv_maxunlag->GetFloat()));
	const float flDelta = fabsf(flCorrect - (flCompTime - flTargetTime));

	return flDelta < .200f - TICKS_TO_TIME(2);
}

//	check if we can go to this tick, ie, within 200ms of us
bool CBacktrack::WithinRewind(const TickRecord& record, const float flDelay)
{
	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	if (!pLocal) { return false; }
	return WithinRewindEx(record, pLocal->GetSimulationTime() + flDelay) && IsTracked(record, flDelay);
}

//	hypothetically if their simtime is within 200ms of us we can hit their original, but idc
bool CBacktrack::CanHitOriginal(CBaseEntity* pEntity){
	if (!bFakeLatency) { return true; }
	if (GetLatency() > .200f) { return false; }

	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	const auto iNetChan = I::EngineClient->GetNetChannelInfo();
	if (!iNetChan || !pLocal) { return false; }

	const float flTargetTime = pEntity->GetSimulationTime() + G::LerpTime;
	const float flCorrect = ROUND_TO_TICKS(std::clamp(G::LerpTime + ROUND_TO_TICKS(iNetChan->GetLatency(FLOW_INCOMING) + iNetChan->GetLatency(FLOW_OUTGOING)), 0.f, g_ConVars.sv_maxunlag->GetFloat()));
	const float flDelta = fabsf(flCorrect - (pLocal->GetSimulationTime() - flTargetTime));

	return flDelta < .200f - TICKS_TO_TIME(2);
}

void CBacktrack::CleanRecords()
{
	for (int n = 1; n < I::ClientEntityList->GetHighestEntityIndex(); n++)
	{
		CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(n);
		if (!pEntity) { continue; }

		if (pEntity->GetDormant() || !pEntity->IsAlive() || !pEntity->IsPlayer())
		{
			mRecords[pEntity].clear();
			continue;
		}

		if (mRecords[pEntity].empty()) { continue; }
		if (!IsTracked(mRecords[pEntity].back())) { mRecords[pEntity].pop_back(); }
		if (mRecords[pEntity].size() > iTickCount) { mRecords[pEntity].pop_back(); }
	}
}

void CBacktrack::MakeRecords()
{
	if (!iTickCount) {
		iTickCount = TIME_TO_TICKS(1);
	}

	const float flCurTime = I::GlobalVars->curtime;
	const int iTickcount = I::GlobalVars->tickcount;
	if (iLastCreationTick == iTickcount) { return; }
	iLastCreationTick = iTickcount;

	for (int n = 1; n < I::ClientEntityList->GetHighestEntityIndex(); n++)
	{
		CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(n);
		if (!pEntity) { continue; }
		if (pEntity == g_EntityCache.GetLocal()) { continue; }
		if (!pEntity->IsPlayer()) { return; }
		const float flSimTime = pEntity->GetSimulationTime(), flOldSimTime = pEntity->GetOldSimulationTime();
		const float flDelta = flSimTime - flOldSimTime;

		if (flDelta < 0) { continue; }	//	i think fucking not
		
		const Vec3 vOrigin = pEntity->m_vecOrigin();
		if (!mRecords[pEntity].empty())
		{
			// as long as we have 1 record we can check for lagcomp breaking here
			const Vec3 vPrevOrigin = mRecords[pEntity].front().vOrigin;
			const Vec3 vDelta = vOrigin - vPrevOrigin;
			if (vDelta.Length2DSqr() > 4096.f)
			{
				mRecords[pEntity].clear();
			}
		}

		//PlayerInfo_t pInfo{};
		//if (I::EngineClient->GetPlayerInfo(n, &pInfo) && flDelta != I::GlobalVars->interval_per_tick) {
		//	Utils::ConLog("Backtrack", std::format("{} Expected {:.3f} but got {:.3f}", pInfo.name, I::GlobalVars->interval_per_tick * 1000, flDelta * 1000).c_str(), { 0, 222, 255, 255 }, Vars::Debug::Logging.Value);
		//}

		if (IsSimulationReliable(pEntity))
		{
			//	create record on simulated players
			matrix3x4 bones[128];
			if (!pEntity->SetupBones(bones, 128, BONE_USED_BY_ANYTHING, flSimTime)) { continue; }

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
		else if (Vars::Backtrack::UnchokePrediction.Value && flSimTime == flOldSimTime) {	//	user is choking, predict location of next record.
			const Vec3 vOriginalPos = pEntity->GetAbsOrigin();
			const Vec3 vOriginalEyeAngles = pEntity->GetEyeAngles();
			const float flNextSimTime = flSimTime + I::GlobalVars->interval_per_tick;
			const float flDeltaRecorded = flNextSimTime - mRecords[pEntity].empty() ? flSimTime : mRecords[pEntity].front().flSimTime;
			if (flDeltaRecorded < I::GlobalVars->interval_per_tick) { continue; }	//	we already predicted a tick for this player. if we havent then their simtime is being reported wrong (should be caught prior)
			//if (pEntity->GetVelocity().Length2D() > 4096.f) { continue; }	//	this will only happen on people that are stuck or it will be caught elsewhere, dont use
			if (F::MoveSim.Initialize(pEntity))
			{
				CMoveData moveData = {};
				Vec3 absOrigin = {};
				F::MoveSim.RunTick(moveData, absOrigin);
				
				//	we've predicted their next record, and we can probably predict their next lag record but if they are fakelagging it's pointless n shieet
				//	i was going to check if this lag comp would be valid here but it seems almost pointless now, dont do it.
				//	might need to do this for setupbones
				pEntity->SetAbsOrigin(moveData.m_vecAbsOrigin);
				pEntity->SetEyeAngles(moveData.m_vecAbsViewAngles);

				matrix3x4 bones[128];
				if (!pEntity->SetupBones(bones, 128, BONE_USED_BY_ANYTHING, flNextSimTime)) { //	if we fail bones we fail lyfe
					pEntity->SetAbsOrigin(vOriginalPos);
					pEntity->SetEyeAngles(vOriginalEyeAngles);
					continue; 
				}

				//create lag record
				mRecords[pEntity].push_front({
				flNextSimTime,
				flCurTime + I::GlobalVars->interval_per_tick,
				iTickcount + 1,
				false,
				*reinterpret_cast<BoneMatrixes*>(&bones),
				absOrigin,
				moveData.m_vecAbsViewAngles,
					});

				//restore
				pEntity->SetAbsOrigin(vOriginalPos);
				pEntity->SetEyeAngles(vOriginalEyeAngles);

				F::MoveSim.Restore();
			}
		}
		//cleanup
		mDidShoot[pEntity->GetIndex()] = false;
		if (mRecords[pEntity].size() > iTickCount)
		{
			/*Utils::ConLog("LagCompensation", "Manually removed tick record", {255, 0, 0, 255});*/
			mRecords[pEntity].pop_back();
		} //	schizoid check
	}
}

// Store the last 2048 sequences
void CBacktrack::UpdateDatagram()
{
	if (INetChannel* iNetChan = I::EngineClient->GetNetChannelInfo())
	{
		if (iNetChan->m_nInSequenceNr > iLastInSequence)
		{
			iLastInSequence = iNetChan->m_nInSequenceNr;
			dSequences.push_front(CIncomingSequence(iNetChan->m_nInReliableState, iNetChan->m_nInSequenceNr, I::GlobalVars->realtime));
		}

		if (dSequences.size() > 2048)
		{
			dSequences.pop_back();
		}
	}
}

// Returns the current (custom) backtrack latency
float CBacktrack::GetLatency()
{
	return bFakeLatency ? flLatencyRampup * std::clamp(static_cast<float>(Vars::Backtrack::Latency.Value), 0.f, 800.f) / 1000.f : 0.f;
}

void CBacktrack::PlayerHurt(CGameEvent* pEvent)
{
	//const int iIndex = I::EngineClient->GetPlayerForUserID(pEvent->GetInt("userid"));
	//if (CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(iIndex)){
	//	mRecords[pEntity].clear();	//	bone cache has gone to poop for this entity, they must be cleansed in holy fire :smiling_imp:
	//}
}

void CBacktrack::ResolverUpdate(CBaseEntity* pEntity) {
	mRecords[pEntity].clear();	//	TODO: eventually remake records and rotate them or smthn idk, maybe just rotate them
}

void CBacktrack::Restart()
{
	mRecords.clear();
	dSequences.clear();
	flLatencyRampup = 0.f;
	iLastInSequence = 0;
	iTickCount = TIME_TO_TICKS(g_ConVars.sv_maxunlag->GetFloat());
}

void CBacktrack::FrameStageNotify()
{
	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	INetChannel* iNetChan = I::EngineClient->GetNetChannelInfo();
	if (!pLocal || !iNetChan) { return Restart(); }

	flLatencyRampup = std::min(1.f, flLatencyRampup += I::GlobalVars->interval_per_tick);
	MakeRecords();
	CleanRecords();
}

void CBacktrack::ReportShot(int iIndex)
{
	CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(iIndex);
	if (!pEntity) { return; }
	mDidShoot[pEntity->GetIndex()] = true;
}

std::optional<TickRecord> CBacktrack::GetHitRecord(CUserCmd* pCmd, CBaseEntity* pEntity, const Vec3 vAngles, const Vec3 vPos)
{
	std::optional<TickRecord> cReturnRecord{};
	float flLastAngle = 45.f;

	for (const auto& rCurQuery : mRecords[pEntity])
	{
		if (!WithinRewind(rCurQuery)) { continue; }
		for (int iCurHitbox = 0; iCurHitbox < 18; iCurHitbox++)
		{
			//	it's possible to set entity positions and bones back to this record and then see what hitbox we will hit and rewind to that record, bt i dont wanna
			const Vec3 vHitboxPos = pEntity->GetHitboxPosMatrix(iCurHitbox, (matrix3x4*)(&rCurQuery.BoneMatrix.BoneMatrix));
			const Vec3 vAngleTo = Math::CalcAngle(vPos, vHitboxPos);
			const float flFOVTo = Math::CalcFov(vAngles, vAngleTo);
			if (flFOVTo < flLastAngle)
			{
				cReturnRecord = rCurQuery;
				flLastAngle = flFOVTo;
			}
		}
	}
	return cReturnRecord;
}

std::optional<TickRecord> CBacktrack::Run(CUserCmd* pCmd)
{
	if (!Vars::Backtrack::Enabled.Value) { return std::nullopt; }
	UpdateDatagram();
	if (F::AimbotGlobal.IsAttacking())
	{
		CBaseEntity* pLocal = g_EntityCache.GetLocal();
		if (!pLocal) { return std::nullopt; }

		const Vec3 vShootPos = pLocal->GetShootPos();
		const Vec3 vAngles = pCmd->viewangles;

		std::optional<TickRecord> cReturnTick;
		for (const auto& pEnemy : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
		{
			if (!pEnemy || !pEnemy->IsAlive() || pEnemy->IsAGhost()) { continue; } //	dont scan

			PlayerInfo_t pInfo{}; //	dont care about ignored players
			if (!I::EngineClient->GetPlayerInfo(pEnemy->GetIndex(), &pInfo))
			{
				if (G::IsIgnored(pInfo.friendsID)) { continue; }
			}

			if (const std::optional<TickRecord> checkRec = GetHitRecord(pCmd, pEnemy, vAngles, vShootPos))
			{
				cReturnTick = checkRec;
				break;
			}
		}
		if (cReturnTick)
		{
			pCmd->tick_count = TIME_TO_TICKS(cReturnTick->flSimTime + G::LerpTime);
			return std::nullopt;
		}
	}
	return std::nullopt;
}

std::optional<TickRecord> CBacktrack::Aimbot(CBaseEntity* pEntity, BacktrackMode iMode, int nHitbox)
{
	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	if (!pLocal) { return std::nullopt; }
	if (mRecords[pEntity].empty()) { return std::nullopt; }
	switch (iMode)
	{
		case BacktrackMode::ALL:
		{
			for (const auto& rCurQuery : mRecords[pEntity])
			{
				if (!WithinRewind(rCurQuery) || IsEarly(pEntity)) { continue; }
				const Vec3 vHitboxPos = pEntity->GetHitboxPosMatrix(nHitbox, (matrix3x4*)(&rCurQuery.BoneMatrix.BoneMatrix));
				if (Utils::VisPos(pLocal, pEntity, pLocal->GetShootPos(), vHitboxPos)) { return rCurQuery; }
			}
			return std::nullopt;
		}

		case BacktrackMode::LAST:
		{
			if (std::optional<TickRecord> LastRecord = GetLastRecord(pEntity))
			{
				const Vec3 vHitboxPos = pEntity->GetHitboxPosMatrix(nHitbox, (matrix3x4*)(&LastRecord->BoneMatrix.BoneMatrix));
				if (Utils::VisPos(pLocal, pEntity, pLocal->GetShootPos(), vHitboxPos)) { return LastRecord; }
			}
			return std::nullopt;
		}

		case BacktrackMode::PREFERONSHOT:
		{
			std::optional<TickRecord> ReturnTick{};
			for (const auto& rCurQuery : mRecords[pEntity])
			{
				if (!WithinRewind(rCurQuery) || IsEarly(pEntity)) { continue; }
				const Vec3 vHitboxPos = pEntity->GetHitboxPosMatrix(nHitbox, (matrix3x4*)(&rCurQuery.BoneMatrix.BoneMatrix));
				if (Utils::VisPos(pLocal, pEntity, pLocal->GetShootPos(), vHitboxPos)) { ReturnTick = rCurQuery; }
				if (ReturnTick.has_value())
				{
					if (ReturnTick->bOnShot) { break; }
				}
			}
			return ReturnTick;
		}
	}
	return std::nullopt;
}

std::deque<TickRecord>* CBacktrack::GetRecords(CBaseEntity* pEntity)
{
	if (mRecords[pEntity].empty())
	{
		return nullptr;
	}

	return &mRecords[pEntity];
}

std::optional<TickRecord> CBacktrack::GetLastRecord(CBaseEntity* pEntity)
{
	if (mRecords[pEntity].empty()) { return std::nullopt; }
	std::optional<TickRecord> rReturnRecord = std::nullopt;
	for (const auto& rCurQuery : mRecords[pEntity])
	{
		if (!WithinRewind(rCurQuery) || IsEarly(pEntity)) { continue; }
		rReturnRecord = rCurQuery;
	}
	return rReturnRecord;
}

std::optional<TickRecord> CBacktrack::GetFirstRecord(CBaseEntity* pEntity)
{
	//	UNUSED AND TRASH! UPDATE B4 USE
	//if (mRecords[pEntity].empty()) { return std::nullopt; }
	//std::optional<TickRecord> rReturnRecord = std::nullopt;
	//for (int nIndex = 2; nIndex < mRecords[pEntity].size(); nIndex++)
	//{
	//	if (!IsTracked(mRecords[pEntity][nIndex]) || !WithinRewind(mRecords[pEntity][nIndex])) { continue; }
	//	return mRecords[pEntity][nIndex];
	//}
	return std::nullopt;
}

// Adjusts the fake latency ping
void CBacktrack::AdjustPing(INetChannel* netChannel)
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
