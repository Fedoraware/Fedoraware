#include "EngineHook.h"

#include "../../Features/Vars.h"

#include "../../Features/Misc/Misc.h"



void __cdecl EngineHook::CL_Move::Hook(float accumulated_extra_samples, bool bFinalTick)
{
	if (Vars::Misc::CL_Move::Enabled.m_Var)
	{
		if (Vars::Misc::CL_Move::TeleportKey.m_Var && (GetAsyncKeyState(Vars::Misc::CL_Move::TeleportKey.m_Var)) && !g_GlobalInfo.m_nShifted) //teleport
		{
			while (g_GlobalInfo.m_nShifted < MAX_NEW_COMMANDS_HEAVY)
			{
				g_GlobalInfo.m_nShifted++;
				Func.Original<fn>()(accumulated_extra_samples, (g_GlobalInfo.m_nShifted == (MAX_NEW_COMMANDS_HEAVY - 1))); //this teleports you
				//g_GlobalInfo.m_nShifted++;
			}

			return;
		}

		if (GetAsyncKeyState(Vars::Misc::CL_Move::RechargeKey.m_Var)) //recharge key
			g_GlobalInfo.m_bRecharging = true;
	}

	if (g_GlobalInfo.m_bRecharging && g_GlobalInfo.m_nShifted) //recharge
	{
		g_GlobalInfo.m_nShifted--; //goes from 15 to 0
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
		const auto& pLocal = g_EntityCache.m_pLocal;
		int nClass = pLocal->GetClassNum();
		int dtTicks;
		if (nClass == CLASS_HEAVY) {
			dtTicks = MAX_NEW_COMMANDS_HEAVY;
		}
		else {
			dtTicks = MAX_NEW_COMMANDS;
		}
		if (GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)) {
			while (g_GlobalInfo.m_nShifted < dtTicks)
			{

				if (!Vars::Misc::CL_Move::NotInAir.m_Var) {
					Func.Original<fn>()(accumulated_extra_samples, (g_GlobalInfo.m_nShifted == (dtTicks - 1))); //this doubletaps
					g_GlobalInfo.m_nShifted++;
				}
				if (Vars::Misc::CL_Move::NotInAir.m_Var) {

					if (pLocal->IsOnGround()) {
						g_GlobalInfo.fast_stop = true;
						Func.Original<fn>()(accumulated_extra_samples, (g_GlobalInfo.m_nShifted == (dtTicks - 1))); //this doubletaps
						g_GlobalInfo.m_nShifted++;
					}
					else {
						return;
					}
				}
			}
		}

		g_GlobalInfo.m_bShouldShift = false;
	}
}