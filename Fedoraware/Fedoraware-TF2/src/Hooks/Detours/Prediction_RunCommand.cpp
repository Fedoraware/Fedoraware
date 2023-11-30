#include "../Hooks.h"

int CalculateTick(int simTicks, CBaseEntity* player)
{
	const int clockcorrect = TIME_TO_TICKS(0.06f); //sv_clockcorrectmsecs

	const int nIdealFinalTick = I::GlobalVars->tickcount + TIME_TO_TICKS(I::EngineClient->GetNetChannelInfo()->GetLatency(0)) + clockcorrect;

	const int estimatedFinal = player->GetTickBase() + simTicks;

	const int fast = nIdealFinalTick + clockcorrect;
	const int slow = nIdealFinalTick - clockcorrect;

	if (estimatedFinal > fast || estimatedFinal < slow) { 
		return nIdealFinalTick - simTicks; 
	}

	// this is useless
	return estimatedFinal;
}

MAKE_HOOK(Prediction_RunCommand, Utils::GetVFuncPtr(I::Prediction, 17), void, __fastcall,
		  void* ecx, void* edx, CBaseEntity* pEntity, CUserCmd* pCmd, CMoveHelper* pMoveHelper)
{
	CBaseEntity* pLocal = g_EntityCache.GetLocal();

	if (pLocal == pEntity && pLocal->IsAlive() && pCmd && pCmd->command_number) {
		if (G::Recharging) {
			return;
		}

		if ((Vars::Misc::CL_Move::Doubletap.Value && G::ShouldShift) &&
			(pCmd->command_number == G::LastUserCmd->command_number)) {
			const int iTickbaseBackup = pLocal->GetTickBase();
			const float iCurtimeBackup = I::GlobalVars->curtime;

			pLocal->SetTickBase(CalculateTick(I::ClientState->chokedcommands + G::ShiftedTicks + 1, pLocal));
			I::GlobalVars->curtime = TICKS_TO_TIME(pLocal->GetTickBase());

			Hook.Original<FN>()(ecx, edx, pEntity, pCmd, pMoveHelper);

			pLocal->SetTickBase(iTickbaseBackup);
			I::GlobalVars->curtime = iCurtimeBackup;
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