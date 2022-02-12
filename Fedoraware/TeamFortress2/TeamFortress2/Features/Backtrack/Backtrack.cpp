#include "Backtrack.h"

float LerpTime()
{

	static ConVar* updaterate = g_Interfaces.CVars->FindVar("cl_updaterate");
	static ConVar* minupdate = g_Interfaces.CVars->FindVar("sv_minupdaterate");
	static ConVar* maxupdate = g_Interfaces.CVars->FindVar("sv_maxupdaterate");
	static ConVar* lerp = g_Interfaces.CVars->FindVar("cl_interp");
	static ConVar* cmin = g_Interfaces.CVars->FindVar("sv_client_min_interp_ratio");
	static ConVar* cmax = g_Interfaces.CVars->FindVar("sv_client_max_interp_ratio");
	static ConVar* ratio = g_Interfaces.CVars->FindVar("cl_interp_ratio");

	const float interpValue = lerp->GetFloat();
	const float maxUpdateValue = maxupdate->GetFloat();
	int updateRateValue = updaterate->GetInt();
	float interpRatioValue = ratio->GetFloat();
	const int svMaxUpdateRate = maxupdate->GetInt();
	const int svMinUpdateRate = minupdate->GetInt();
	const float minRatioValue = cmin->GetFloat();
	const float maxRatioValue = cmax->GetFloat();

	if (svMaxUpdateRate && svMinUpdateRate)
		updateRateValue = static_cast<int>(maxUpdateValue);

	if (interpRatioValue == 0)
		interpRatioValue = 1.0f;

	if (cmin && cmax && cmin->GetFloat() != 1)
		interpRatioValue = std::clamp(interpRatioValue, minRatioValue, maxRatioValue);

	return std::max(interpValue, interpRatioValue / static_cast<float>(updateRateValue));
}

bool CBacktrack::IsGoodTick(const int tick) const {
	const auto netChannel = g_Interfaces.Engine->GetNetChannelInfo();

	if (!netChannel) {
		return false;
	}

	const float correct = std::clamp(netChannel->GetLatency(FLOW_OUTGOING) + LerpTime(), 0.f, 1.f);

	const float deltaTime = correct - (g_Interfaces.GlobalVars->curtime - TICKS_TO_TIME(tick));

	return fabsf(deltaTime) < 0.2f;
}

void CBacktrack::Start(CUserCmd* pCmd) {
	if (const auto& pLocal = g_EntityCache.m_pLocal) {
		if (!pCmd) {
			return;
		}
		if (const auto& pWeapon = g_EntityCache.m_pLocalWeapon) {
			for (int i = 0; i < g_Interfaces.Engine->GetMaxClients(); i++) {
				if (CBaseEntity* pEntity = g_Interfaces.EntityList->GetClientEntity(i)) {
					if (!(pEntity->GetDormant() && pEntity->IsAlive())) {
						int hitbox = HITBOX_HEAD;

						matrix3x4 bones[128];

						pEntity->SetupBones(bones, 128, BONE_USED_BY_ANYTHING, 0.0f);

						Record[i].insert(Record[i].begin(), TickRecord(pEntity->GetSimulationTime(), pEntity->GetHitboxPos(hitbox), pEntity->GetAbsOrigin(), *reinterpret_cast<BoneMatrixes*>(&bones)));

						if (Record[i].size() > 12) {
							Record[i].pop_back();
						}
					}
				}
			}
		}

	}
}

