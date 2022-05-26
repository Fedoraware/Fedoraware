#include "../Hooks.h"

#include "../../Features/Misc/Misc.h"

MAKE_HOOK(CL_Move, g_Pattern.Find(L"engine.dll", L"55 8B EC 83 EC ? 83 3D ? ? ? ? 02 0F 8C ? ? 00 00 E8 ? ? ? 00 84 C0"), void, __cdecl,
		  float accumulated_extra_samples, bool bFinalTick)
{
	static auto oClMove = Hook.Original<FN>();

	if (!Vars::Misc::CL_Move::Enabled.m_Var)
	{
		G::m_nShifted = 0;
		return oClMove(accumulated_extra_samples, bFinalTick);
	}

	if (G::m_nShifted > Vars::Misc::CL_Move::DTTicks.m_Var)
	{
		G::m_nShifted -= 1;
		oClMove(accumulated_extra_samples, (G::m_nShifted == Vars::Misc::CL_Move::DTTicks.m_Var + 1));
	} // pCode

	// pSpeedhack
	if (Vars::Misc::CL_Move::SEnabled.m_Var)
	{
		int SpeedTicks{ 0 };
		int SpeedTicksDesired = Vars::Misc::CL_Move::SFactor.m_Var;
		G::m_nShifted = 0;

		while (SpeedTicks < SpeedTicksDesired)
		{
			SpeedTicks++;
			oClMove(accumulated_extra_samples, (SpeedTicks == (SpeedTicksDesired)));
		}
	}

	const auto pLocal = g_EntityCache.m_pLocal;
	static KeyHelper rechargeKey{ &Vars::Misc::CL_Move::RechargeKey.m_Var };

	// Clear tick shift queue
	if (G::m_nShifted && !G::m_bRecharging && G::tickShiftQueue > 0)
	{
		while (G::tickShiftQueue > 0 && G::m_nShifted > 0)
		{
			oClMove(accumulated_extra_samples, (G::tickShiftQueue == 1));
			G::m_nShifted--;
			G::tickShiftQueue--;
		}
		return;
	}

	if (G::m_bRechargeQueued && !G::m_bChoking)
	{
		// probably perfect method of waiting to ensure we don't mess with fakelag
		G::m_bRechargeQueued = false; // see relevant code @clientmodehook
		G::m_bRecharging = true;
		G::tickShiftQueue = 0;
	}
	else if (G::m_bRecharging && (G::m_nShifted < Vars::Misc::CL_Move::DTTicks.m_Var))
	{
		G::m_bForceSendPacket = true; // force uninterrupted connection with server
		G::m_nShifted++; // add ticks to tick counter
		G::m_nWaitForShift = 67 - Vars::Misc::CL_Move::DTTicks.m_Var; // set wait condition (genius)
		return; // this recharges
	}
	else if (rechargeKey.Down())
	{
		// queue recharge
		G::m_bForceSendPacket = true;
		G::m_bRechargeQueued = true;
	}
	else
	{
		G::m_bRecharging = false; // if we are unable to recharge, don't
	}

	oClMove(accumulated_extra_samples,
			(G::m_bShouldShift && !G::m_nWaitForShift) ? true : bFinalTick);

	if (G::m_nWaitForShift && Vars::Misc::CL_Move::WaitForDT.m_Var)
	{
		G::m_nWaitForShift--;
		return;
	}

	if (G::lateUserCmd != nullptr)
	{
		// Shift if attacking normally
		if (Vars::Misc::CL_Move::NotInAir.m_Var)
		{
			if (pLocal)
			{
				if (pLocal->IsOnGround())
				{
					G::m_bShouldShift = G::m_bShouldShift
						? true
						: G::lateUserCmd->buttons & IN_ATTACK;
				}
				else
				{
					G::m_bShouldShift = false;
				}
			}
		}
		else
		{
			G::m_bShouldShift = G::m_bShouldShift
				? true
				: G::lateUserCmd->buttons & IN_ATTACK;
		}
	}


	if (!pLocal)
	{
		G::m_nShifted = 0; // we do not have charge if we do not exist
		return;
	}

	if (G::m_bShouldShift && !G::m_nWaitForShift)
	{
		if (
			(Vars::Misc::CL_Move::DTMode.m_Var == 0 && GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)) ||
			// 0 - On key
			(Vars::Misc::CL_Move::DTMode.m_Var == 1) || // 1 - Always
			(Vars::Misc::CL_Move::DTMode.m_Var == 2 && !GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.m_Var)))
		// 2 - Disable on key 
		{
			while (G::m_nShifted > 0)
			{
				oClMove(accumulated_extra_samples, (G::m_nShifted == 1));
				G::m_nShifted--;
				//G::m_bForceSendPacket = true;
			}
			I::Engine->FireEvents();
			G::m_nWaitForShift = DT_WAIT_CALLS;
		}
		G::m_bShouldShift = false;
	}
}