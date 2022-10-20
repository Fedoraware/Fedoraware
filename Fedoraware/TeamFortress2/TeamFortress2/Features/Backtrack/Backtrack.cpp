#include "Backtrack.h"

bool CBacktrackNew::IsTracked(const TickRecord& record)
{
	return I::GlobalVars->curtime - record.flCreateTime < 1.f;
}

//	should return true if the current position on the client has a lag comp record created for it by the server (SHOULD)
//	if the player has updated more than once, only the first update will have a backtrack record (i think)
//	dont use this yet
bool CBacktrackNew::IsSimulationReliable(CBaseEntity* pEntity)
{
	const float flSimTimeDelta = pEntity->GetSimulationTime() - pEntity->GetOldSimulationTime();
	const int iTicksSimTimeDelta = flSimTimeDelta / I::GlobalVars->interval_per_tick;
	return iTicksSimTimeDelta == 1;
}

bool CBacktrackNew::WithinRewind(const TickRecord& record)
{
	//	check if we can go to this tick, ie, within 200ms of us
	const auto pLocal = g_EntityCache.GetLocal();
	const auto iNetChan = I::EngineClient->GetNetChannelInfo();

	if (!pLocal || !iNetChan) { return false; }

	const float flCorrect = std::clamp(iNetChan->GetLatency(FLOW_INCOMING) + GetLatency() + G::LerpTime, 0.f, g_ConVars.sv_maxunlag->GetFloat());
	const float flDelta = fabsf(flCorrect - (I::GlobalVars->curtime - record.flSimTime));

	return flDelta < .2f; //	in short, check if the record is +- 200ms from us
}

void CBacktrackNew::CleanRecords()
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
		if (mRecords[pEntity].size() > 67) { mRecords[pEntity].pop_back(); }
	}
}