void CBacktrack::Calculate(CUserCmd* pCmd) {
	if (const auto& pLocal = g_EntityCache.m_pLocal) {
		Vec3 newViewDirection;
		const Vec3 viewDirection = pCmd->viewangles;
		/*g_Interfaces.Engine->GetViewAngles(viewDirection);*/
		Math::AngleVectors(viewDirection, &newViewDirection);
		if (CBaseCombatWeapon* pWeapon = pLocal->GetActiveWeapon()) {
			int bestTargetIndex = -1; float bestFieldOfView = FLT_MAX;
			for (int i = 0; i < g_Interfaces.Engine->GetMaxClients(); i++) {
				CBaseEntity* pEntity = g_Interfaces.EntityList->GetClientEntity(i);
				if (!pEntity || pEntity->GetDormant() || pEntity->GetLifeState() != LIFE_ALIVE) {
					continue;
				}

				if (pEntity->GetTeamNum() == pLocal->GetTeamNum()) {
					continue;
				}

				if (Record[i].empty()) {
					continue;
				}

				if (const float fovDistance = Math::DistPointToLine(pEntity->GetEyePosition(), pLocal->GetEyePosition(), newViewDirection); fovDistance < bestFieldOfView) {
					bestFieldOfView = fovDistance;
					bestTargetIndex = i;
				}
			}

			float finalTargetIndex = -1;
			if (bestTargetIndex != -1) {
				for (const auto& i : Record[bestTargetIndex]) {
					if (const float fieldOfViewDistance = Math::DistPointToLine(i.HeadPosition, pLocal->GetEyePosition(), newViewDirection); fieldOfViewDistance < bestFieldOfView) {
						bestFieldOfView = fieldOfViewDistance;
						finalTargetIndex = i.SimulationTime;
					}
				}

				if (finalTargetIndex != -1) {
					if (pCmd->buttons & IN_ATTACK || g_GlobalInfo.m_bAttacking) {
						pCmd->tick_count = TIME_TO_TICKS(finalTargetIndex);
					}
				}
			}
		}
	}
}

void CBacktrack::Run(CUserCmd* pCmd) {
	if (g_EntityCache.m_pLocal && pCmd) {
		Start(pCmd);
		Calculate(pCmd);
	}
}

static int lastincomingsequence = 0;
std::deque<CIncomingSequence> sequences;
static float latencyRampup = 0.0f;

// Update our sequences
void CBacktrack::UpdateDatagram()
{
	INetChannel* ch = g_Interfaces.Engine->GetNetChannelInfo();
	if (ch)
	{
		int m_nInSequenceNr = ch->m_nInSequenceNr;
		int instate = ch->m_nInReliableState;
		if (m_nInSequenceNr > lastincomingsequence)
		{
			lastincomingsequence = m_nInSequenceNr;
			sequences.insert(sequences.begin(), CIncomingSequence(instate, m_nInSequenceNr, g_Interfaces.GlobalVars->realtime));
		}
		if (sequences.size() > 2048)
			sequences.pop_back();
	}
}

void CBacktrack::UpdateDatagrams(CUserCmd* pCmd) {
	if (!Vars::Backtrack::Enabled.m_Var) {
		latencyRampup = 0.0f;
		return;
	}

	if (g_EntityCache.m_pLocal && pCmd) {
		UpdateDatagram();
	} else {
		sequences.clear();
	}

	latencyRampup += g_Interfaces.GlobalVars->interval_per_tick;
	latencyRampup = std::min(1.0f, latencyRampup);
}

// Change Datagram data
void CBacktrack::AdjustPing(INetChannel* ch)
{
	if (!Vars::Backtrack::Enabled.m_Var)
		return;
	for (auto& seq : sequences)
	{
		if (g_Interfaces.GlobalVars->realtime - seq.curtime >= GetLatency() / 1000.0f)
		{
			ch->m_nInReliableState = seq.inreliablestate;
			ch->m_nInSequenceNr = seq.sequencenr;
			break;
		}
	}
}


// Latency to add for backtrack
float CBacktrack::GetLatency()
{
	INetChannel* ch = g_Interfaces.Engine->GetNetChannelInfo();
	// Track what actual latency we have
	float real_latency = 0.0f;

	// If we have a netchannel (just in case) set real latency to it
	if (ch)
		real_latency = ch->GetLatency(FLOW_OUTGOING) * 1000.0f;

	// Clamp and apply rampup, also ensure we do not go out of the 1000.0f bounds
	float backtrack_latency = latencyRampup * std::clamp(Vars::Backtrack::Latency.m_Var, 0.0f, 900.0f - real_latency);

	return backtrack_latency;
}