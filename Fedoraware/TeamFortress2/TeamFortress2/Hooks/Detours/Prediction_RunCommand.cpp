#include "../Hooks.h"

int CalculateTick(int simTicks, CBaseEntity* player)
{
	const int clockcorrect = TIME_TO_TICKS(0.06f); //sv_clockcorrectmsecs
	const int nIdealFinalTick = I::GlobalVars->tickcount + TIME_TO_TICKS(
		I::Engine->GetNetChannelInfo()->GetLatency(0)) + clockcorrect;
	const int estimatedFinal = player->GetTickBase() + simTicks;
	const int fast = nIdealFinalTick + clockcorrect;
	const int slow = nIdealFinalTick - clockcorrect;
	if (estimatedFinal > fast || estimatedFinal < slow) { return nIdealFinalTick - simTicks; }
	// this is useless
	return estimatedFinal;
}

MAKE_HOOK(Prediction_RunCommand, Utils::GetVFuncPtr(I::Prediction, 17), void, __fastcall,
		  void* ecx, void* edx, CBaseEntity* pEntity, CUserCmd* pCmd, CMoveHelper* pMoveHelper)
{
	
	if (pMoveHelper && !I::MoveHelper)
	{
		I::MoveHelper = pMoveHelper;
	}

	const auto& pLocal = g_EntityCache.m_pLocal;

	if (pEntity && pEntity->IsAlive() && pLocal && pLocal->IsAlive() && pLocal == pEntity && pCmd && pCmd->command_number)
	{
		const int backupTick = pEntity->GetTickBase();
		const float curtimeBackup = I::GlobalVars->curtime;

		if (Vars::Misc::CL_Move::Doubletap.m_Var && g_GlobalInfo.m_bShouldShift)
		{
			if (pCmd->command_number == g_GlobalInfo.lateUserCmd->command_number)
			{
				pEntity->SetTickBase(CalculateTick(I::ClientState->chokedcommands + g_GlobalInfo.m_nShifted + 1,
										pEntity));
				I::GlobalVars->curtime = TICKS_TO_TIME(pEntity->GetTickBase());
			}
		}

		Hook.Original<FN>()(ecx, edx, pEntity, pCmd, pMoveHelper);

		if (Vars::Misc::CL_Move::Doubletap.m_Var && g_GlobalInfo.m_bShouldShift)
		{
			if (pCmd->command_number == g_GlobalInfo.lateUserCmd->command_number)
			{
				pEntity->SetTickBase(backupTick);
				I::GlobalVars->curtime = curtimeBackup;
			}
		}
	}
	else
	{
		Hook.Original<FN>()(ecx, edx, pEntity, pCmd, pMoveHelper);
	}
}