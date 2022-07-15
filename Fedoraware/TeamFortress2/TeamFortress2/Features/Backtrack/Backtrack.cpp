#include "Backtrack.h"

bool CBacktrack::IsGoodTick(const int tick)
{
	const auto netChannel = I::Engine->GetNetChannelInfo();

	if (!netChannel)
	{
		return false;
	}

	const float correct = std::clamp(GetLatency(), 0.f, 0.9f);

	const float deltaTime = correct - (I::GlobalVars->curtime - TICKS_TO_TIME(tick));

	return fabsf(deltaTime) <= 0.2f - TICKS_TO_TIME(2);
}

void CBacktrack::Start(const CUserCmd* pCmd)
{
	if (!pCmd) { return; }

	if (const auto& pLocal = g_EntityCache.GetLocal())
	{
		if (const auto& pWeapon = g_EntityCache.GetWeapon())
		{
			for (int i = 0; i < I::Engine->GetMaxClients(); i++)
			{
				if (CBaseEntity* pEntity = I::EntityList->GetClientEntity(i))
				{
					if (pEntity->GetDormant() || !pEntity->IsAlive())
					{
						Records[i].clear();
						continue;
					}

					int hitbox = HITBOX_HEAD;

					matrix3x4 bones[128];

					pEntity->SetupBones(bones, 128, BONE_USED_BY_ANYTHING, 0.0f);

					model_t* model = pEntity->GetModel();
					studiohdr_t* hdr = I::ModelInfo->GetStudioModel(model);

					Vec3 mins = pEntity->m_vecMins();
					Vec3 maxs = pEntity->m_vecMaxs();
					Vec3 worldspacecenter = pEntity->GetWorldSpaceCenter();
					Vec3 eyeangles = pEntity->GetEyeAngles();


					if (model && hdr)
					{
						Records[i].insert(Records[i].begin(), TickRecord(
							pEntity->GetSimulationTime(),
							pEntity->GetHitboxPos(hitbox),
							pEntity->GetAbsOrigin(),
							*reinterpret_cast<BoneMatrixes*>(&bones),
							model,
							hdr,
							pEntity->GetHitboxSet(),
							mins,
							maxs,
							worldspacecenter,
							eyeangles)
						);
					}

					while (Records[i].size() > std::clamp(TIME_TO_TICKS(GetLatency()), 0, TIME_TO_TICKS(0.9f)))
					{
						Records[i].pop_back();
					}
				}
			}
		}
	}
}

void CBacktrack::Calculate(CUserCmd* pCmd)
{
	if (const auto& pLocal = g_EntityCache.GetLocal())
	{
		Vec3 newViewDirection;
		const Vec3 viewDirection = pCmd->viewangles;
		/*I::Engine->GetViewAngles(viewDirection);*/
		Math::AngleVectors(viewDirection, &newViewDirection);
		if (CBaseCombatWeapon* pWeapon = pLocal->GetActiveWeapon())
		{
			int bestTargetIndex = -1;
			float bestFieldOfView = FLT_MAX;
			for (int i = 0; i < I::Engine->GetMaxClients(); i++)
			{
				CBaseEntity* pEntity = I::EntityList->GetClientEntity(i);
				if (!pEntity || pEntity->GetDormant() || pEntity->GetLifeState() != LIFE_ALIVE)
				{
					continue;
				}

				if (pEntity->GetTeamNum() == pLocal->GetTeamNum())
				{
					continue;
				}

				if (Records[i].empty())
				{
					continue;
				}

				if (const float fovDistance = Math::DistPointToLine(pEntity->GetEyePosition(), pLocal->GetEyePosition(), newViewDirection); fovDistance < bestFieldOfView)
				{
					bestFieldOfView = fovDistance;
					bestTargetIndex = i;
				}
			}

			float finalTargetIndex = -1;
			if (bestTargetIndex != -1)
			{
				for (auto& i : Records[bestTargetIndex])
				{
					if (const float fieldOfViewDistance = Math::DistPointToLine(i.HeadPosition, pLocal->GetEyePosition(), newViewDirection); fieldOfViewDistance < bestFieldOfView)
					{
						bestFieldOfView = fieldOfViewDistance;
						finalTargetIndex = i.SimulationTime;
					}
					i.AimedAt = true;
				}

				if (finalTargetIndex != -1)
				{
					if (!G::ShouldShift && G::ShiftedTicks == 0)
					{
						if (pCmd->buttons & IN_ATTACK || G::IsAttacking)
						{
							pCmd->tick_count = TIME_TO_TICKS(finalTargetIndex);
						}
					}
				}
			}
		}
	}
}

void CBacktrack::Run(CUserCmd* pCmd)
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

std::vector<TickRecord>* CBacktrack::GetPlayerRecord(int iEntityIndex)
{
	if (Records[iEntityIndex].empty())
	{
		return nullptr;
	}
	return &Records[iEntityIndex];
}

std::vector<TickRecord>* CBacktrack::GetPlayerRecord(CBaseEntity* pEntity)
{
	if (!pEntity)
	{
		return nullptr;
	}
	auto entindex = pEntity->GetIndex();
	if (Records[entindex].empty())
	{
		return nullptr;
	}
	return &Records[entindex];
}
