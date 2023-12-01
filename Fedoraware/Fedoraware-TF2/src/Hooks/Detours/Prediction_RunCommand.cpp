#include "../Hooks.h"

/*
	https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/server/player.cpp#L3380
	- bradley wuzz here fr fr on goh
*/
inline void TickCorrection(const int iSimTicks, CBaseEntity* pLocal) {
	const int iClockCorrect = TIME_TO_TICKS(I::Cvar->FindVar("sv_clockcorrection_msecs")->GetFloat());

	const int iIdealEnd = I::GlobalVars->tickcount + TIME_TO_TICKS(I::EngineClient->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING)) + iClockCorrect;
	const int iActualEnd = pLocal->GetTickBase() + iSimTicks;

	if (iActualEnd > iIdealEnd + iClockCorrect ||
		iActualEnd < iIdealEnd - iClockCorrect) {
		pLocal->SetTickBase(iIdealEnd - iSimTicks + 1);
		I::GlobalVars->curtime = TICKS_TO_TIME(pLocal->GetTickBase());
	}
}

MAKE_HOOK(Prediction_RunCommand, Utils::GetVFuncPtr(I::Prediction, 17), void, __fastcall,
		  void* ecx, void* edx, CBaseEntity* pEntity, CUserCmd* pCmd, CMoveHelper* pMoveHelper)
{
	CBaseEntity* pLocal = g_EntityCache.GetLocal();

	if (pLocal && pLocal == pEntity && pCmd) {
		if (G::Recharging) {
			return;
		}

		if (G::ShouldShift) {
			const int iTickbaseBackup = pLocal->GetTickBase();
			const float flCurtimeBackup = I::GlobalVars->curtime;

			TickCorrection(I::ClientState->chokedcommands + G::ShiftedTicks, pLocal);
			Hook.Original<FN>()(ecx, edx, pEntity, pCmd, pMoveHelper);

			pLocal->SetTickBase(iTickbaseBackup);
			I::GlobalVars->curtime = flCurtimeBackup;
		}
		else {
			Hook.Original<FN>()(ecx, edx, pEntity, pCmd, pMoveHelper);
		}
	}
	else {
		Hook.Original<FN>()(ecx, edx, pEntity, pCmd, pMoveHelper);
	}

	//credits: KGB
	if (pLocal && !pLocal->InCond(TF_COND_HALLOWEEN_KART)) {
		if (!pCmd->hasbeenpredicted && pEntity == pLocal) {
			if (CTFPlayerAnimState* pAnimState = pLocal->GetAnimState()) {
				const float flOldFrameTime = I::GlobalVars->frametime;
				I::GlobalVars->frametime = I::Prediction->m_bEnginePaused ? 0.0f : TICK_INTERVAL;
				pAnimState->Update(pCmd->viewangles.y, pCmd->viewangles.x);
				pLocal->FrameAdvance(I::GlobalVars->frametime);
				I::GlobalVars->frametime = flOldFrameTime;
			}
		}
	}
}