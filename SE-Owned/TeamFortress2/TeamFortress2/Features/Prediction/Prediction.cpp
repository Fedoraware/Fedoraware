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

	if (pLocal && pLocal->IsAlive() && g_Interfaces.MoveHelper)
	{
		pLocal->SetCurrentCmd(pCmd);

		*g_Interfaces.RandomSeed = MD5_PseudoRandom(pCmd->command_number) & std::numeric_limits<int>::max();

		m_fOldCurrentTime = g_Interfaces.GlobalVars->curtime;
		m_fOldFrameTime = g_Interfaces.GlobalVars->frametime;
		m_nOldTickCount = g_Interfaces.GlobalVars->tickcount;

		const int nOldTickBase = pLocal->GetTickBase();
		const bool bOldIsFirstPrediction = g_Interfaces.Prediction->m_bFirstTimePredicted;
		const bool bOldInPrediction = g_Interfaces.Prediction->m_bInPrediction;

		g_Interfaces.GlobalVars->curtime = TICKS_TO_TIME(GetTickbase(pCmd, pLocal));
		g_Interfaces.GlobalVars->frametime = (g_Interfaces.Prediction->m_bEnginePaused ? 0.0f : TICK_INTERVAL);
		g_Interfaces.GlobalVars->tickcount = GetTickbase(pCmd, pLocal);

		g_Interfaces.Prediction->m_bFirstTimePredicted = false;
		g_Interfaces.Prediction->m_bInPrediction = true;

		g_Interfaces.Prediction->SetLocalViewAngles(pCmd->viewangles);

		g_Interfaces.MoveHelper->SetHost(pLocal);

		g_Interfaces.Prediction->SetupMove(pLocal, pCmd, g_Interfaces.MoveHelper, &m_MoveData);
		g_Interfaces.GameMovement->ProcessMovement(pLocal, &m_MoveData);
		g_Interfaces.Prediction->FinishMove(pLocal, pCmd, &m_MoveData);

		pLocal->SetTickBase(nOldTickBase);

		g_Interfaces.Prediction->m_bInPrediction = bOldInPrediction;
		g_Interfaces.Prediction->m_bFirstTimePredicted = bOldIsFirstPrediction;
	}
}

void CEnginePrediction::End(CUserCmd* pCmd)
{
	CBaseEntity* pLocal = g_EntityCache.m_pLocal;

	if (pLocal && pLocal->IsAlive() && g_Interfaces.MoveHelper)
	{
		g_Interfaces.MoveHelper->SetHost(nullptr);

		g_Interfaces.GlobalVars->curtime = m_fOldCurrentTime;
		g_Interfaces.GlobalVars->frametime = m_fOldFrameTime;
		g_Interfaces.GlobalVars->tickcount = m_nOldTickCount;

		pLocal->SetCurrentCmd(nullptr);

		*g_Interfaces.RandomSeed = -1;
	}
}