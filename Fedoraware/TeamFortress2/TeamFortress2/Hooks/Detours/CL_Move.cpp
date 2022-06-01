#include "../Hooks.h"

#include "../../Features/Misc/Misc.h"

MAKE_HOOK(CL_Move, g_Pattern.Find(L"engine.dll", L"55 8B EC 83 EC ? 83 3D ? ? ? ? 02 0F 8C ? ? 00 00 E8 ? ? ? 00 84 C0"), void, __cdecl,
		  float accumulated_extra_samples, bool bFinalTick)
{
	static auto oClMove = Hook.Original<FN>();

	if (!Vars::Misc::CL_Move::Enabled.Value)
	{
		G::ShiftedTicks = 0;
		return oClMove(accumulated_extra_samples, bFinalTick);
	}

	if (G::ShiftedTicks > Vars::Misc::CL_Move::DTTicks.Value)
	{
		G::ShiftedTicks -= 1;
		oClMove(accumulated_extra_samples, (G::ShiftedTicks == Vars::Misc::CL_Move::DTTicks.Value + 1));
	} // pCode

	// pSpeedhack
	if (Vars::Misc::CL_Move::SEnabled.Value)
	{
		int SpeedTicks{ 0 };
		int SpeedTicksDesired = Vars::Misc::CL_Move::SFactor.Value;
		G::ShiftedTicks = 0;

		while (SpeedTicks < SpeedTicksDesired)
		{
			SpeedTicks++;
			oClMove(accumulated_extra_samples, (SpeedTicks == (SpeedTicksDesired)));
		}
	}

	const auto pLocal = g_EntityCache.GetLocal();
	static KeyHelper rechargeKey{ &Vars::Misc::CL_Move::RechargeKey.Value };

	// Clear tick shift queue (for teleport)
	if (G::ShiftedTicks && !G::Recharging && G::TickShiftQueue > 0)
	{
		while (G::TickShiftQueue > 0 && G::ShiftedTicks > 0)
		{
			oClMove(accumulated_extra_samples, (G::TickShiftQueue == 1));
			G::ShiftedTicks--;
			G::TickShiftQueue--;
		}
		return;
	}

	// Prepare for a recharge (Is a recharge queued?)
	if (G::RechargeQueued && !G::IsChoking)
	{
		// probably perfect method of waiting to ensure we don't mess with fakelag
		G::RechargeQueued = false; // see relevant code @clientmodehook
		G::Recharging = true;
		G::TickShiftQueue = 0;
	}

	// Recharge ticks
	else if (G::Recharging && (G::ShiftedTicks < Vars::Misc::CL_Move::DTTicks.Value))
	{
		G::ForceSendPacket = true; // force uninterrupted connection with server
		G::ShiftedTicks++; // add ticks to tick counter
		G::WaitForShift = 67 - Vars::Misc::CL_Move::DTTicks.Value; // set wait condition (genius)
		return; // this recharges
	}

	// Queue a recharge if the recharge key was pressed
	else if (rechargeKey.Down())
	{
		// queue recharge
		G::ForceSendPacket = true;
		G::RechargeQueued = true;
	}

	// We're unable to recharge
	else
	{
		G::Recharging = false;
	}

	oClMove(accumulated_extra_samples, (G::ShouldShift && !G::WaitForShift) ? true : bFinalTick);

	if (G::WaitForShift && Vars::Misc::CL_Move::WaitForDT.Value)
	{
		G::WaitForShift--;
		return;
	}

	if (G::LastUserCmd != nullptr)
	{
		// Shift if attacking normally
		if (Vars::Misc::CL_Move::NotInAir.Value)
		{
			if (pLocal)
			{
				if (pLocal->IsOnGround())
				{
					G::ShouldShift = G::ShouldShift
						? true
						: G::LastUserCmd->buttons & IN_ATTACK;
				}
				else
				{
					G::ShouldShift = false;
				}
			}
		}
		else
		{
			G::ShouldShift = G::ShouldShift
				? true
				: G::LastUserCmd->buttons & IN_ATTACK;
		}
	}


	if (!pLocal)
	{
		G::ShiftedTicks = 0; // we do not have charge if we do not exist
		return;
	}

	// Should we shift?
	if (G::ShouldShift && !G::WaitForShift)
	{
		if (// 0 - On key
			(Vars::Misc::CL_Move::DTMode.Value == 0 && GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.Value)) ||

			// 1 - Always
			(Vars::Misc::CL_Move::DTMode.Value == 1) ||

			// 2 - Disable on key 
			(Vars::Misc::CL_Move::DTMode.Value == 2 && !GetAsyncKeyState(Vars::Misc::CL_Move::DoubletapKey.Value)))
		{
			while (G::ShiftedTicks > 0)
			{
				oClMove(accumulated_extra_samples, (G::ShiftedTicks == 1));
				G::ShiftedTicks--;
				//G::m_bForceSendPacket = true;
			}

			I::Engine->FireEvents();
			G::WaitForShift = DT_WAIT_CALLS;
		}
		G::ShouldShift = false;
	}
}