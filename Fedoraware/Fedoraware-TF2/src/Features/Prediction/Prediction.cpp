#include "Prediction.h"

namespace S
{
	MAKE_SIGNATURE(ResetInstanceCounters, CLIENT_DLL, "68 ? ? ? ? 6A ? 68 ? ? ? ? C7 05 ? ? ? ? ? ? ? ? E8 ? ? ? ? 83 C4 ? C3", 0x0);
}

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
	// credits for some sigs https://www.unknowncheats.me/forum/3333826-post1.html

	CBaseEntity* pLocal = g_EntityCache.GetLocal();

	if (pLocal && pLocal->IsAlive() && I::MoveHelper && !G::ShouldShift)
	{
		static auto fnResetInstanceCounters = S::ResetInstanceCounters.As<void(__cdecl*)()>();

		fnResetInstanceCounters();
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

		I::GameMovement->StartTrackPredictionErrors(pLocal);

		//if (pCmd->weaponselect) {
		//	if (CBaseCombatWeapon* pWeapon = pLocal->GetActiveWeapon()) { 
		//		pLocal->SelectItem(pWeapon->GetName(), pCmd->weaponsubtype); 
		//	}
		//}

		pLocal->UpdateButtonState(pCmd->buttons);

		I::Prediction->SetLocalViewAngles(pCmd->viewangles);

		const int iThinkTick = pLocal->m_nNextThinkTick();

		if (pLocal->PhysicsRunThink(0)) {
			pLocal->PreThink();
		}

		if (iThinkTick > 0 && iThinkTick <= I::GlobalVars->tickcount) {
			pLocal->SetNextThink(-1, NULL);
			pLocal->Think();
		}

		I::MoveHelper->SetHost(pLocal);

		I::Prediction->SetupMove(pLocal, pCmd, I::MoveHelper, &m_MoveData);
		I::GameMovement->ProcessMovement(pLocal, &m_MoveData);
		I::Prediction->FinishMove(pLocal, pCmd, &m_MoveData);

		pLocal->PostThink();
		I::GameMovement->FinishTrackPredictionErrors(pLocal);
		pLocal->SetTickBase(nOldTickBase);

		I::Prediction->m_bInPrediction = bOldInPrediction;
		I::Prediction->m_bFirstTimePredicted = bOldIsFirstPrediction;
	}
}

void CEnginePrediction::End(CUserCmd* pCmd)
{
	CBaseEntity* pLocal = g_EntityCache.GetLocal();

	if (pLocal && pLocal->IsAlive() && I::MoveHelper && !G::ShouldShift)
	{
		I::MoveHelper->SetHost(nullptr);

		I::GlobalVars->curtime = m_fOldCurrentTime;
		I::GlobalVars->frametime = m_fOldFrameTime;
		I::GlobalVars->tickcount = m_nOldTickCount;
		pLocal->SetCurrentCmd(nullptr);

		*I::RandomSeed = -1;
	}
}
