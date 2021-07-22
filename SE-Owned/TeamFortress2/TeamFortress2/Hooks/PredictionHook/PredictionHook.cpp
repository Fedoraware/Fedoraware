#include "PredictionHook.h"

void __stdcall PredictionHook::RunCommand::Hook(CBaseEntity *pEntity, CUserCmd *pCmd, CMoveHelper *pMoveHelper)
{
	Table.Original<fn>(index)(g_Interfaces.Prediction, pEntity, pCmd, pMoveHelper);

	if (pMoveHelper && !g_Interfaces.MoveHelper)
		g_Interfaces.MoveHelper = pMoveHelper;
}