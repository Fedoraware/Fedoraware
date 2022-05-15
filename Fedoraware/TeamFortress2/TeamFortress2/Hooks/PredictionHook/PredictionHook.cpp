#include "PredictionHook.h"

int CalculateTick(int simTicks, CBaseEntity* player)
{
	const int clockcorrect = TIME_TO_TICKS(0.06f); //sv_clockcorrectmsecs
	const int nIdealFinalTick = g_Interfaces.GlobalVars->tickcount + TIME_TO_TICKS(
		g_Interfaces.Engine->GetNetChannelInfo()->GetLatency(0)) + clockcorrect;
	const int estimatedFinal = player->GetTickBase() + simTicks;
	const int fast = nIdealFinalTick + clockcorrect;
	const int slow = nIdealFinalTick - clockcorrect;
	if (estimatedFinal > fast || estimatedFinal < slow) { return nIdealFinalTick - simTicks; }
	// this is useless
	return estimatedFinal;
}

void __stdcall PredictionHook::RunCommand::Hook(CBaseEntity* pEntity, CUserCmd* pCmd, CMoveHelper* pMoveHelper)
{
	if (pMoveHelper && !g_Interfaces.MoveHelper)
	{
		g_Interfaces.MoveHelper = pMoveHelper;
	}

	const auto& pLocal = g_EntityCache.m_pLocal;

	if (pEntity && pEntity->IsAlive() && pLocal && pLocal->IsAlive() && pLocal == pEntity && pCmd && pCmd->command_number)
	{
		const int backupTick = pEntity->GetTickBase();
		const float curtimeBackup = g_Interfaces.GlobalVars->curtime;

		if (Vars::Misc::CL_Move::Doubletap.m_Var && g_GlobalInfo.m_bShouldShift)
		{
			if (pCmd->command_number == g_GlobalInfo.lateUserCmd->command_number)
			{
				pEntity->SetTickBase(CalculateTick(g_Interfaces.ClientState->chokedcommands + g_GlobalInfo.m_nShifted + 1,
				                                   pEntity));
				g_Interfaces.GlobalVars->curtime = TICKS_TO_TIME(pEntity->GetTickBase());
			}
		}

		Table.Original<fn>(index)(g_Interfaces.Prediction, pEntity, pCmd, pMoveHelper);

		if (Vars::Misc::CL_Move::Doubletap.m_Var && g_GlobalInfo.m_bShouldShift)
		{
			if (pCmd->command_number == g_GlobalInfo.lateUserCmd->command_number)
			{
				pEntity->SetTickBase(backupTick);
				g_Interfaces.GlobalVars->curtime = curtimeBackup;
			}
		}
	}
	else
	{
		Table.Original<fn>(index)(g_Interfaces.Prediction, pEntity, pCmd, pMoveHelper);
	}
}
