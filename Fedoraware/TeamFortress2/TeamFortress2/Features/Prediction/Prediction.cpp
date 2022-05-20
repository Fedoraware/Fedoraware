#include "Prediction.h"

int CEnginePrediction::GetTickbase(CUserCmd* pCmd, CBaseEntity* pLocal)
{
	static int nTick = 0;
	static CUserCmd* pLastCmd = nullptr;

	if (pCmd)
	{
		if (!pLastCmd || pLastCmd->hasbeenpredicted)
			nTick = pLocal->GetTickBase();

		else nTick++;

		pLastCmd = pCmd;
	}

	return nTick;
}

void CEnginePrediction::Start(CUserCmd* pCmd)
{
	CBaseEntity* pLocal = g_EntityCache.m_pLocal;

	if (pLocal && pLocal->IsAlive() && I::MoveHelper)
	{
		pLocal->SetCurrentCmd(pCmd);
		oldrandomseed = *I::RandomSeed;
		*I::RandomSeed = MD5_PseudoRandom(pCmd->command_number) & std::numeric_limits<int>::max();

		m_fOldCurrentTime = I::GlobalVars->curtime;
		m_fOldFrameTime = I::GlobalVars->frametime;
		m_nOldTickCount = I::GlobalVars->tickcount;

		const int nOldTickBase = pLocal->GetTickBase();
		const bool bOldIsFirstPrediction = I::Prediction->m_bFirstTimePredicted;
		const bool bOldInPrediction = I::Prediction->m_bInPrediction;

		I::GlobalVars->curtime = TICKS_TO_TIME(GetTickbase(pCmd, pLocal));
		I::GlobalVars->frametime = (I::Prediction->m_bEnginePaused ? 0.0f : TICK_INTERVAL);
		I::GlobalVars->tickcount = GetTickbase(pCmd, pLocal);

		I::Prediction->m_bFirstTimePredicted = false;
		I::Prediction->m_bInPrediction = true;

		I::Prediction->SetLocalViewAngles(pCmd->viewangles);

		I::MoveHelper->SetHost(pLocal);

		I::Prediction->SetupMove(pLocal, pCmd, I::MoveHelper, &m_MoveData);
		I::GameMovement->ProcessMovement(pLocal, &m_MoveData);
		I::Prediction->FinishMove(pLocal, pCmd, &m_MoveData);

		pLocal->SetTickBase(nOldTickBase);

		I::Prediction->m_bInPrediction = bOldInPrediction;
		I::Prediction->m_bFirstTimePredicted = bOldIsFirstPrediction;
	}
}

void CEnginePrediction::End(CUserCmd* pCmd)
{
	CBaseEntity* pLocal = g_EntityCache.m_pLocal;

	if (pLocal && pLocal->IsAlive() && I::MoveHelper)
	{
		I::MoveHelper->SetHost(nullptr);

		I::GlobalVars->curtime = m_fOldCurrentTime;
		I::GlobalVars->frametime = m_fOldFrameTime;
		I::GlobalVars->tickcount = m_nOldTickCount;

		pLocal->SetCurrentCmd(nullptr);

		*I::RandomSeed = -1; 
	}
}
