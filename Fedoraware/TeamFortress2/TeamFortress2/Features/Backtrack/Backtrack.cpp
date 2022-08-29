#include "Backtrack.h"

/*bool CBacktrack::IsGoodTick(const float simTime)
{
	const float latency = std::clamp(GetLatency(), 0.f, 0.9f);
	const float deltaTime = latency - (I::GlobalVars->curtime - simTime);

	return std::abs(deltaTime) <= 0.2f - TICKS_TO_TIME(2);
}*/

bool CBacktrack::IsTickInRange(int tickCount)
{
	if (!G::CurrentUserCmd) { return false; }

	const int deltaTicks = std::abs(tickCount - G::CurrentUserCmd->tick_count + TIME_TO_TICKS(GetLatency()));
	return TICKS_TO_TIME(deltaTicks) <= 0.2f - TICKS_TO_TIME(2);
}

void CBacktrack::UpdateRecords()
{
	const auto& pLocal = g_EntityCache.GetLocal();
	if (!pLocal || !pLocal->IsAlive())
	{
		for (auto& record : Records)
		{
			record.clear();
		}
		return;
	}

	for (int i = 0; i < I::EngineClient->GetMaxClients(); i++)
	{
		// Check if the entity is valid
		const auto& pEntity = I::ClientEntityList->GetClientEntity(i);
		if (!pEntity || pEntity->GetDormant() || !pEntity->IsAlive()) { Records[i].clear(); continue; }

		// Get bone matrix
		matrix3x4 bones[128];
		pEntity->SetupBones(bones, 128, BONE_USED_BY_ANYTHING, 0.0f);

		// Get model info
		model_t* model = pEntity->GetModel();
		studiohdr_t* hdr = I::ModelInfoClient->GetStudioModel(model);

		if (model && hdr)
		{
			Records[i].push_front({
				pEntity->GetSimulationTime(),
				I::GlobalVars->tickcount,
				pEntity->GetHitboxPos(HITBOX_HEAD),
				pEntity->GetAbsOrigin(),
				*reinterpret_cast<BoneMatrixes*>(&bones),
				model,
				hdr,
				pEntity->GetHitboxSet(),
				pEntity->m_vecMins(),
				pEntity->m_vecMaxs(),
				pEntity->GetWorldSpaceCenter(),
				pEntity->GetEyeAngles(),
			});
		}

		// Remove old out-of-range records
		while (static_cast<int>(Records[i].size()) > std::clamp(TIME_TO_TICKS(GetLatency()), 0, TIME_TO_TICKS(0.9f)))
		{
			Records[i].pop_back();
		}
	}
}

void CBacktrack::Run()
{
	if (!Vars::Backtrack::Enabled.Value)
	{
		LatencyRampup = 0.f;
		if (Vars::Aimbot::Projectile::StrafePredictionAir.Value || Vars::Aimbot::Projectile::StrafePredictionGround.Value)
		{
			if (g_EntityCache.GetLocal())
			{
				UpdateRecords();
			}
		}
		return;
	}

	LatencyRampup = std::min(1.f, LatencyRampup += I::GlobalVars->interval_per_tick);

	if (g_EntityCache.GetLocal())
	{
		UpdateDatagram();
		UpdateRecords();
		//else
		//{
		//	for (auto& record : Records)
		//	{
		//		record.clear();
		//	}
		//}
	}
	else
	{
		Sequences.clear();
	}
}

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

// Returns the last valid backtrack tick (further away from the player)
std::optional<TickRecord> CBacktrack::GetLastRecord(int entIdx)
{
	const auto& entRecord = Records[entIdx];
	if (!entRecord.empty()) {
		return { entRecord.back() };
	}

	return std::nullopt;
}

/* Returns the first valid backtrack tick(close to the player) */
std::optional<TickRecord> CBacktrack::GetFirstRecord(int entIdx)
{
	const auto& entRecord = Records[entIdx];
	for (const auto& record : entRecord)
	{
		if (IsTickInRange(record.TickCount))
		{
			return record;
		}
	}

	return std::nullopt;
}

/* Returns the best tick for the chosen mode */
std::optional<TickRecord> CBacktrack::GetRecord(int entIdx, BacktrackMode mode)
{
	switch (mode)
	{
	case BacktrackMode::First: { return GetFirstRecord(entIdx); }
	default: { return GetLastRecord(entIdx); }
	}
}