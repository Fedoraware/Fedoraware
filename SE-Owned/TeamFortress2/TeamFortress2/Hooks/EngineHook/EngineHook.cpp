#include "EngineHook.h"

#include "../../Features/Vars.h"

void __cdecl EngineHook::CL_Move::Hook(float accumulated_extra_samples, bool bFinalTick)
{
	if (Vars::Misc::CL_Move::Enabled.m_Var)
	{
		if (Vars::Misc::CL_Move::TeleportKey.m_Var && (GetAsyncKeyState(Vars::Misc::CL_Move::TeleportKey.m_Var)) && !g_GlobalInfo.m_nShifted)
		{
			while (g_GlobalInfo.m_nShifted < MAX_NEW_COMMANDS)
			{
				Func.Original<fn>()(accumulated_extra_samples, (g_GlobalInfo.m_nShifted == (MAX_NEW_COMMANDS - 1)));
				g_GlobalInfo.m_nShifted++;
			}

			return;
		}

		if (GetAsyncKeyState(Vars::Misc::CL_Move::RechargeKey.m_Var))
			g_GlobalInfo.m_bRecharging = true;
	}

	if (g_GlobalInfo.m_bRecharging && g_GlobalInfo.m_nShifted)
	{
		g_GlobalInfo.m_nShifted--;
		g_GlobalInfo.m_nWaitForShift = DT_WAIT_CALLS;
		return;
	}
	else
		g_GlobalInfo.m_bRecharging = false;

	Func.Original<fn>()(accumulated_extra_samples, (g_GlobalInfo.m_bShouldShift && !g_GlobalInfo.m_nWaitForShift) ? true : bFinalTick);

	if (g_GlobalInfo.m_nWaitForShift)
	{
		g_GlobalInfo.m_nWaitForShift--;
		return;
	}

	if (g_GlobalInfo.m_bShouldShift)
	{
		while (g_GlobalInfo.m_nShifted < MAX_NEW_COMMANDS)
		{
			Func.Original<fn>()(accumulated_extra_samples, (g_GlobalInfo.m_nShifted == (MAX_NEW_COMMANDS - 1)));
			g_GlobalInfo.m_nShifted++;
		}

		g_GlobalInfo.m_bShouldShift = false;
	}
}