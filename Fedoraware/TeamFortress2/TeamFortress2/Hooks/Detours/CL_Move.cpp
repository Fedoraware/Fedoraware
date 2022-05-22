#include "../Hooks.h"

#include "../../Features/Misc/Misc.h"

MAKE_HOOK(CL_Move, g_Pattern.Find(L"engine.dll", L"55 8B EC 83 EC ? 83 3D ? ? ? ? 02 0F 8C ? ? 00 00 E8 ? ? ? 00 84 C0"), void, __cdecl,
		  float accumulated_extra_samples, bool bFinalTick)
{
	static auto oClMove = Hook.Original<FN>();

	if (!Vars::Misc::CL_Move::Enabled.m_Var)
	{
		g_GlobalInfo.m_nShifted = 0;
		return oClMove(accumulated_extra_samples, bFinalTick);
	}

	if (g_GlobalInfo.m_nShifted > Vars::Misc::CL_Move::DTTicks.m_Var)
	{
		g_GlobalInfo.m_nShifted -= 1;
		oClMove(accumulated_extra_samples, (g_GlobalInfo.m_nShifted == Vars::Misc::CL_Move::DTTicks.m_Var + 1));
	} // pCode

	// pSpeedhack
	if (Vars::Misc::CL_Move::SEnabled.m_Var)
	{
		int SpeedTicks{ 0 };
		int SpeedTicksDesired = Vars::Misc::CL_Move::SFactor.m_Var;
		g_GlobalInfo.m_nShifted = 0;

		while (SpeedTicks < SpeedTicksDesired)
		{
			SpeedTicks++;
			oClMove(accumulated_extra_samples, (SpeedTicks == (SpeedTicksDesired)));
		}
	}

	const auto pLocal = g_EntityCache.m_pLocal;
	static KeyHelper rechargeKey{ &Vars::Misc::CL_Move::RechargeKey.m_Var };

	// Clear tick shift queue
	if (g_GlobalInfo.m_nShifted && !g_GlobalInfo.m_bRecharging && g_GlobalInfo.tickShiftQueue > 0)
	{
		while (g_GlobalInfo.tickShiftQueue > 0 && g_GlobalInfo.m_nShifted > 0)
		{
			oClMove(accumulated_extra_samples, (g_GlobalInfo.tickShiftQueue == 1));
			g_GlobalInfo.m_nShifted--;
			g_GlobalInfo.tickShiftQueue--;
		}
		return;
	}

	if (g_GlobalInfo.m_bRechargeQueued && !g_GlobalInfo.m_bChoking)
	{
		// probably perfect method of waiting to ensure we don't mess with fakelag
		g_GlobalInfo.m_bRechargeQueued = false; // see relevant code @clientmodehook
		g_GlobalInfo.m_bRecharging = true;
		g_GlobalInfo.tickShiftQueue = 0;
	}
	else if (g_GlobalInfo.m_bRecharging && (g_GlobalInfo.m_nShifted < Vars::Misc::CL_Move::DTTicks.m_Var))
	{
		g_GlobalInfo.m_bForceSendPacket = true; // force uninterrupted connection with server
		g_GlobalInfo.m_nShifted++; // add ticks to tick counter
		g_GlobalInfo.m_nWaitForShift = 67 - Vars::Misc::CL_Move::DTTicks.m_Var; // set wait condition (genius)
		return; // this recharges
	}
	else if (rechargeKey.Down())
	{
		// queue recharge
		g_GlobalInfo.m_bForceSendPacket = true;
		g_GlobalInfo.m_bRechargeQueued = true;
	}
	else
	{
		g_GlobalInfo.m_bRecharging = false; // if we are unable to recharge, don't
	}

	oClMove(accumulated_extra_samples,
			(g_GlobalInfo.m_bShouldShift && !g_GlobalInfo.m_nWaitForShift) ? true : bFinalTick);

	if (g_GlobalInfo.m_nWaitForShift && Vars::Misc::CL_Move::WaitForDT.m_Var)
	{
		g_GlobalInfo.m_nWaitForShift--;
		return;
	}

	if (g_GlobalInfo.lateUserCmd != nullptr)
	{
		// Shift if attacking normally
		if (Vars::Misc::CL_Move::NotInAir.m_Var)
		{
			if (pLocal)
			{
				if (pLocal->IsOnGround())
				{
					g_GlobalInfo.m_bShouldShift = g_GlobalInfo.m_bShouldShift
						? true
						: g_GlobalInfo.lateUserCmd->buttons & IN_ATTACK;
				}
				else
				{
					g_GlobalInfo.m_bShouldShift = false;
				}
			}
		}
		else
		{
			g_GlobalInfo.m_bShouldShift = g_GlobalInfo.m_bShouldShift
				? true
				: g_GlobalInfo.lateUserCmd->buttons & IN_ATTACK;
		}
	}


	if (!pLocal)
	{
		g_GlobalInfo.m_nShifted = 0; // we do not have charge if we do not exist
		return;
	}

	if (g_GlobalInfo.m_bShouldShift && !g_GlobalInfo.m_nWaitForShift)
	{
		if (
			(Vars::Misc::CL_Move::DTMode.m_Var == 0 && GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)) ||
			// 0 - On key
			(Vars::Misc::CL_Move::DTMode.m_Var == 1) || // 1 - Always
			(Vars::Misc::CL_Move::DTMode.m_Var == 2 && !GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)))
		// 2 - Disable on key 
		{
			while (g_GlobalInfo.m_nShifted > 0)
			{
				oClMove(accumulated_extra_samples, (g_GlobalInfo.m_nShifted == 1));
				g_GlobalInfo.m_nShifted--;
				//g_GlobalInfo.m_bForceSendPacket = true;
			}
			I::Engine->FireEvents();
			g_GlobalInfo.m_nWaitForShift = DT_WAIT_CALLS;
		}
		g_GlobalInfo.m_bShouldShift = false;
	}
}