#include "Backtrack.h"

static float LerpTime()
{
	static ConVar* updaterate = I::CVars->FindVar("cl_updaterate");
	static ConVar* minupdate = I::CVars->FindVar("sv_minupdaterate");
	static ConVar* maxupdate = I::CVars->FindVar("sv_maxupdaterate");
	static ConVar* lerp = I::CVars->FindVar("cl_interp");
	static ConVar* cmin = I::CVars->FindVar("sv_client_min_interp_ratio");
	static ConVar* cmax = I::CVars->FindVar("sv_client_max_interp_ratio");
	static ConVar* ratio = I::CVars->FindVar("cl_interp_ratio");

	const float interpValue = lerp->GetFloat();
	const float maxUpdateValue = maxupdate->GetFloat();
	int updateRateValue = updaterate->GetInt();
	float interpRatioValue = ratio->GetFloat();
	const int svMaxUpdateRate = maxupdate->GetInt();
	const int svMinUpdateRate = minupdate->GetInt();
	const float minRatioValue = cmin->GetFloat();
	const float maxRatioValue = cmax->GetFloat();

	if (svMaxUpdateRate && svMinUpdateRate)
	{
		updateRateValue = static_cast<int>(maxUpdateValue);
	}

	if (interpRatioValue == 0)
	{
		interpRatioValue = 1.0f;
	}

	if (cmin && cmax && cmin->GetFloat() != 1)
	{
		interpRatioValue = std::clamp(interpRatioValue, minRatioValue, maxRatioValue);
	}

	return std::max(interpValue, interpRatioValue / static_cast<float>(updateRateValue));
}

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

	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (const auto& pWeapon = g_EntityCache.m_pLocalWeapon)
		{
			for (int i = 0; i < I::Engine->GetMaxClients(); i++)
			{
				if (CBaseEntity* pEntity = I::EntityList->GetClientEntity(i))
				{
					if (pEntity->GetDormant() || !pEntity->IsAlive())
					{
						Record[i].clear();
						continue;
					}

					int hitbox = HITBOX_HEAD;

					matrix3x4 bones[128];

					pEntity->SetupBones(bones, 128, BONE_USED_BY_ANYTHING, 0.0f);

					model_t* model = pEntity->GetModel();
					studiohdr_t* hdr = I::ModelInfo->GetStudioModel(model);

					if (model && hdr)
					{
						Record[i].insert(Record[i].begin(), TickRecord(
							                 pEntity->GetSimulationTime(),
							                 pEntity->GetHitboxPos(hitbox),
							                 pEntity->GetAbsOrigin(),
							                 *reinterpret_cast<BoneMatrixes*>(&bones),
							                 model,
							                 hdr,
							                 pEntity->GetHitboxSet()));
					}

					while (Record[i].size() > std::clamp(TIME_TO_TICKS(GetLatency()), 0, TIME_TO_TICKS(0.9f)))
					{
						Record[i].pop_back();
					}
				}
			}
		}
	}
}

void CBacktrack::Calculate(CUserCmd* pCmd)
{
	if (const auto& pLocal = g_EntityCache.m_pLocal)
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

				if (Record[i].empty())
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
				for (auto& i : Record[bestTargetIndex])
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
					if (!g_GlobalInfo.m_bShouldShift && g_GlobalInfo.m_nShifted == 0)
					{
						if (pCmd->buttons & IN_ATTACK || g_GlobalInfo.m_bAttacking)
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
	if (!Vars::Backtrack::Enabled.m_Var)
	{
		LatencyRampup = 0.f;
		return;
	}

	LatencyRampup += I::GlobalVars->interval_per_tick;
	LatencyRampup = std::min(1.f, LatencyRampup);

	if (g_EntityCache.m_pLocal && pCmd)
	{
		UpdateDatagram();

		Start(pCmd);
		Calculate(pCmd);
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
		static int lastInSequence = 0;
		if (netChannel->m_nInSequenceNr > lastInSequence)
		{
			lastInSequence = netChannel->m_nInSequenceNr;
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
	
	return LatencyRampup * std::clamp(Vars::Backtrack::Latency.m_Var * 0.001f, 0.f, 0.9f - realLatency);
}

// Adjusts the fake latency ping
void CBacktrack::AdjustPing(INetChannel* netChannel)
{
	for (const auto& sequence : Sequences)
	{
		if (I::GlobalVars->realtime - sequence.CurTime >= GetLatency())
		{
			netChannel->m_nInReliableState = sequence.IsReliableState;
			netChannel->m_nInSequenceNr = sequence.SequenceNr;
			break;
		}
	}
}