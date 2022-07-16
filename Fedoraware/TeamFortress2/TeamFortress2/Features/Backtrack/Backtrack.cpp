#include "Backtrack.h"

bool CBacktrack::IsGoodTick(const float simTime)
{
	const float latency = std::clamp(GetLatency(), 0.f, 0.9f);
	const float deltaTime = latency - (I::GlobalVars->curtime - simTime);

	return std::abs(deltaTime) <= 0.2f - TICKS_TO_TIME(2);
}

void CBacktrack::Start(const CUserCmd* pCmd)
{
	if (!pCmd) { return; }

	const auto& pLocal = g_EntityCache.GetLocal();
	if (!pLocal || !pLocal->IsAlive()) { return; }

	for (int i = 0; i < I::Engine->GetMaxClients(); i++)
	{
		if (CBaseEntity* pEntity = I::EntityList->GetClientEntity(i))
		{
			if (pEntity->GetDormant() || !pEntity->IsAlive())
			{
				Records[i].clear();
				continue;
			}
			
			matrix3x4 bones[128];
			pEntity->SetupBones(bones, 128, BONE_USED_BY_ANYTHING, 0.0f);
			
			model_t* model = pEntity->GetModel();
			studiohdr_t* hdr = I::ModelInfo->GetStudioModel(model);

			if (model && hdr)
			{
				Records[i].push_front({
					pEntity->GetSimulationTime(),
					pEntity->GetHitboxPos(HITBOX_HEAD),
					pEntity->GetAbsOrigin(),
					*reinterpret_cast<BoneMatrixes*>(&bones),
					model,
					hdr,
					pEntity->GetHitboxSet(),
					pEntity->m_vecMins(),
					pEntity->m_vecMaxs(),
					pEntity->GetWorldSpaceCenter(),
					pEntity->GetEyeAngles()
				});
			}

			// Remove old out-of-range records
			while (Records[i].size() > std::clamp(TIME_TO_TICKS(GetLatency()), 0, TIME_TO_TICKS(0.9f)))
			{
				Records[i].pop_back();
			}
		}
	}
}

void CBacktrack::Run(const CUserCmd* pCmd)
{
	if (!Vars::Backtrack::Enabled.Value)
	{
		LatencyRampup = 0.f;
		return;
	}

	LatencyRampup += I::GlobalVars->interval_per_tick;
	LatencyRampup = std::min(1.f, LatencyRampup);

	if (g_EntityCache.GetLocal() && pCmd)
	{
		UpdateDatagram();
		if (G::CurWeaponType != EWeaponType::PROJECTILE)
		{
			Start(pCmd);
		}
		else
		{
			for (auto& a : Records)
			{
				a.clear();
			}
		}
		
		/*Calculate(pCmd);*/
	}
	else
	{
		Sequences.clear();
	}
}

// Store the last 2048 sequences
void CBacktrack::UpdateDatagram()
{
	const INetChannel* netChannel = I::Engine->GetNetChannelInfo();
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

	if (const auto netChannel = I::Engine->GetNetChannelInfo())
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

std::deque<TickRecord>* CBacktrack::GetPlayerRecord(int iEntityIndex)
{
	if (Records[iEntityIndex].empty())
	{
		return nullptr;
	}
	return &Records[iEntityIndex];
}

std::deque<TickRecord>* CBacktrack::GetPlayerRecord(CBaseEntity* pEntity)
{
	if (!pEntity)
	{
		return nullptr;
	}

	const auto entindex = pEntity->GetIndex();
	if (Records[entindex].empty())
	{
		return nullptr;
	}
	return &Records[entindex];
}