void CBacktrackNew::MakeRecords()
{
	const float flCurTime = I::GlobalVars->curtime;
	const int iTickcount = I::GlobalVars->tickcount;
	if (iLastCreationTick == iTickcount) { return; }
	iLastCreationTick = iTickcount;

	for (int n = 1; n < I::ClientEntityList->GetHighestEntityIndex(); n++)
	{
		CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(n);
		if (!pEntity) { continue; }
		if (!pEntity->IsPlayer()) { return; }
		const float flSimTime = pEntity->GetSimulationTime(), flOldSimTime = pEntity->GetOldSimulationTime();

		//Utils::ConLog("LagCompensation", tfm::format("SimTime = %.1f\nOldSimTime = %.1f", flSimTime, flOldSimTime).c_str(), {255, 0, 0, 255});

		if (flSimTime != flOldSimTime)
		{
			//	create record on simulated players
			//Utils::ConLog("LagCompensation", "Setting Up Bones", {255, 0, 0, 255});
			matrix3x4 bones[128];
			if (!pEntity->SetupBones(bones, 128, BONE_USED_BY_ANYTHING, flSimTime)) { continue; }
			//Utils::ConLog("LagCompensation", "Creating Record", {255, 0, 0, 255});
			const Vec3 vOrigin = pEntity->GetAbsOrigin();
			if (!mRecords[pEntity].empty())
			{
				// as long as we have 1 record we can check for lagcomp breaking here
				const Vec3 vPrevOrigin = mRecords[pEntity].front().vOrigin;
				const Vec3 vDelta = vOrigin - vPrevOrigin;
				if (vDelta.Length2DSqr() > 4096.f)
				{
					/*Utils::ConLog("LagCompensation", "Cleared borked records", {255, 0, 0, 255});*/
					mRecords[pEntity].clear();
				}
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
		if (mRecords[pEntity].size() > 67)
		{
			/*Utils::ConLog("LagCompensation", "Manually removed tick record", {255, 0, 0, 255});*/
			mRecords[pEntity].pop_back();
		} //	schizoid check
	}
}

// Store the last 2048 sequences
void CBacktrackNew::UpdateDatagram()
{
	if (const auto iNetChan = I::EngineClient->GetNetChannelInfo())
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
float CBacktrackNew::GetLatency()
{
	if (const auto iNetChan = I::EngineClient->GetNetChannelInfo())
	{
		const float flRealLatency = iNetChan->GetLatency(FLOW_OUTGOING);
		const float flFakeLatency = flLatencyRampup * std::clamp(static_cast<float>(Vars::Backtrack::Latency.Value), 0.f, 800.f) / 1000.f;
		const float flAdjustedLatency = std::clamp((flRealLatency + (bFakeLatency ? flFakeLatency : 0.f)), 0.f, 1.f);
		return flAdjustedLatency;
	}

	return 0.f; //	we failed to get net channel and therefor have no latency.
}

void CBacktrackNew::PlayerHurt(CGameEvent* pEvent)
{
	//const int iIndex = I::EngineClient->GetPlayerForUserID(pEvent->GetInt("userid"));
	//if (CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(iIndex)){
	//	mRecords[pEntity].clear();	//	bone cache has gone to poop for this entity, they must be cleansed in holy fire :smiling_imp:
	//}
}

void CBacktrackNew::Restart()
{
	mRecords.clear();
	dSequences.clear();
	flLatencyRampup = 0.f;
	iLastInSequence = 0;
}

void CBacktrackNew::FrameStageNotify()
{
	const auto pLocal = g_EntityCache.GetLocal();
	const auto iNetChan = I::EngineClient->GetNetChannelInfo();
	if (!pLocal || !iNetChan) { return Restart(); }

	flLatencyRampup = std::min(1.f, flLatencyRampup += I::GlobalVars->interval_per_tick);
	MakeRecords();
	CleanRecords();
}

void CBacktrackNew::ReportShot(int iIndex)
{
	CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(iIndex);
	if (!pEntity) { return; }
	mDidShoot[pEntity->GetIndex()] = true;
}

std::optional<TickRecord> CBacktrackNew::GetHitRecord(CUserCmd* pCmd, CBaseEntity* pEntity, const Vec3 vAngles, const Vec3 vPos)
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

std::optional<TickRecord> CBacktrackNew::Run(CUserCmd* pCmd)
{
	if (!Vars::Backtrack::Enabled.Value) { return std::nullopt; }
	UpdateDatagram();
	if (G::IsAttacking)
	{
		CBaseEntity* pLocal = g_EntityCache.GetLocal();
		if (!pLocal) { return std::nullopt; }

		const Vec3 vShootPos = pLocal->GetShootPos();
		const Vec3 vAngles = pCmd->viewangles;

		std::optional<TickRecord> cReturnTick;
		for (const auto& pEnemy : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
		{
			if (!pEnemy || !pEnemy->IsAlive()) { continue; } //	dont scan

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

std::optional<TickRecord> CBacktrackNew::Aimbot(CBaseEntity* pEntity, BacktrackMode iMode, int nHitbox)
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
				if (!WithinRewind(rCurQuery) || !IsTracked(rCurQuery)) { continue; }
				const Vec3 vHitboxPos = pEntity->GetHitboxPosMatrix(nHitbox, (matrix3x4*)(&rCurQuery.BoneMatrix.BoneMatrix));
				if (Utils::VisPos(pLocal, pEntity, pLocal->GetShootPos(), vHitboxPos)) { return rCurQuery; }
			}
			return std::nullopt;
		}

	case BacktrackMode::FIRST:
		{
			if (std::optional<TickRecord> FirstRecord = GetFirstRecord(pEntity))
			{
				const Vec3 vHitboxPos = pEntity->GetHitboxPosMatrix(nHitbox, (matrix3x4*)(&FirstRecord->BoneMatrix.BoneMatrix));
				if (Utils::VisPos(pLocal, pEntity, pLocal->GetShootPos(), vHitboxPos)) { return FirstRecord; }
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

	case BacktrackMode::ADAPTIVE:
		{
			std::optional<TickRecord> ReturnTick{};
			for (const auto& rCurQuery : mRecords[pEntity])
			{
				if (!WithinRewind(rCurQuery) || !IsTracked(rCurQuery)) { continue; }
				const Vec3 vHitboxPos = pEntity->GetHitboxPosMatrix(nHitbox, (matrix3x4*)(&rCurQuery.BoneMatrix.BoneMatrix));
				if (Utils::VisPos(pLocal, pEntity, pLocal->GetShootPos(), vHitboxPos)) { ReturnTick = rCurQuery; }
				if (ReturnTick->bOnShot) { break; }
			}
			return ReturnTick;
		}

	case BacktrackMode::ONSHOT:
		{
			for (const auto& rCurQuery : mRecords[pEntity])
			{
				if (!WithinRewind(rCurQuery) || !IsTracked(rCurQuery)) { continue; }
				const Vec3 vHitboxPos = pEntity->GetHitboxPosMatrix(nHitbox, (matrix3x4*)(&rCurQuery.BoneMatrix.BoneMatrix));
				if (Utils::VisPos(pLocal, pEntity, pLocal->GetShootPos(), vHitboxPos))
				{
					if (rCurQuery.bOnShot) { return rCurQuery; }
				}
			}
			return std::nullopt;
		}
	}
	return std::nullopt;
}

std::deque<TickRecord>* CBacktrackNew::GetRecords(CBaseEntity* pEntity)
{
	if (mRecords[pEntity].empty())
	{
		return nullptr;
	}

	return &mRecords[pEntity];
}

std::optional<TickRecord> CBacktrackNew::GetLastRecord(CBaseEntity* pEntity)
{
	if (mRecords[pEntity].empty()) { return std::nullopt; }
	std::optional<TickRecord> rReturnRecord = std::nullopt;
	for (const auto& rCurQuery : mRecords[pEntity])
	{
		if (!IsTracked(rCurQuery) || !WithinRewind(rCurQuery)) { continue; }
		rReturnRecord = rCurQuery;
	}
	return rReturnRecord;
}

std::optional<TickRecord> CBacktrackNew::GetFirstRecord(CBaseEntity* pEntity)
{
	if (mRecords[pEntity].empty()) { return std::nullopt; }
	std::optional<TickRecord> rReturnRecord = std::nullopt;
	for (const auto& rCurQuery : mRecords[pEntity])
	{
		if (!IsTracked(rCurQuery) || !WithinRewind(rCurQuery)) { continue; }
		return rCurQuery;
	}
	return std::nullopt;
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
